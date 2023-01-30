/*
  DXL APIを使う
   複数軸へ電流指令
*/

#include  <stdio.h>

#include  "dxlib.h"
#include  "dxmisc.h"

// 複数軸の現在角度と角速度を指定時間(ms)モニタし続ける
bool mon (TDeviceID dev, const uint8_t *ids, int num, double tim) {
  bool    result = false;
  double  t = GetQueryPerformanceCounter() + tim;
  double  pangle[num], pvelo[num];
  while (t > GetQueryPerformanceCounter()) {
    for (int i = 0; i < num; i++) { pangle[i] = pvelo[i] = 0.0; }
    DXL_GetPresentAngles (dev, ids, pangle, num);
    DXL_GetPresentVelocities (dev, ids, pvelo, num);
    for (int i = 0; i < num; i++) {
      printf ("[%3d:$%04X]%7.1f %6.1f ", ids[i], DXL_GetErrorCode (dev, ids[i]), pangle[i], pvelo[i]);
    }
    printf("\r");
  }
  return result;
}

//===========================================================
// メイン
//===========================================================
int main (void) {
  // 検出したDynamixelのID一覧
  uint8_t ids[254];
  // 検出したDynamixel数
  int num = 0;

  TDeviceID dev = DX_OpenPort (_COMPORT, _BAUDRATE);
  if (dev != 0) {
    printf ("Successful opening of %s\n", _COMPORT);

    // 接続された全デバイスを検索しその一覧を表示
    printf ("scanning devices...");

#if 1
    for (uint8_t id = 0; id <= 16; id++) {
      PDXL_ModelInfo p = DXL_GetModelInfo (dev, id);
      if (p->modelno >= 2) {
        printf ("\n[%3d] %s ($%04X) %d", id, p->name, p->modelno, p->devtype);
        ids[num++] = id;
      }
    }
#else
    num = DXL_ScanDevices (dev, ids);
    printf ("\n %d device detected.\n", num);
    DXL_PrintDevicesList (&printf);
#endif

    // 電流用変数
    double cur[num];

    // Currentモードに設定
    printf("\nSet OPMode(0)=%s\n", DXL_SetOperatingModesEquival (dev, ids, num, 0) ? "True" : "False");

    // トルクイネーブル
    bool ten[num];
    for (int i = 0; i < num; i++) ten[i] = true;
    DXL_SetTorqueEnables (dev, ids, ten, num);

    for (int i = 0; i < num; i++) cur[i] = -1980;
    DXL_SetGoalCurrents (dev, ids, cur, num);
    mon (dev, ids, num, 3000);
    for (int i = 0; i < num; i++) cur[i] = 1980;
    DXL_SetGoalCurrents (dev, ids, cur, num);
    mon (dev, ids, num, 3000);
    for (int i = 0; i < num; i++) cur[i] = 30;
    DXL_SetGoalCurrents (dev, ids, cur, num);
    mon (dev, ids, num, 3000);
    for (int i = 0; i < num; i++) cur[i] = -60;
    DXL_SetGoalCurrents (dev, ids, cur, num);
    mon (dev, ids, num, 3000);

    for (int i = 0; i < num; i++) ten[i] = false;
    DXL_SetTorqueEnables (dev, ids, ten, num);

    // Positionモードに設定
    printf("\nSet OPMode(3)=%s\n", DXL_SetOperatingModesEquival (dev, ids, num, 3) ? "True" : "False");
    DX_ClosePort (dev);
  } else printf ("Failed to open %s\n", _COMPORT);
}
