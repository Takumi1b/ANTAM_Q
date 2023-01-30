/*
  DXL API���g��
   �������ւ̊p�x�E�p���x�w��
*/

#include  <stdio.h>

#include  "dxlib.h"
#include  "dxmisc.h"

//===========================================================
// �������̌��݊p�x�Ɗp���x���w�莞��(ms)���j�^��������
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
// ���C��
//===========================================================
int main (void) {
  // ���o����Dynamixel��ID�ꗗ
  uint8_t ids[254];
  // ���o����Dynamixel��
  int num = 0;

  TDeviceID dev = DX_OpenPort (_COMPORT, _BAUDRATE);
  if (dev != 0) {
    printf ("Open success\n");

    // �ڑ����ꂽ�S�f�o�C�X�����������̈ꗗ��\��
    printf ("scanning devices...");
    num = DXL_ScanDevices (dev, ids);
    printf (" %d device detected.\n", num);
    DXL_PrintDevicesList (&printf);

    // �p�x�E�p���x�����w�ߗp�ϐ�
    TAngleVelocity AngleVelocity[num];
    // Position���[�h�ɐݒ�
    if (DXL_SetOperatingModesEquival (dev, ids, num, 3)) {
      // ����J�n
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

      // �����~
      DXL_SetTorqueEnablesEquival (dev, ids, num, false);
    }

    DX_ClosePort (dev);
  }
}
