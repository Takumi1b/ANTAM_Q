/*
  PING2でネットワーク上の対象を検索
*/

#include  <stdio.h>
#ifdef _WIN32
#include  <conio.h>
#else
#include  "kbhit.h"
#endif

#include  "dxlib.h"
// サンプル用のポート及びIDのマクロ宣言
#include  "dxmisc.h"

int main (void) {
  TDeviceID   dev;
  TErrorCode  err;
  uint32_t    num;          // 最大検索数と検索結果
  TDxAlarmStatus stat[254]; // 検索結果の保存先

  if ((dev = DX_OpenPort (_COMPORT, _BAUDRATE))) {
    printf ("Successful opening of %s\n", _COMPORT);

    for (int b = 0; (b <= 254) && !kbhit (); b++){
      uint32_t baud = 2000000 / (b + 1);
      DX_SetBaudrate (dev, baud);
      printf("\rBaud(%d)=%d[bps]         ", b, baud);

      num = 254;

      if (DX_Ping2 (dev, &num, stat, &err)) {
        printf ("\n %d device found\n", num);
        for (int i = 0; i < num; i++)
          printf (" Found ID=%d stat:$%04X\n", stat[i].id, stat[i].Status);
      }
    }
    DX_ClosePort (dev);
  } else printf ("Failed to open %s\n", _COMPORT);

  printf ("Fin\n");
}
