/*
  DXL APIを使う
   デバイスの検索
*/

#include  <stdio.h>

#include  "dxlib.h"
#include  "dxmisc.h"

//===========================================================
// メイン
//===========================================================
int main (void) {
  TDeviceID dev = DX_OpenPort (_COMPORT, _BAUDRATE);  // ポートオープン

  if (dev != 0) {
    printf ("Open success\n");

    // 以下のいずれかの方法でAPI内のIDリストを更新
#if 1
    printf("scanning devices...");
    int n = DXL_ScanDevices (dev, NULL);
    printf(" %d device detected.\n", n);
    DXL_PrintDevicesList ((void *)&printf);
#else
    for (uint8_t id = 0; id <= 253; id++) {
      PDXL_ModelInfo p = DXL_GetModelInfo (dev, id);
      if (p->modelno != 0) {
        printf ("[%3d] %-16s ($%04X) %d\n", id, p->name, p->modelno, p->devtype);
      }
    }
#endif

    DX_ClosePort (dev);   // ポートクローズ
  } else {
    printf ("Open error\n");
  }
}
