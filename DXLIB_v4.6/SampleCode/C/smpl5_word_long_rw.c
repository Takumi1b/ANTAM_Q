/*
  16/32bitアイテムへのアクセス

  モデルを間違えてコントロールテーブルにアクセスすると危険なため、
  対象のモデルが何であるかを判別してから処理を行う。
*/

#include  <stdio.h>
#include  <string.h>
#ifdef _WIN32
#include  <conio.h>
#else
#include  "kbhit.h"
#endif

#include  "dxlib.h"
#include  "dxmisc.h"
#include  "dxmemmap.h"

#define MAX_POS     (1023)
#define MIN_POS     (0)

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
  uint16_t    adr_max = 0, adr_min = 0, adr_ppos = 0, adr_pos = 0, adr_ten = 0, dataw = 0; // 各アイテムのアドレス
  int32_t     maxpos, maxpos32, minpos, minpos32, diffpos, pos, pos32;
  int16_t     maxpos16, minpos16, pos16;
  TDeviceID   dev;
  TErrorCode  err;

  char comname[20];
  uint32_t baud;
  uint8_t id;

  EditComAndBaudAndTarget (comname, &baud, &id);
  printf("\nCOM=%s, Baudrate=%d, id=%d\n", comname, baud, id);

  if ((dev = DX_OpenPort (comname, baud))) {
    printf ("Successful opening of %s\n", comname);

    uint16_t modelno;
    // モデル番号からシリーズを判定し使用するアドレスを変更
    if (DX_ReadWordData (dev, id, 0, &modelno, NULL)) {
      switch (CheckType (modelno)) {
        case devtDX:
        case devtAX:
        case devtEX:
        case devtMX:
          adr_max  = ADDRESS_CCW_ANGLE_LIMIT;
          adr_min  = ADDRESS_CW_ANGLE_LIMIT;
          adr_ppos = ADDRESS_PRESENT_POSITION;
          adr_ten  = ADDRESS_TORQUE_ENABLE;
          adr_pos  = ADDRESS_GOAL_POSITION;
          dataw    = 2;
          break;
        case devtX:
          adr_max  = ADDRESS_X_MAX_POSITION_LIMIT;
          adr_min  = ADDRESS_X_MIN_POSITION_LIMIT;
          adr_ppos = ADDRESS_X_PRESENT_POSITION;
          adr_ten  = ADDRESS_X_TORQUE_ENABLE;
          adr_pos  = ADDRESS_X_GOAL_POSITION;
          dataw    = 4;
          break;
        default:
          adr_max  = 0;
          adr_min  = 0;
          adr_ppos = 0;
          adr_ten  = 0;
          adr_pos  = 0;
          dataw    = 0;
          break;
      }
    }
    if (dataw != 0) {
      // 現在の位置を読み出し
      if (
        (dataw == 2) ? (
          // 最大角度取得
          DX_ReadWordData (dev, id, adr_max, (uint16_t *)&maxpos16, &err) &&
          // 最小角度取得
          DX_ReadWordData (dev, id, adr_min, (uint16_t *)&minpos16, &err) &&
          // 現在角度取得
          DX_ReadWordData (dev, id, adr_ppos, (uint16_t *)&pos16, &err) &&
          // トルクイネーブル
          DX_WriteByteData (dev, id, adr_ten, 1, &err)
        ) : (
          // 最大角度取得
          DX_ReadLongData (dev, id, adr_max, (uint32_t *)&maxpos32, &err) &&
          // 最小角度取得
          DX_ReadLongData (dev, id, adr_min, (uint32_t *)&minpos32, &err) &&
          // 現在角度取得
          DX_ReadLongData (dev, id, adr_ppos, (uint32_t *)&pos32, &err) &&
          // トルクイネーブル
          DX_WriteByteData (dev, id, adr_ten, 1, &err)
       )
      ) {
        pos = (dataw == 2) ? pos16 : pos32;
        maxpos = (dataw == 2) ? maxpos16 : maxpos32;
        minpos = (dataw == 2) ? minpos16 : minpos32;
        diffpos = (maxpos - minpos) / 1000;
        printf ("maxpos=%d, minpos=%d, presentpos=%d\n", maxpos, minpos, pos);
        // 何かキーを押すとループを抜ける
        while (!kbhit ()) {
          pos += diffpos;
          if (pos >= maxpos) {
            pos = maxpos;
            diffpos *= -1;
          } else if (pos <= minpos) {
            pos = minpos;
            diffpos *= -1;
          }
          // 目標位置を書き込み
          if (
            dataw == 2 ?
            DX_WriteWordData (dev, id, adr_pos, (uint16_t)pos, &err) :
            DX_WriteLongData (dev, id, adr_pos, (uint32_t)pos, &err)
          )
            printf ("Sent position = %4d\r", pos);
          else
            printf ("Sent position error [$%04X]\n", err);
          // 1ミリ秒待ち
          Sleep (1);
        }
        // トルクディスエーブル
        DX_WriteByteData (dev, id, adr_ten, 0, &err);
        printf ("\n");
      }
    } else printf ("Not supported device ID=%d\n", id);

    DX_ClosePort (dev);
  } else printf ("Failed to open %s\n", comname);

#ifndef _WIN32
  disable_raw_mode ();
#endif

  printf ("Fin\n");
}
