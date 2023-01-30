/*
  DXL API���g��
   �p�x�w��
*/

#include  <stdio.h>

#include  "dxlib.h"
#include  "dxmisc.h"

//===========================================================
// ���C��
//===========================================================
int main (void) {
  TDeviceID dev = DX_OpenPort (_COMPORT, _BAUDRATE);

  if (dev != 0) {
    printf ("Successful opening of %s\n", _COMPORT);

    // �w��ID�̃f�o�C�X�����擾
    // ���f��No.��������Dynamixel�ƈ�v���Ȃ��ƈȌ�@�\���Ȃ�
    printf ("[id:%d] Model Name=%s\n", _TARGETID, DXL_GetModelInfo (dev, _TARGETID)->name);

    // �g���N�f�B�X�G�[�u��
    DXL_SetTorqueEnable (dev, _TARGETID, false);
    Sleep (100);
    // Position���[�h�ɐݒ�
    if (DXL_SetOperatingMode (dev, _TARGETID, 3)) {
      // LED�_��
      DXL_SetLED (dev, _TARGETID, true);
      // �g���N�C�l�[�u��
      DXL_SetTorqueEnable (dev, _TARGETID, true);

      DXL_SetGoalAngle (dev, _TARGETID, 90.0);   // 90deg
      Sleep (1000);
      DXL_SetGoalAngle (dev, _TARGETID, -90.0);  // -90deg
      Sleep (1000);
      DXL_SetGoalAngle (dev, _TARGETID, 0.0);    // 0deg
      Sleep (1000);

      // �g���N�f�B�X�G�[�u��
      DXL_SetTorqueEnable (dev, _TARGETID, false);
      // LED����
      DXL_SetLED (dev, _TARGETID, false);
    } else {
      printf ("SetOPmode error\n");
    }
    DX_ClosePort (dev);
  } else printf ("Failed to open %s\n", _COMPORT);
}