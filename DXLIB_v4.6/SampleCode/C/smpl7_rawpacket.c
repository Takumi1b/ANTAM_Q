/*
  生パケットの送受信

  ターゲット:DX/AX/RX/EX/MXシリーズ
*/

#include  <stdio.h>
#ifdef _WIN32
#include  <conio.h>
#else
#include  "kbhit.h"
#endif

#include  "dxlib.h"
#include  "dxmisc.h"
#include  "dxmemmap.h"

// マクロ定義
#define TIMEOUT     (50)            // 受信タイムアウト [ms]

#define MAX_POS     (1023)
#define MIN_POS     (0)

//===========================================================
// メイン
//===========================================================
int main (void) {
  TDeviceID dev;
  TErrorCode err;
  uint32_t l;
  uint8_t  param[10], rbuf[50];

  if ((dev = DX_OpenPort (_COMPORT, _BAUDRATE))) {
    printf ("Successful opening of %s\n", _COMPORT);

    while (!kbhit () && DX_Active (dev)) {
      printf ("TxPacket INST_WRITE\n");
      param[0] = ADDRESS_LED;
      param[1] = 1;
      if (DX_TxPacket (dev, _TARGETID, INST_WRITE, param, 2, &err)) {
        printf ("RxPacket\n");
        if (!DX_RxPacket (dev, rbuf, sizeof (rbuf), &l, TIMEOUT, &err))
          printf ("error [$%04X]\n", err);
        for (int i = 0; i < l; i++) printf ("[%02X]", rbuf[i]);
        printf ("\n");
      } else
        printf ("error [$%04X]\n", err);

      printf ("TxPacket INST_READ\n");
      param[0] = ADDRESS_LED;
      param[1] = 5;
      if (DX_TxPacket (dev, _TARGETID, INST_READ, param, 2, &err)) {
        printf ("RxPacket\n");
        if(!DX_RxPacket (dev, rbuf, sizeof (rbuf), &l, TIMEOUT, &err))
          printf ("error [$%04X]\n", err);
        for (int i = 0; i < l; i++) printf ("[%02X]", rbuf[i]);
        printf ("\n");
      } else
        printf ("error [$%04X]\n", err);

      Sleep(1000);

      printf ("TxPacket INST_WRITE\n");
      param[0] = ADDRESS_LED;
      param[1] = 0;
      if (DX_TxPacket (dev, _TARGETID, INST_WRITE, param, 2, &err)) {
        printf ("RxPacket\n");
        if (!DX_RxPacket (dev, rbuf, sizeof (rbuf), &l, TIMEOUT, &err))
          printf ("error [$%04X]\n", err);
        for (int i = 0; i < l; i++) printf ("[%02X]", rbuf[i]);
        printf ("\n");
      } else
        printf ("error [$%04X]\n", err);

      printf ("TxPacket INST_READ\n");
      param[0] = ADDRESS_LED;
      param[1] = 10;
      if (DX_TxPacket (dev, _TARGETID, INST_READ, param, 2, &err)) {
        printf ("RxPacket\n");
        if (!DX_RxPacket (dev, rbuf, sizeof (rbuf), &l, TIMEOUT, &err))
          printf ("error [$%04X]\n", err);
        for (int i = 0; i < l; i++) printf ("[%02X]", rbuf[i]);
        printf ("\n");
      } else
        printf ("error [$%04X]\n", err);

      Sleep(1000);
    }

    DX_ClosePort (dev);
  } else printf ("Failed to open %s\n", _COMPORT);

  printf ("Fin\n");
}
