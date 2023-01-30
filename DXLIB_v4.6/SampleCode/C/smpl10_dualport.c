/*
  複数I/Fの同時使用

  ターゲット:DX/AX/RX/EX/MXシリーズ
*/

#include  <stdio.h>
#ifdef _WIN32
#include  <conio.h>
#include  <process.h>
#else
#include  "kbhit.h"
#include  <pthread.h>
#endif

#include  "dxlib.h"
#include  "dxmisc.h"
#include  "dxmemmap.h"

// マクロ定義
#define MAX_POS     (1023)
#define MIN_POS     (0)

bool terminate = false;

//===========================================================
// 現在位置読み出し
//===========================================================
bool DxGetPosition (TDeviceID dvid, uint8_t id, uint16_t *pos, TErrorCode *err) {
  return DX_ReadWordData (dvid, id, ADDRESS_PRESENT_POSITION, (uint16_t *)pos, err);
}

//===========================================================
// 目標位置書き込み
//===========================================================
bool DxSetPosition (TDeviceID dvid, uint8_t id, uint16_t pos, TErrorCode *err) {
  return DX_WriteWordData (dvid, id, ADDRESS_GOAL_POSITION, (uint16_t)pos, err);
}

//===========================================================
// スレッド
//===========================================================
#ifdef _WIN32
unsigned __stdcall thread (void *dum) {
#else
void *thread (void *dum) {
#endif
  TDeviceID   dev;
  TErrorCode  err;
  int16_t     pos = 511;
  uint16_t    ppos;
  int         f = 1;

  if ((dev = DX_OpenPort (_COMPORT2, _BAUDRATE))) {
    printf ("Successful opening of %s\n", _COMPORT2);
  } else {
    printf ("Failed to open %s\n", _COMPORT2);
  }

  while (!terminate) {
    if (DX_Active (dev)) {
      pos += f;
      if (pos >= MAX_POS) { pos = MAX_POS; f *= -1; }
      else if (pos <= MIN_POS) { pos = MIN_POS; f *= -1; }

      if (!DxSetPosition (dev, _TARGETID2, pos, &err))
        printf ("\nSetPos (%s, %d) error [%04X]", _COMPORT2, _TARGETID2, err);
      if (DxGetPosition (dev, _TARGETID2, &ppos, &err))
        printf ("\nGetPos (%s, %d) = %d", _COMPORT2, _TARGETID2, ppos);
      else
        printf ("\nGetPos (%s, %d) error [%04X]", _COMPORT2, _TARGETID2, err);
    }
    Sleep (10);
  }

  if (dev) DX_ClosePort (dev);

#ifdef _WIN32
  _endthreadex (0);
  return 0;
#else
  return NULL;
#endif
  return 0;
}

//===========================================================
// メイン
//===========================================================
int main (void) {
  TDeviceID   dev;
  TErrorCode  err;
#ifdef _WIN32
  HANDLE      mythread;
#else
  pthread_t   mythread;
#endif

  // _COMPORT2の処理を別スレッドで
#ifdef _WIN32
  if ((mythread = (HANDLE)_beginthreadex (NULL, 0, &thread, 0, 0, NULL)) == INVALID_HANDLE_VALUE) {
#else
  int ret;
  if ((ret = pthread_create (&mythread, NULL, thread, NULL))) {
#endif
    printf ("beginthread error\n");
  }

  // _COMPORTの処理をmainで
  if ((dev = DX_OpenPort (_COMPORT, _BAUDRATE))) {
    printf ("Successful opening of %s\n", _COMPORT);
  } else {
    printf ("Failed to open %s\n", _COMPORT);
  }

  Sleep (1000);

  int16_t   pos = 511;
  uint16_t  ppos;
  int       f = 1;
  while (!kbhit ()) {
    if (DX_Active (dev)) {
      pos += f;
      if (pos >= MAX_POS) { pos = MAX_POS; f *= -1; }
      else if (pos <= MIN_POS) { pos = MIN_POS; f *= -1; }

      if (!DxSetPosition (dev, _TARGETID, pos, &err))
        printf ("\nSetPos (%s, %d) error [%04X]", _COMPORT, _TARGETID, err);
      if (DxGetPosition (dev, _TARGETID, &ppos, &err))
        printf ("\nGetPos (%s, %d) = %d", _COMPORT, _TARGETID, ppos);
      else
        printf ("\nGetPos (%s, %d) error [%04X]", _COMPORT, _TARGETID, err);
    }
    Sleep(10);
  }

  if (dev) DX_ClosePort (dev);

#ifdef _WIN32
  if (mythread != INVALID_HANDLE_VALUE) {
    terminate = true;
    WaitForSingleObject (mythread, 1000);
  }
#else
  if (ret == 0) {
    terminate = true;
    pthread_join (mythread, NULL);
  }
#endif

  printf ("Fin\n");
}
