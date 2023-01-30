/*
  DXL API���g��
   �p���x�w��
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
    // Velocity���[�h�ɐݒ�
    if (DXL_SetOperatingMode (dev, _TARGETID, 1)) {
      // LED�_��
      DXL_SetLED (dev, _TARGETID, true);
      // �g���N�C�l�[�u��
      DXL_SetTorqueEnable (dev, _TARGETID, true);

      DXL_SetGoalVelocity (dev, _TARGETID, 180.0);   // 180deg/s
      Sleep (5000);
      DXL_SetGoalVelocity (dev, _TARGETID, -180.0);  // -180deg/s
      Sleep (5000);
      DXL_SetGoalVelocity (dev, _TARGETID, 0.0);     // 0rpm
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
