/*
  8bit幅のアイテムへのアクセス

  モデルを間違えてコントロールテーブルにアクセスすると危険なため、
  対象のモデルが何であるかを判別してから処理を行う。
*/

#include  <stdio.h>
#include  <string.h>
#ifndef _WIN32
#include  "kbhit.h"
#endif

#include  "dxlib.h"
#include  "dxmisc.h"
#include  "dxmemmap.h"

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
  TDeviceID   dev;
  TErrorCode  err;
  uint8_t     led;

  char comname[20];
  uint32_t baud;
  uint8_t id;

  // デフォルトの設定から初期条件を変更
  EditComAndBaudAndTarget (comname, &baud, &id);
  printf ("\nCOM=%s, Baudrate=%d, id=%d\n", comname, baud, id);

  if ((dev = DX_OpenPort (comname, baud))) {
    printf ("Successful opening of %s\n", comname);

    uint16_t modelno, addr_led;
    // モデル番号からシリーズを判定し使用するアドレスを変更
    if (DX_ReadWordData (dev, id, 0, &modelno, NULL)) {
      switch (CheckType (modelno)) {
        case devtDX:
        case devtAX:
        case devtEX:
        case devtMX:    addr_led = ADDRESS_LED;       break;
        case devtX:     addr_led = ADDRESS_X_LED_RED; break;
        default:        addr_led = 0;                 break;
      }
      if (addr_led != 0) {
        for (int i = 0; i < 10; i++) {
          // LEDの状態を読み出し
          if (DX_ReadByteData (dev, id, addr_led, &led, &err))
            printf ("Read LED=%d\n", led);
          else
            printf ("Read error [$%04X]\n", err);

          // 読み出したLEDの値を反転
          led ^= 1;

          // LEDへ値を書き込み
          if (DX_WriteByteData (dev, id, addr_led, led, &err))
            printf ("Write LED=%d\n", led);
          else
            printf ("Write error [$%04X]\n", err);

          // 0.5秒待ち
          Sleep (500);
        }
      } else printf ("Not supported device ID=%d\n", id);
    } else printf ("Cannot read model no.\n");

    DX_ClosePort (dev);
  } else printf ("Failed to open %s\n", comname);

#ifndef _WIN32
  disable_raw_mode ();
#endif

  printf ("Fin\n");
}
