/*
  DXL API���g��
   �������֓d���w��
*/

#include  <stdio.h>

#include  "dxlib.h"
#include  "dxmisc.h"

// �������̌��݊p�x�Ɗp���x���w�莞��(ms)���j�^��������
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
    printf ("Successful opening of %s\n", _COMPORT);

    // �ڑ����ꂽ�S�f�o�C�X�����������̈ꗗ��\��
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

    // �d���p�ϐ�
    double cur[num];

    // Current���[�h�ɐݒ�
    printf("\nSet OPMode(0)=%s\n", DXL_SetOperatingModesEquival (dev, ids, num, 0) ? "True" : "False");

    // �g���N�C�l�[�u��
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

    // Position���[�h�ɐݒ�
    printf("\nSet OPMode(3)=%s\n", DXL_SetOperatingModesEquival (dev, ids, num, 3) ? "True" : "False");
    DX_ClosePort (dev);
  } else printf ("Failed to open %s\n", _COMPORT);
}
