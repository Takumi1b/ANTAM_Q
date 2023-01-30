/*
  Dynamixel�ȊO�̃^�[�Q�b�g

  �^�[�Q�b�g:DXMIO with IMU
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

// �}�N����`
#define _DXMIO_BAUD  (1000000)
#define _DXMIO_ID    (200)

// IMU data
typedef struct {
  int16_t acc_x;    // Acceleration Data
  int16_t acc_y;
  int16_t acc_z;
  int16_t mag_x;    // Magnetometer Data
  int16_t mag_y;
  int16_t mag_z;
  int16_t gyro_x;   // Gyroscope Data
  int16_t gyro_y;
  int16_t gyro_z;
  int16_t heading;  // Euler Data
  int16_t roll;
  int16_t pitch;
  int16_t q_w;      // Quaternion Data
  int16_t q_x;
  int16_t q_y;
  int16_t q_z;
  int16_t lia_x;    // Linear Acceleration Data
  int16_t lia_y;
  int16_t lia_z;
  int16_t grv_x;    // Gravity Vector Data
  int16_t grv_y;
  int16_t grv_z;
  int8_t  temp;     // Temperature
} _PACKED_ Tbno055;

// Control table
typedef struct {
  uint16_t  ModelNumber;      // ���f���i���o�[ (R)
  uint8_t   FirmwareVersion;  // �t�@�[���E�F�A�o�[�W���� (R)
  uint8_t   ID;               // ID (R/W & NVM)
  uint8_t   Baudrate;         // �{�[���[�g (R/W & NVM)
  uint8_t   WriteNVM;         // �s�����f�[�^�������݃t���O (R/W)

  uint8_t   LED;              // LED2  (R/W)
  uint8_t   Terminator;       // �I�[��R (R/W & NVM)
  uint8_t   PinCfg[12];       // GPIO�[�q�@�\�ݒ� (R/W & NVM)
  uint16_t  PWMFrequency;     // PWM���g���ݒ� (R/W & NVM)
  uint16_t  PWMDuty[6];       // PWM�f���[�e�B�[�ݒ� (R/W & NVM)
  uint8_t   reserve2[10];
  uint16_t  Capture[4];       // �p���X�ݒ�E�v���l (R/W)
  uint16_t  GPIO_OUT;         // GPIO�o�͐ݒ�l (R/W)
  uint16_t  GPIO_IN;          // GPIO���͒l (R)
  uint16_t  ADV[12];          // �A�i���O�d������l (R)
  uint16_t  DAV[12];          // �A�i���O�d���ݒ�l (R/W)
  uint8_t   IMU_OprMode;      // IMU���샂�[�h (R/W & NVM)
  uint8_t   IMU_AxisPlace;    // IMU�Z���T�z�u (R/W & NVM)
  Tbno055   IMU;              // IMU����l (R)
  uint8_t   IMU_stat;         // IMU�X�e�[�^�X�l (R)
  uint8_t   IMU_err;          // IMU�G���[�l (R)
  uint8_t   userarea[20];     //
} _PACKED_ TE097BCtrlTable;


//===========================================================
// 1�����擾
//===========================================================
int Getc (void) {
#ifdef _WIN32
  return _getch ();
#else
  return getchar ();
#endif
}

//===========================================================
// ���C��
//===========================================================
int main (void) {
  TDeviceID   dev;
  TErrorCode  err;

  TE097BCtrlTable table;
  uint8_t led;

  if ((dev = DX_OpenPort (_COMPORT, _DXMIO_BAUD))) {
    printf ("Successful opening of %s\n", _COMPORT);
    bool term = false;
    // �����L�[�������ƃ��[�v�𔲂���
    for (;!term;) {
      // �S�R���g���[���e�[�u�����ꊇ�ǂݏo��
      if (DX_ReadBlockData (dev, _DXMIO_ID, 0, (uint8_t *)&table, sizeof (TE097BCtrlTable), &err)) {
        printf (
          "IMU[%8.3f %8.3f %8.3f] GPIO[%03x] ADC[%03x %03x %03x %03x %03x %03x %03x %03x %03x %03x %03x %03x]\r",
          table.IMU.heading / 16.0, table.IMU.roll / 16.0, table.IMU.pitch / 16.0,
          table.GPIO_IN,
          table.ADV[0], table.ADV[1], table.ADV[2], table.ADV[3], table.ADV[4], table.ADV[5], table.ADV[6], table.ADV[7], table.ADV[8], table.ADV[9], table.ADV[10], table.ADV[11]
        );
      }

      if (kbhit ()) {
        // �L�[���͂ɉ����Đݒ��ύX
        switch (Getc ()) {
          case '1': // LED�̖���
            if (DX_ReadByteData (dev, _DXMIO_ID, 10, &led, &err)) {
              led ^= 1;
              DX_WriteByteData (dev, _DXMIO_ID, 10, led, &err);
            }
            break;
          case '2': // �f�W�^������
            for (int i = 0; i < 12; i++) DX_WriteByteData (dev, _DXMIO_ID, 12 + i, 0, &err);
            break;
          case '3': // �A�i���O����
            for (int i = 0; i < 12; i++) DX_WriteByteData (dev, _DXMIO_ID, 12 + i, 5, &err);
            break;
          default: // �I��
            term = true;
            break;
        }
      }
    }
    // �|�[�g�N���[�Y
    DX_ClosePort (dev);
  } else printf ("Failed to open %s\n", _COMPORT);

#ifndef _WIN32
  disable_raw_mode ();
#endif

  printf ("\nFin");
}
