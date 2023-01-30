/*
  DXL APIを使う
   複数軸への角度・角速度指令
*/

#include  <stdio.h>

#include  "dxlib.h"
#include  "dxmisc.h"

//===========================================================
// 複数軸の現在角度と角速度を指定時間(ms)モニタし続ける
//===========================================================
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
    printf ("Open success\n");

    // 接続された全デバイスを検索しその一覧を表示
    printf ("scanning devices...");
    num = DXL_ScanDevices (dev, ids);
    printf (" %d device detected.\n", num);
    DXL_PrintDevicesList (&printf);

    // 角度・角速度同時指令用変数
    TAngleVelocity AngleVelocity[num];
    // Positionモードに設定
    if (DXL_SetOperatingModesEquival (dev, ids, num, 3)) {
      // 制御開始
      DXL_SetTorqueEnablesEquival (dev, ids, num, true);

      for (int i = 0; i < num; i++) AngleVelocity[i].angle = 90;   // 90deg
      DXL_SetGoalAnglesAndVelocities (dev, ids, AngleVelocity, num);
      mon (dev, ids, num, 3000);
      for (int i = 0; i < num; i++) AngleVelocity[i].angle = 0;    // 0deg
      DXL_SetGoalAnglesAndVelocities (dev, ids, AngleVelocity, num);
      mon (dev, ids, num, 3000);
      for (int i = 0; i < num; i++) AngleVelocity[i].angle = -90;  // -90deg
      DXL_SetGoalAnglesAndVelocities (dev, ids, AngleVelocity, num);
      mon (dev, ids, num, 3000);
      for (int i = 0; i < num; i++) AngleVelocity[i].angle = 0;    // 0deg
      DXL_SetGoalAnglesAndVelocities (dev, ids, AngleVelocity, num);
      mon (dev, ids, num, 3000);

      // 制御停止
      DXL_SetTorqueEnablesEquival (dev, ids, num, false);
    }

    DX_ClosePort (dev);
  }
}
