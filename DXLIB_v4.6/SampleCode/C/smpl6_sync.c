/*
  SYNCインストラクションで複数台へ同時に位置指令

  ターゲット:DX/AX/RX/EX/MXシリーズ
*/

#include <stdio.h>
#include  <string.h>
#ifdef _WIN32
#include  <conio.h>
#else
#include  "kbhit.h"
#endif

#include  "dxlib.h"
#include  "dxmisc.h"
#include  "dxmemmap.h"

#define MAX_POS   (1023)
#define MIN_POS   (0)
#define DIFFPOS   (2)

#define AXISNUM   (2)   // 対象となる軸数(_TARGETID=1だとしたら2,3,4...と連続しているものとし全軸存在している必要がある

//===========================================================
// 実行時にポート番号・ボーレート・IDを選択
//===========================================================
bool InputNum (int32_t *num) {
  char buf[100];
#ifndef _WIN32
  disable_raw_mode ();
#endif
  fgets (buf, sizeof(buf), stdin);
  rewind (stdin);
#ifndef _WIN32
  enable_raw_mode ();
#endif
  return (strlen(buf) && sscanf(buf, "%d", num) == 1);
}

bool InputStr (char *str) {
  char buf[100];
#ifndef _WIN32
  disable_raw_mode ();
#endif
  fgets (buf, sizeof(buf), stdin);
  rewind (stdin);
#ifndef _WIN32
  enable_raw_mode ();
#endif
  return (strlen(buf) && sscanf(buf, "%s", str) == 1);
}

bool EditComAndBaudAndTarget (char *comport, uint32_t *baud, uint8_t *id) {
  bool result = true;

  int32_t n;

  printf ("Input comport name (default %s) = ", _COMPORT);
  if (!(result = InputStr (comport))) result = strcpy (comport, _COMPORT);

  printf ("Input baudrate (default %dbps) = ", _BAUDRATE);
  if (!InputNum (&n)) { n = _BAUDRATE; result = false; }
  if (n >= 9600 && n <= 4000000) *baud = n; else *baud = _BAUDRATE;

  printf ("Input target id (default %d) = ", _TARGETID);
  if (!InputNum (&n)) { n = _TARGETID; result = false; }
  if (n >= 1 && n < 0xfd) *id = n; else *id = _TARGETID;

  return result;
}

//===========================================================
// メイン
//===========================================================
int main (void) {
  int         f = DIFFPOS;
  uint16_t    pos = 511;
  TDeviceID   dev;
  TErrorCode  err;

  char comname[20];
  uint32_t baud;
  uint8_t id;

  EditComAndBaudAndTarget (comname, &baud, &id);
  printf("\nCOM=%s, Baudrate=%d, id=%d\n", comname, baud, id);

  if ((dev = DX_OpenPort (comname, baud))) {
    printf ("Successful opening of %s\n", comname);

    uint8_t param[250];
    // 何かキーを押すとループを抜ける
    while (!kbhit () && DX_Active (dev)) {
      param[0] = ADDRESS_GOAL_POSITION;   // アイテムのアドレス
      param[1] = 2;                       // アイテムのサイズ(ここではWORDなので2byte)
      for (int i = 0; i < AXISNUM; i++) { // _TARGETID～を対象とする
        param[2 + i * 3] = i + _TARGETID; // ID
        param[3 + i * 3] = pos & 0xff;    // 値の下位
        param[4 + i * 3] = pos >> 8;      // 値の上位
      }

      if (DX_WriteSyncData (dev, param, 2 + 3 * AXISNUM, &err))
        printf("\rSend success (%4d)  ", pos);
      else
        printf("\rSend error   [$%04X] ", err);

      pos += f;
      if (pos >= MAX_POS) { pos = MAX_POS; f *= -1; }
      else if (pos <= MIN_POS) { pos = MIN_POS; f *= -1; }

      // 5ミリ秒待ち
      Sleep (5);
    }

    DX_ClosePort(dev);
  } else printf ("Failed to open %s\n", comname);

#ifndef _WIN32
  disable_raw_mode ();
#endif

  printf( "\nFin");
}
