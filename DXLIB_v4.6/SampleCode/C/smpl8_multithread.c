/*
  マルチスレッドで1つのターゲットへの非同期アクセス

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
unsigned __stdcall testThread (void *pdev) {
#else
void *testThread (void *pdev) {
#endif
  TDeviceID   dev = (TDeviceID)pdev;
  TErrorCode  err;
  uint16_t    p;

  while (!terminate && DX_Active (dev)) {
    // 現在位置の読み取り
    DxGetPosition (dev, _TARGETID, &p, &err) ? printf ("\rGetPos=%4d     ", p) : printf ("\rGetPos error [$%04X]", err);
    Sleep (100);
  }

#ifdef _WIN32
  _endthreadex (0);
  return 0;
#else
  return NULL;
#endif
}

//===========================================================
// メイン
//===========================================================
int main (void) {
  TDeviceID   dev;
  TErrorCode  err;
  uint16_t    pos;
  int f = 1;
#ifdef _WIN32
  HANDLE      mythread;
#else
  pthread_t   mythread;
#endif

  if ((dev = DX_OpenPort (_COMPORT, _BAUDRATE))) {
    printf ("Successful opening of %s\n", _COMPORT);

    // 現在の位置を読み出し
    if (DX_ReadWordData (dev, _TARGETID, ADDRESS_PRESENT_POSITION, &pos, &err)) {
      // 読み出し専用スレッド作成
#ifdef _WIN32
      if ((mythread = (HANDLE)_beginthreadex (NULL, 0, &testThread, (void *)dev, 0, NULL)) == INVALID_HANDLE_VALUE) {
#else
      if (pthread_create (&mythread, NULL, testThread, (void *)dev)) {
#endif
        printf ("Beginthread error\n");
      } else {
        // 何かキーを押すとループを抜ける
        while (!kbhit () && DX_Active (dev)) {
          pos += f;
          if (pos >= MAX_POS) { pos = MAX_POS; f *= -1; }
          else if (pos <= MIN_POS) { pos = MIN_POS; f *= -1; }
          if (!DxSetPosition (dev, _TARGETID, pos, &err)) printf ("\nSetPos error [$%04X]\n", err);

          Sleep (1);
        }
        // スレッド終了
        terminate = true;

#ifdef _WIN32
        WaitForSingleObject (mythread, 2000);
        CloseHandle (mythread);
#else
        pthread_join (mythread, NULL);
#endif
      }
    }

    DX_ClosePort(dev);
  } else printf ("Failed to open %s\n", _COMPORT);

  printf( "\nFin");
}
