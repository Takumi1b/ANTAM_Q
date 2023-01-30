/*----------------------------------------------------------*
   dxlib_intuitive.cpp
   アドレスやモデルの差異を吸収するサブルーチン集 V4.6
                                       Last Edit '22 03/01
   Copyright (c) 2005, 2022 BestTechnology CO.,LTD.
 *----------------------------------------------------------*/
/*
   モデル毎に異なるコントロールテーブルのアイテムの扱いを吸収し、
   物理値で指令やモニタを行う。
   モデルによって存在しない機能は別として、できるだけモデルやプ
   ロトコルの違いを意識せずにDynamixelを扱う事ができる。

   なお、基本的に通信環境が最良である前提としたため、エラーが生
   じた際のリカバリは行わない。
 */

#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <math.h>
#include  "./dxlib.h"
#include  "./dxmemmap.h"

#ifndef _WIN32
#include <unistd.h>
#define Sleep(w)  usleep(w*1000)
#endif

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

static const TDXL_ModelInfo ModelInfoList[] = {
  { 0,      "NONE",        devtNONE },  // 不定な時
  { 0xffff, "UNKNOWN",     devtNONE },  // リストに無い時

  { 0x0071, "DX-113",      devtDX, { 1023,    0 }, {  150.0,  -150.0 }, { 1023, -1023 }, {    0,    0 }, 0.111 * 6.0,               0,             0 },
  { 0x0074, "DX-116",      devtDX, { 1023,    0 }, {  150.0,  -150.0 }, { 1023, -1023 }, {    0,    0 }, 0.111 * 6.0,               0,             0 },
  { 0x0075, "DX-117",      devtDX, { 1023,    0 }, {  150.0,  -150.0 }, { 1023, -1023 }, {    0,    0 }, 0.111 * 6.0,               0,             0 },

  { 0x002C, "AX-12W",      devtAX, { 1023,    0 }, {  150.0,  -150.0 }, { 1023, -1023 }, {    0,    0 }, 0.111 * 6.0,  800.0 / 1023.0,             0 },
  { 0x000C, "AX-12",       devtAX, { 1023,    0 }, {  150.0,  -150.0 }, { 1023, -1023 }, {    0,    0 }, 0.111 * 6.0, 1500.0 / 1023.0,             0 },
  { 0x0012, "AX-18",       devtAX, { 1023,    0 }, {  150.0,  -150.0 }, { 1023, -1023 }, {    0,    0 }, 0.111 * 6.0, 2200.0 / 1023.0,             0 },

  { 0x000A, "RX-10",       devtRX, { 1023,    0 }, {  150.0,  -150.0 }, { 1023, -1023 }, {    0,    0 }, 0.111 * 6.0,               0,             0 },
  { 0x0018, "RX-24F",      devtRX, { 1023,    0 }, {  150.0,  -150.0 }, { 1023, -1023 }, {    0,    0 }, 0.111 * 6.0, 2400.0 / 1023.0,             0 },
  { 0x001C, "RX-28",       devtRX, { 1023,    0 }, {  150.0,  -150.0 }, { 1023, -1023 }, {    0,    0 }, 0.111 * 6.0, 1200.0 / 1023.0,             0 },
  { 0x0040, "RX-64",       devtRX, { 1023,    0 }, {  150.0,  -150.0 }, { 1023, -1023 }, {    0,    0 }, 0.111 * 6.0, 1700.0 / 1023.0,             0 },
  { 0x006B, "EX-106+",     devtEX, { 4095,    0 }, { 125.46, -125.46 }, { 1023, -1023 }, {    0,    0 }, 0.111 * 6.0,              10,             0 },

  { 0x0068, "MX-12W",      devtMX, { 4095,    0 }, {  180.0,  -180.0 }, { 1023, -1023 }, {    0,    0 }, 0.916 * 6.0,               0,             0 },
  { 0x001D, "MX-28",       devtMX, { 4095,    0 }, {  180.0,  -180.0 }, { 1023, -1023 }, {    0,    0 }, 0.114 * 6.0, 1400.0 / 1023.0,             0 },
  { 0x0136, "MX-64",       devtMX, { 4095,    0 }, {  180.0,  -180.0 }, { 1023, -1023 }, {    0,    0 }, 0.114 * 6.0, 9211.5 / 2048.0,             0 },
  { 0x0140, "MX-106",      devtMX, { 4095,    0 }, {  180.0,  -180.0 }, { 1023, -1023 }, {    0,    0 }, 0.114 * 6.0, 9211.5 / 2048.0,             0 },

  { 0x001E, "MX-28(2.0)",  devtX,  { 4095,    0 }, {  180.0,  -180.0 }, {  380,  -380 }, {  885, -885 }, 0.229 * 6.0, 1400.0 / 1000.0, 100.0 / 885.0 },
  { 0x0137, "MX-64(2.0)",  devtX,  { 4095,    0 }, {  180.0,  -180.0 }, {  435,  -435 }, {  885, -885 }, 0.229 * 6.0,            2.69, 100.0 / 885.0 },
  { 0x0141, "MX-106(2.0)", devtX,  { 4095,    0 }, {  180.0,  -180.0 }, {  360,  -360 }, {  885, -885 }, 0.229 * 6.0,            2.69, 100.0 / 885.0 },

  { 0x0424, "XL430-W250",  devtX,  { 4095,    0 }, {  180.0,  -180.0 }, {  265,  -265 }, {  885, -885 }, 0.229 * 6.0, 1400.0 / 1000.0, 100.0 / 885.0 },
  { 0x0442, "2XL430-W250", devtX,  { 4095,    0 }, {  180.0,  -180.0 }, {  250,  -250 }, {  885, -885 }, 0.229 * 6.0, 1400.0 / 1000.0, 100.0 / 885.0 },
  { 0x0488, "2XC430-W250", devtX,  { 4095,    0 }, {  180.0,  -180.0 }, {  275,  -275 }, {  885, -885 }, 0.229 * 6.0, 1400.0 / 1000.0, 100.0 / 885.0 },
  { 0x042E, "XC430-W150",  devtX,  { 4095,    0 }, {  180.0,  -180.0 }, {  460,  -460 }, {  885, -885 }, 0.229 * 6.0, 1400.0 / 1000.0, 100.0 / 885.0 },
  { 0x0438, "XC430-W240",  devtX,  { 4095,    0 }, {  180.0,  -180.0 }, {  306,  -306 }, {  885, -885 }, 0.229 * 6.0, 1400.0 / 1000.0, 100.0 / 885.0 },
  { 0x0406, "XM430-W210",  devtX,  { 4095,    0 }, {  180.0,  -180.0 }, {  330,  -330 }, {  885, -885 }, 0.229 * 6.0,            2.69, 100.0 / 885.0 },
  { 0x03F2, "XH430-W210",  devtX,  { 4095,    0 }, {  180.0,  -180.0 }, {  210,  -210 }, {  885, -885 }, 0.229 * 6.0,            2.69, 100.0 / 885.0 },
  { 0x03F3, "XD430-T210",  devtX,  { 4095,    0 }, {  180.0,  -180.0 }, {  210,  -210 }, {  885, -885 }, 0.229 * 6.0,            2.69, 100.0 / 885.0 },
  { 0x041A, "XH430-V210",  devtX,  { 4095,    0 }, {  180.0,  -180.0 }, {  230,  -230 }, {  885, -885 }, 0.229 * 6.0,            1.34, 100.0 / 885.0 },
  { 0x03FC, "XM430-W350",  devtX,  { 4095,    0 }, {  180.0,  -180.0 }, {  200,  -200 }, {  885, -885 }, 0.229 * 6.0,            2.69, 100.0 / 885.0 },
  { 0x03E8, "XH430-W350",  devtX,  { 4095,    0 }, {  180.0,  -180.0 }, {  130,  -130 }, {  885, -885 }, 0.229 * 6.0,            2.69, 100.0 / 885.0 },
  { 0x03E9, "XD430-T350",  devtX,  { 4095,    0 }, {  180.0,  -180.0 }, {  130,  -130 }, {  885, -885 }, 0.229 * 6.0,            2.69, 100.0 / 885.0 },
  { 0x0410, "XH430-V350",  devtX,  { 4095,    0 }, {  180.0,  -180.0 }, {  135,  -135 }, {  885, -885 }, 0.229 * 6.0,            1.34, 100.0 / 885.0 },
  { 0x0500, "XW430-T200",  devtX,  { 4095,    0 }, {  180.0,  -180.0 }, {  235,  -235 }, {  885, -885 }, 0.229 * 6.0,    2.69 * 1.000, 100.0 / 885.0 },
  { 0x04F6, "XW430-T333",  devtX,  { 4095,    0 }, {  180.0,  -180.0 }, {  139,  -139 }, {  885, -885 }, 0.229 * 6.0,    2.69 * 1.000, 100.0 / 885.0 },
  { 0x046A, "XM540-W150",  devtX,  { 4095,    0 }, {  180.0,  -180.0 }, {  230,  -230 }, {  885, -885 }, 0.229 * 6.0,            2.69, 100.0 / 885.0 },
  { 0x0456, "XH540-W150",  devtX,  { 4095,    0 }, {  180.0,  -180.0 }, {  300,  -300 }, {  885, -885 }, 0.229 * 6.0,            2.69, 100.0 / 885.0 },
  { 0x0457, "XD540-T150",  devtX,  { 4095,    0 }, {  180.0,  -180.0 }, {  300,  -300 }, {  885, -885 }, 0.229 * 6.0,            2.69, 100.0 / 885.0 },
  { 0x047E, "XH540-V150",  devtX,  { 4095,    0 }, {  180.0,  -180.0 }, {  230,  -230 }, {  885, -885 }, 0.229 * 6.0,            2.69, 100.0 / 885.0 },
  { 0x0460, "XM540-W270",  devtX,  { 4095,    0 }, {  180.0,  -180.0 }, {  128,  -128 }, {  885, -885 }, 0.229 * 6.0,            2.69, 100.0 / 885.0 },
  { 0x044C, "XH540-W270",  devtX,  { 4095,    0 }, {  180.0,  -180.0 }, {  167,  -167 }, {  885, -885 }, 0.229 * 6.0,            2.69, 100.0 / 885.0 },
  { 0x044D, "XD540-T270",  devtX,  { 4095,    0 }, {  180.0,  -180.0 }, {  167,  -167 }, {  885, -885 }, 0.229 * 6.0,            2.69, 100.0 / 885.0 },
  { 0x0474, "XH540-V270",  devtX,  { 4095,    0 }, {  180.0,  -180.0 }, {  128,  -128 }, {  885, -885 }, 0.229 * 6.0,            2.69, 100.0 / 885.0 },
  { 0x049C, "XW540-T140",  devtX,  { 4095,    0 }, {  180.0,  -180.0 }, {  304,  -304 }, {  885, -885 }, 0.229 * 6.0,            2.69, 100.0 / 885.0 },
  { 0x0492, "XW540-T260",  devtX,  { 4095,    0 }, {  180.0,  -180.0 }, {  167,  -167 }, {  885, -885 }, 0.229 * 6.0,            2.69, 100.0 / 885.0 },
};

// 全デバイスの情報
static struct TDevices {
  uint8_t     num;
  uint8_t     id2model[256];
  uint8_t     drivemode[256];
  uint8_t     opmode[256];
  TErrorCode  err[256];
} Devices;

// 全デバイス情報初期化
static void initarray (void) {
  static bool init = false;
  if (!init) {
    DXL_InitDevicesList();
    init = true;
  }
}

// 角度→位置
static int32_t ang2pos (double angle, const TDXL_ModelInfo *m) {
  return (angle - m->anglelimit.min) * (m->positionlimit.max - m->positionlimit.min) / (m->anglelimit.max - m->anglelimit.min) + m->positionlimit.min;
}

// 位置→角度
static double pos2ang (int32_t position, const TDXL_ModelInfo *m) {
  return (position - m->positionlimit.min) * (m->anglelimit.max - m->anglelimit.min) / (m->positionlimit.max - m->positionlimit.min) + m->anglelimit.min;
}

//-------------------------------------------------
// LED明滅
//-------------------------------------------------
DXAPIDLL bool DXL_SetLED (TDeviceID dvid, uint8_t id, bool en) {
  initarray();
  if (id <= 253) {
    int idx = Devices.id2model[id];
    if (idx >= 2) {
      switch (ModelInfoList[idx].devtype) {
        case devtDX:
        case devtAX:
        case devtRX:
        case devtEX:
        case devtMX:
          return DX_WriteByteData (dvid, id, ADDRESS_LED, en ? 1 : 0, &Devices.err[id]);
          break;
        case devtX:
          return DX_WriteByteData (dvid, id, ADDRESS_X_LED_RED, en ? 1 : 0, &Devices.err[id]);
          break;
        default:
          break;
      }
    }
  }
  return false;
}

//-------------------------------------------------
// トルクイネーブル指令
//-------------------------------------------------
DXAPIDLL bool DXL_SetTorqueEnable (TDeviceID dvid, uint8_t id, bool en) {
  initarray();
  if (id <= 253) {
    uint8_t ren;
    int idx = Devices.id2model[id];
    if (idx >= 2) {
      switch (ModelInfoList[idx].devtype) {
        case devtDX:
        case devtAX:
        case devtRX:
        case devtEX:
        case devtMX:
          if (DX_WriteByteData (dvid, id, ADDRESS_TORQUE_ENABLE, en ? 1 : 0, &Devices.err[id])) {
            if (DX_ReadByteData (dvid, id, ADDRESS_TORQUE_ENABLE, &ren, &Devices.err[id])) {
              return (((ren == 1) ? true : false) == en);
            }
          }
          break;
        case devtX:
          if (DX_WriteByteData (dvid, id, ADDRESS_X_TORQUE_ENABLE, en ? 1 : 0, &Devices.err[id])) {
            if (DX_ReadByteData (dvid, id, ADDRESS_X_TORQUE_ENABLE, &ren, &Devices.err[id])) {
              return (((ren == 1) ? true : false) == en);
            }
          }
          break;
        default:
          break;
      }
    }
  }
  return false;
}

DXAPIDLL bool DXL_SetTorqueEnables (TDeviceID dvid, const uint8_t *ids, const bool *ens, int num) {
#ifdef _MSC_VER
  __pragma (pack (push, 1))
  typedef struct {
    uint8_t  id;
    uint8_t  TorqueEnable;
  } Tid_ten;
  struct {
    uint8_t num;
    uint8_t addr;
    uint8_t size;
    Tid_ten data[254];
  } SyncDataMX, SyncDataX;
  __pragma (pack (pop))
#elif defined(__GNUC__)
  typedef struct {
    uint8_t  id;
    uint8_t  TorqueEnable;
  } _PACKED_ Tid_ten;
  struct {
    uint8_t num;
    uint8_t addr;
    uint8_t size;
    Tid_ten data[254];
  } _PACKED_ SyncDataMX, SyncDataX;
#endif
  initarray();
  SyncDataMX.num = SyncDataX.num = 0;
  SyncDataMX.addr = ADDRESS_TORQUE_ENABLE;
  SyncDataMX.size = 1;
  SyncDataX.addr = ADDRESS_X_TORQUE_ENABLE;
  SyncDataX.size = 1;
  if (ens != NULL) {
    for (int i = 0; i < num; i++) {
      uint8_t id = ids[i];
      if (id <= 253) {
        int idx = Devices.id2model[id];
        if (idx >= 2) {
          switch (ModelInfoList[idx].devtype) {
            case devtDX:
            case devtAX:
            case devtRX:
            case devtEX:
            case devtMX:
              SyncDataMX.data[SyncDataMX.num].id = id;
              SyncDataMX.data[SyncDataMX.num].TorqueEnable = ens[i] ? 1 : 0;
              SyncDataMX.num++;
              break;
            case devtX:
              SyncDataX.data[SyncDataX.num].id = id;
              SyncDataX.data[SyncDataX.num].TorqueEnable = ens[i] ? 1 : 0;
              SyncDataX.num++;
              break;
            default:
              break;
          }
        }
      }
    }
    bool result = false;
    if (SyncDataMX.num > 0) result = DX_WriteSyncData (dvid, (uint8_t *)&SyncDataMX.addr, 2 + 3 * SyncDataMX.num, NULL);
    if (SyncDataX.num > 0) result = ((SyncDataMX.num > 0) ? result : true) && DX_WriteSyncData (dvid, (uint8_t *)&SyncDataX.addr, 2 + 5 * SyncDataX.num, NULL);
    return result;
  }
  return false;
}

DXAPIDLL bool DXL_SetTorqueEnablesEquival (TDeviceID dvid, const uint8_t *ids, int num, bool en) {
  bool ten[num];
  initarray();
  for (int i = 0; i < num; i++) ten[i] = en;
  return DXL_SetTorqueEnables (dvid, ids, ten, num);
}

//-------------------------------------------------
// トルクイネーブル取得
//-------------------------------------------------
DXAPIDLL bool DXL_GetTorqueEnable (TDeviceID dvid, uint8_t id, bool *en) {
  initarray();
  if (en != NULL) {
    if (id <= 253) {
      int idx = Devices.id2model[id];
      if (idx >= 2) {
        uint8_t ren;
        switch (ModelInfoList[idx].devtype) {
          case devtDX:
          case devtAX:
          case devtRX:
          case devtEX:
          case devtMX:
            if (DX_ReadByteData (dvid, id, 24, &ren, &Devices.err[id])) {
              *en = (ren == 1);
              return true;
            }
            break;
          case devtX:
            if (DX_ReadByteData (dvid, id, ADDRESS_X_TORQUE_ENABLE, &ren, &Devices.err[id])) {
              *en = (ren == 1);
              return true;
            }
            break;
          default:
            break;
        }
      }
    }
  }
  return false;
}

DXAPIDLL bool DXL_GetTorqueEnables (TDeviceID dvid, const uint8_t *ids, bool *en, int num) {
  bool result = true;
  initarray();
  if (ids != NULL && en != NULL) {
    for (int i = 0; i < num; i++) {
      int idx = Devices.id2model[ids[i]];
      if (idx >= 2) {
        if (!DXL_GetTorqueEnable (dvid, ids[i], &en[i])) result = false;
      }
    }
  }
  return result;
}

//-------------------------------------------------
// 角度指令
//-------------------------------------------------
DXAPIDLL bool DXL_SetGoalAngle (TDeviceID dvid, uint8_t id, double angle) {
  initarray();
  if (id <= 253) {
    int idx = Devices.id2model[id];
    if (idx >= 2) {
      int32_t pmax, pmin;
      if (Devices.opmode[id] == 3) {
        pmax = ModelInfoList[idx].positionlimit.max;
        pmin = ModelInfoList[idx].positionlimit.min;
      } else {
        pmax = INT32_MAX;
        pmin = INT32_MIN;
      }
      int32_t pos = max (min (ang2pos (angle, &ModelInfoList[idx]), pmax), pmin);
      switch (ModelInfoList[idx].devtype) {
        case devtDX:
        case devtAX:
        case devtRX:
        case devtEX:
        case devtMX:
          return DX_WriteWordData (dvid, id, ADDRESS_GOAL_POSITION, pos, &Devices.err[id]);
          break;
        case devtX:
          return DX_WriteLongData (dvid, id, ADDRESS_X_GOAL_POSITION, pos, &Devices.err[id]);
          break;
        default:
          break;
      }
    }
  }
  return false;
}

DXAPIDLL bool DXL_SetGoalAngles (TDeviceID dvid, const uint8_t *ids, const double *angles, int num) {
#ifdef _MSC_VER
  __pragma (pack (push, 1))
  typedef struct {
    uint8_t   id;
    uint16_t  GoalPosition;
  } Tid_pos_W;
  struct {
    uint8_t   num;
    uint8_t   addr;
    uint8_t   size;
    Tid_pos_W data[254];
  } SyncData_W;
  typedef struct {
    uint8_t   id;
    uint32_t  GoalPosition;
  } Tid_pos_L;
  struct {
    uint8_t   num;
    uint8_t   addr;
    uint8_t   size;
    Tid_pos_L data[254];
  } SyncData_L;
  __pragma (pack (pop))
#elif defined(__GNUC__)
  typedef struct {
    uint8_t   id;
    uint16_t  GoalPosition;
  } _PACKED_ Tid_pos_W;
  struct {
    uint8_t   num;
    uint8_t   addr;
    uint8_t   size;
    Tid_pos_W data[254];
  } _PACKED_ SyncData_W;
  typedef struct {
    uint8_t   id;
    uint32_t  GoalPosition;
  } _PACKED_ Tid_pos_L;
  struct {
    uint8_t   num;
    uint8_t   addr;
    uint8_t   size;
    Tid_pos_L data[254];
  } _PACKED_ SyncData_L;
#endif
  initarray();
  SyncData_W.num = SyncData_L.num = 0;
  SyncData_W.addr = ADDRESS_GOAL_POSITION;
  SyncData_W.size = 2;
  SyncData_L.addr = ADDRESS_X_GOAL_POSITION;
  SyncData_L.size = 4;
  if (ids != NULL && angles != NULL) {
    for (int i = 0; i < num; i++) {
      uint8_t id = ids[i];
      if (id <= 253) {
        int idx = Devices.id2model[id];
        if (idx >= 2) {
          int32_t pmax, pmin;
          if (Devices.opmode[id] == 3) {
            pmax = ModelInfoList[idx].positionlimit.max;
            pmin = ModelInfoList[idx].positionlimit.min;
          } else {
            pmax = INT32_MAX;
            pmin = INT32_MIN;
          }
          int32_t pos = max (min (ang2pos (angles[i], &ModelInfoList[idx]), pmax), pmin);
          switch (ModelInfoList[idx].devtype) {
            case devtDX:
            case devtAX:
            case devtRX:
            case devtEX:
            case devtMX:
              SyncData_W.data[SyncData_W.num].id = id;
              SyncData_W.data[SyncData_W.num].GoalPosition = pos;
              SyncData_W.num++;
              break;
            case devtX:
              SyncData_L.data[SyncData_L.num].id = id;
              SyncData_L.data[SyncData_L.num].GoalPosition = pos;
              SyncData_L.num++;
              break;
            default:
              break;
          }
        }
      }
    }
    bool result = false;
    if (SyncData_W.num > 0) result = DX_WriteSyncData (dvid, (uint8_t *)&SyncData_W.addr, 2 + 3 * SyncData_W.num, NULL);
    if (SyncData_L.num > 0) result = ((SyncData_W.num > 0) ? result : true) && DX_WriteSyncData (dvid, (uint8_t *)&SyncData_L.addr, 2 + 5 * SyncData_L.num, NULL);
    return result;
  }
  return false;
}

//-------------------------------------------------
// 角度取得
//-------------------------------------------------
DXAPIDLL bool DXL_GetPresentAngle (TDeviceID dvid, uint8_t id, double *angle) {
  initarray();
  if (angle != NULL) {
    if (id <= 253) {
      int idx = Devices.id2model[id];
      if (idx >= 2) {
        switch (ModelInfoList[idx].devtype) {
          case devtDX:
          case devtAX:
          case devtRX:
          case devtEX:
          case devtMX: {
              int16_t pos;
              if (DX_ReadWordData (dvid, id, ADDRESS_PRESENT_POSITION, (uint16_t *)&pos, &Devices.err[id])) {
                *angle = pos2ang (pos, &ModelInfoList[idx]);
                return true;
              }
            }
            break;
          case devtX: {
              int32_t pos;
              if (DX_ReadLongData (dvid, id, ADDRESS_X_PRESENT_POSITION, (uint32_t *)&pos, &Devices.err[id])) {
                *angle = pos2ang (pos, &ModelInfoList[idx]);
                return true;
              }
            }
            break;
          default:
            break;
        }
      }
    }
  }
  return false;
}

DXAPIDLL bool DXL_GetPresentAngles (TDeviceID dvid, const uint8_t *ids, double *angles, int num) {
  bool result = true;
  initarray();
  if (ids != NULL && angles != NULL) {
    for (int i = 0; i < num; i++) {
      int idx = Devices.id2model[ids[i]];
      if (idx >= 2) {
        if (!DXL_GetPresentAngle (dvid, ids[i], &angles[i])) result = false;
      }
    }
  }
  return result;
}

//-------------------------------------------------
// 現在位置で即時停止
//-------------------------------------------------
DXAPIDLL bool DXL_StandStillAngle (TDeviceID dvid, uint8_t id) {
  double pangle;
  initarray();
  if (DXL_GetPresentAngle (dvid, id, &pangle)) return DXL_SetGoalAngleAndVelocity (dvid, id, pangle, 0);
  return false;
}

DXAPIDLL bool DXL_StandStillAngles (TDeviceID dvid, const uint8_t *ids, int num) {
  double pangles[num];
  initarray();
#if 0
  if (DXL_GetPresentAngles (dvid, ids, pangles, num)) return DXL_SetGoalAngles (dvid, ids, pangles, num);
#else
  TAngleVelocity angvelo[num];
  memset (&angvelo, 0, sizeof (angvelo));
  if (DXL_GetPresentAngles (dvid, ids, pangles, num)) {
    for (int i = 0; i < num; i++) angvelo[i].angle = pangles[i];
    return DXL_SetGoalAnglesAndVelocities (dvid, ids, angvelo, num);
  }
#endif
  return false;
}

//-------------------------------------------------
// 角速度指令
//-------------------------------------------------
DXAPIDLL bool DXL_SetGoalVelocity (TDeviceID dvid, uint8_t id, double velocity) {
  initarray();
  if (id <= 253) {
    int idx = Devices.id2model[id];
    if (idx >= 2) {
      int32_t velo = max (min (velocity / ModelInfoList[idx].velocityratio, ModelInfoList[idx].velocitylimit.max), ModelInfoList[idx].velocitylimit.min);
      switch (ModelInfoList[idx].devtype) {
        case devtDX:
        case devtAX:
        case devtRX:
        case devtEX:
        case devtMX:
          if (velo < 0) velo = abs (velo) | 0x400;
          return DX_WriteWordData (dvid, id, ADDRESS_GOAL_SPEED, velo, &Devices.err[id]);
          break;
        case devtX:
          return DX_WriteLongData (dvid, id, ADDRESS_X_GOAL_VELOCITY, velo, &Devices.err[id]);
          break;
        default:
          break;
      }
    }
  }
  return false;
}

DXAPIDLL bool DXL_SetGoalVelocities (TDeviceID dvid, const uint8_t *ids, const double *velocities, int num) {
#ifdef _MSC_VER
  __pragma (pack (push, 1))
  typedef struct {
    uint8_t  id;
    uint16_t MovingSpeed;
  } Tid_velo_W;
  struct {
    uint8_t    num;
    uint8_t    addr;
    uint8_t    size;
    Tid_velo_W data[254];
  } SyncData_W;
  typedef struct {
    uint8_t  id;
    int32_t  GoalVelocity;
  } Tid_velo_L;
  struct {
    uint8_t    num;
    uint8_t    addr;
    uint8_t    size;
    Tid_velo_L data[254];
  } SyncData_L;
  __pragma (pack (pop))
#elif defined(__GNUC__)
  typedef struct {
    uint8_t  id;
    uint16_t MovingSpeed;
  } _PACKED_ Tid_velo_W;
  struct {
    uint8_t    num;
    uint8_t    addr;
    uint8_t    size;
    Tid_velo_W data[254];
  } _PACKED_ SyncData_W;
  typedef struct {
    uint8_t  id;
    int32_t  GoalVelocity;
  } _PACKED_ Tid_velo_L;
  struct {
    uint8_t    num;
    uint8_t    addr;
    uint8_t    size;
    Tid_velo_L data[254];
  } _PACKED_ SyncData_L;
#endif
  initarray();
  SyncData_W.num = SyncData_L.num = 0;
  SyncData_W.addr = ADDRESS_GOAL_SPEED;
  SyncData_W.size = 2;
  SyncData_L.addr = ADDRESS_X_GOAL_VELOCITY;
  SyncData_L.size = 4;
  if (ids != NULL && velocities != NULL) {
    for (int i = 0; i < num; i++) {
      uint8_t id = ids[i];
      if (id <= 253) {
        int idx = Devices.id2model[id];
        if (idx >= 2) {
          int32_t velo = max (min (velocities[i] / ModelInfoList[idx].velocityratio, ModelInfoList[idx].velocitylimit.max), ModelInfoList[idx].velocitylimit.min);
          switch (ModelInfoList[idx].devtype) {
            case devtDX:
            case devtAX:
            case devtRX:
            case devtEX:
            case devtMX:
              SyncData_W.data[SyncData_W.num].id = id;
              if (velo < 0) velo = abs (velo) | 0x400;
              SyncData_W.data[SyncData_W.num].MovingSpeed = velo;
              SyncData_W.num++;
              break;
            case devtX:
              SyncData_L.data[SyncData_L.num].id = id;
              SyncData_L.data[SyncData_L.num].GoalVelocity = velo;
              SyncData_L.num++;
              break;
            default:
              break;
          }
        }
      }
    }
    bool result = false;
    if (SyncData_W.num > 0) result = DX_WriteSyncData (dvid, (uint8_t *)&SyncData_W.addr, 2 + 3 * SyncData_W.num, NULL);
    if (SyncData_L.num > 0) result = ((SyncData_W.num > 0) ? result : true) && DX_WriteSyncData (dvid, (uint8_t *)&SyncData_L.addr, 2 + 5 * SyncData_L.num, NULL);
    return result;
  }
  return false;
}

//-------------------------------------------------
// 角速度取得
//-------------------------------------------------
DXAPIDLL bool DXL_GetPresentVelocity (TDeviceID dvid, uint8_t id, double *velocity) {
  initarray();
  if (velocity != NULL) {
    if (id <= 253) {
      int idx = Devices.id2model[id];
      if (idx >= 2) {
        switch (ModelInfoList[idx].devtype) {
          case devtDX:
          case devtAX:
          case devtRX:
          case devtEX:
          case devtMX: {
              int16_t velo;
              if (DX_ReadWordData (dvid, id, ADDRESS_PRESENT_SPEED, (uint16_t *)&velo, &Devices.err[id])) {
                if (velo & 0x400) velo = - (velo & 0x3ff);
                *velocity = (double)velo * ModelInfoList[idx].velocityratio;
                return true;
              }
            }
            break;
          case devtX: {
              int32_t velo;
              if (DX_ReadLongData (dvid, id, ADDRESS_X_PRESENT_VELOCITY, (uint32_t *)&velo, &Devices.err[id])) {
                *velocity = (double)velo * ModelInfoList[idx].velocityratio;
                return true;
              }
            }
            break;
          default:
            break;
        }
      }
    }
  }
  return false;
}

DXAPIDLL bool DXL_GetPresentVelocities (TDeviceID dvid, const uint8_t *ids, double *velocities, int num) {
  initarray();
  bool result = true;
  if (ids != NULL && velocities != NULL) {
    for (int i = 0; i < num; i++) {
      int idx = Devices.id2model[ids[i]];
      if (idx >= 2) {
        if (!DXL_GetPresentVelocity (dvid, ids[i], &velocities[i])) result = false;
      }
    }
  }
  return result;
}

//-------------------------------------------------
// 角度・角速度指令
//-------------------------------------------------
DXAPIDLL bool DXL_SetGoalAngleAndVelocity (TDeviceID dvid, uint8_t id, double angle, double velocity) {
  initarray();
  if (id <= 253) {
    int idx = Devices.id2model[id];
    if (idx >= 2) {
      int32_t pmax, pmin;
      if (Devices.opmode[id] == 3) {
        pmax = ModelInfoList[idx].positionlimit.max;
        pmin = ModelInfoList[idx].positionlimit.min;
      } else {
        pmax = INT32_MAX;
        pmin = INT32_MIN;
      }
      int32_t pos = max (min (ang2pos (angle, &ModelInfoList[idx]), pmax), pmin);
      int32_t velo = abs (max (min (velocity / ModelInfoList[idx].velocityratio, ModelInfoList[idx].velocitylimit.max), ModelInfoList[idx].velocitylimit.min));
      switch (ModelInfoList[idx].devtype) {
        case devtDX:
        case devtAX:
        case devtRX:
        case devtEX:
        case devtMX: {
#ifdef _MSC_VER
            __pragma (pack (push, 1))
            struct {
              int16_t GoalPosition;
              int16_t MovingSpeed;
            } block;
            __pragma (pack (pop))
#elif defined(__GNUC__)
            struct {
              int16_t GoalPosition;
              int16_t MovingSpeed;
            } _PACKED_ block;
#endif
            block.GoalPosition = pos;
            block.MovingSpeed = velo;
            return DX_WriteBlockData (dvid, id, ADDRESS_GOAL_POSITION, (uint8_t *)&block, 4, &Devices.err[id]);
          }
          break;
        case devtX: {
#ifdef _MSC_VER
            __pragma (pack (push, 1))
            struct {
              int32_t ProfileVelocity;
              int32_t GoalPosition;
            } block;
            __pragma (pack (pop))
#elif defined(__GNUC__)
            struct {
              int32_t ProfileVelocity;
              int32_t GoalPosition;
            } _PACKED_ block;
#endif
            block.ProfileVelocity = velo;
            block.GoalPosition = pos;
            return DX_WriteBlockData (dvid, id, ADDRESS_X_PROF_VELOCITY, (uint8_t *)&block, 8, &Devices.err[id]);
          }
          break;
        default:
          break;
      }
    }
  }
  return false;
}

DXAPIDLL bool DXL_SetGoalAnglesAndVelocities (TDeviceID dvid, const uint8_t *ids, PAngleVelocity anglevelocity, int num) {
#ifdef _MSC_VER
  __pragma (pack (push, 1))
  typedef struct {
    uint8_t  id;
    uint16_t GoalPosition;
    uint16_t MovingSpeed;
  } Tid_pos_velo_W;
  struct {
    uint8_t         num;
    uint8_t         addr;
    uint8_t         size;
    Tid_pos_velo_W  data[254];
  } SyncData_W;
  typedef struct {
    uint8_t  id;
    uint32_t ProfileVelocity;
    uint32_t GoalPosition;
  } Tid_velo_pos_L;
  struct {
    uint8_t         num;
    uint8_t         addr;
    uint8_t         size;
    Tid_velo_pos_L  data[254];
  } SyncData_L;
  __pragma (pack (pop))
#elif defined(__GNUC__)
  typedef struct {
    uint8_t  id;
    uint16_t GoalPosition;
    uint16_t MovingSpeed;
  } _PACKED_ Tid_pos_velo_W;
  struct {
    uint8_t         num;
    uint8_t         addr;
    uint8_t         size;
    Tid_pos_velo_W  data[254];
  } _PACKED_ SyncData_W;
  typedef struct {
    uint8_t  id;
    uint32_t ProfileVelocity;
    uint32_t GoalPosition;
  } _PACKED_ Tid_velo_pos_L;
  struct {
    uint8_t         num;
    uint8_t         addr;
    uint8_t         size;
    Tid_velo_pos_L  data[254];
  } _PACKED_ SyncData_L;
#endif
  initarray();
  SyncData_W.num = SyncData_L.num = 0;
  SyncData_W.addr = ADDRESS_GOAL_POSITION;
  SyncData_W.size = 4;
  SyncData_L.addr = ADDRESS_X_PROF_VELOCITY;
  SyncData_L.size = 8;
  if (ids != NULL && anglevelocity != NULL) {
    for (int i = 0; i < num; i++) {
      uint8_t id = ids[i];
      if (id <= 253) {
        int idx = Devices.id2model[id];
        if (idx >= 2) {
          int32_t pmax, pmin;
          if (Devices.opmode[id] == 3) {
            pmax = ModelInfoList[idx].positionlimit.max;
            pmin = ModelInfoList[idx].positionlimit.min;
          } else {
            pmax = INT32_MAX;
            pmin = INT32_MIN;
          }
          int32_t pos = max (min (ang2pos (anglevelocity[i].angle, &ModelInfoList[idx]), pmax), pmin);
          int32_t velo = abs (max (min (anglevelocity[i].velocity / ModelInfoList[idx].velocityratio, ModelInfoList[idx].velocitylimit.max), ModelInfoList[idx].velocitylimit.min));
          switch (ModelInfoList[idx].devtype) {
            case devtDX:
            case devtAX:
            case devtRX:
            case devtEX:
            case devtMX:
              SyncData_W.data[SyncData_W.num].id = id;
              SyncData_W.data[SyncData_W.num].MovingSpeed = velo;
              SyncData_W.data[SyncData_W.num].GoalPosition = pos;
              SyncData_W.num++;
              break;
            case devtX:
              SyncData_L.data[SyncData_L.num].id = id;
              SyncData_L.data[SyncData_L.num].GoalPosition = pos;
              SyncData_L.data[SyncData_L.num].ProfileVelocity = velo;
              SyncData_L.num++;
              break;
            default:
              break;
          }
        }
      }
    }
    bool result = false;
    if (SyncData_W.num > 0) result = DX_WriteSyncData (dvid, (uint8_t *)&SyncData_W.addr, 2 + 5 * SyncData_W.num, NULL);
    if (SyncData_L.num > 0) result = ((SyncData_W.num > 0) ? result : true) && DX_WriteSyncData (dvid, (uint8_t *)&SyncData_L.addr, 2 + 9 * SyncData_L.num, NULL);
    return result;
  }
  return false;
}

//-------------------------------------------------
// 角度・時間指令(実際には速度指令)
//-------------------------------------------------
DXAPIDLL bool DXL_SetGoalAngleAndTime (TDeviceID dvid, uint8_t id, double angle, double sec) {
  initarray();
  if (id <= 253) {
    int idx = Devices.id2model[id];
    if (idx >= 2) {
      int32_t pmax, pmin;
      if (Devices.opmode[id] == 3) {
        pmax = ModelInfoList[idx].positionlimit.max;
        pmin = ModelInfoList[idx].positionlimit.min;
      } else {
        pmax = INT32_MAX;
        pmin = INT32_MIN;
      }
      double pangle;
      if (DXL_GetPresentAngle (dvid, id, &pangle) && ((Devices.drivemode[id] & 0x4) == 0)) {
        double velocity = fabs (((pangle - angle) / 360.0) / (sec / 60.0));
        int32_t pos = max (min (ang2pos (angle, &ModelInfoList[idx]), pmax), pmin);
        int32_t velo = abs (max (min (velocity / ModelInfoList[idx].velocityratio, ModelInfoList[idx].velocitylimit.max), ModelInfoList[idx].velocitylimit.min));
        switch (ModelInfoList[idx].devtype) {
          case devtDX:
          case devtAX:
          case devtRX:
          case devtEX:
          case devtMX: {
#ifdef _MSC_VER
              __pragma (pack (push, 1))
              struct {
                int16_t GoalPosition;
                int16_t MovingSpeed;
              } block;
              __pragma (pack (pop))
#elif defined(__GNUC__)
              struct {
                int16_t GoalPosition;
                int16_t MovingSpeed;
              } _PACKED_ block;
#endif
              block.GoalPosition = pos;
              block.MovingSpeed = velo;
              return DX_WriteBlockData (dvid, id, ADDRESS_GOAL_POSITION, (uint8_t *)&block, 4, &Devices.err[id]);
            }
            break;
          case devtX: {
#ifdef _MSC_VER
              __pragma (pack (push, 1))
              struct {
                int32_t ProfileVelocity;
                int32_t GoalPosition;
              } block;
              __pragma (pack (pop))
#elif defined(__GNUC__)
              struct {
                int32_t ProfileVelocity;
                int32_t GoalPosition;
              } _PACKED_ block;
#endif
              block.ProfileVelocity = velo;
              block.GoalPosition = pos;
              if ((Devices.drivemode[id] & 0x4) == 0)
                return DX_WriteBlockData (dvid, id, ADDRESS_X_PROF_VELOCITY, (uint8_t *)&block, 8, &Devices.err[id]);
            }
            break;
          default:
            break;
        }
      }
    }
  }
  return false;
}

DXAPIDLL bool DXL_SetGoalAnglesAndTime (TDeviceID dvid, const uint8_t *ids, const double *angles, int num, double sec) {
#ifdef _MSC_VER
  __pragma (pack (push, 1))
  typedef struct {
    uint8_t  id;
    uint16_t GoalPosition;
    uint16_t MovingSpeed;
  } Tid_pos_velo_W;
  struct {
    uint8_t         num;
    uint8_t         addr;
    uint8_t         size;
    Tid_pos_velo_W  data[254];
  } SyncData_W;
  typedef struct {
    uint8_t  id;
    uint32_t ProfileVelocity;
    uint32_t GoalPosition;
  } Tid_velo_pos_L;
  struct {
    uint8_t         num;
    uint8_t         addr;
    uint8_t         size;
    Tid_velo_pos_L  data[254];
  } SyncData_L;
  __pragma (pack (pop))
#elif defined(__GNUC__)
  typedef struct {
    uint8_t  id;
    uint16_t GoalPosition;
    uint16_t MovingSpeed;
  } _PACKED_ Tid_pos_velo_W;
  struct {
    uint8_t         num;
    uint8_t         addr;
    uint8_t         size;
    Tid_pos_velo_W  data[254];
  } _PACKED_ SyncData_W;
  typedef struct {
    uint8_t  id;
    uint32_t ProfileVelocity;
    uint32_t GoalPosition;
  } _PACKED_ Tid_velo_pos_L;
  struct {
    uint8_t         num;
    uint8_t         addr;
    uint8_t         size;
    Tid_velo_pos_L  data[254];
  } _PACKED_ SyncData_L;
#endif
  initarray();
  SyncData_W.num = SyncData_L.num = 0;
  SyncData_W.addr = ADDRESS_GOAL_POSITION;
  SyncData_W.size = 4;
  SyncData_L.addr = ADDRESS_X_PROF_VELOCITY;
  SyncData_L.size = 8;
  if (ids != NULL && angles != NULL) {
    for (int i = 0; i < num; i++) {
      uint8_t id = ids[i];
      if (id <= 253) {
        int idx = Devices.id2model[id];
        if (idx >= 2) {
          int32_t pmax, pmin;
          if (Devices.opmode[id] == 3) {
            pmax = ModelInfoList[idx].positionlimit.max;
            pmin = ModelInfoList[idx].positionlimit.min;
          } else {
            pmax = INT32_MAX;
            pmin = INT32_MIN;
          }
          double pangle;
          if (DXL_GetPresentAngle (dvid, id, &pangle) && ((Devices.drivemode[id] & 0x4) == 0)) {
            double velocity = fabs ((pangle - angles[i]) / sec);
            int32_t pos = max (min (ang2pos (angles[i], &ModelInfoList[idx]), pmax), pmin);
            int32_t velo = abs (max (min (velocity / ModelInfoList[idx].velocityratio, ModelInfoList[idx].velocitylimit.max), ModelInfoList[idx].velocitylimit.min));
            switch (ModelInfoList[idx].devtype) {
              case devtDX:
              case devtAX:
              case devtRX:
              case devtEX:
              case devtMX:
                SyncData_W.data[SyncData_W.num].id = id;
                SyncData_W.data[SyncData_W.num].MovingSpeed = velo;
                SyncData_W.data[SyncData_W.num].GoalPosition = pos;
                SyncData_W.num++;
                break;
              case devtX:
                SyncData_L.data[SyncData_L.num].id = id;
                SyncData_L.data[SyncData_L.num].GoalPosition = pos;
                SyncData_L.data[SyncData_L.num].ProfileVelocity = velo;
                SyncData_L.num++;
                break;
              default:
                break;
            }
          }
        }
      }
    }
    bool result = false;
    if (SyncData_W.num > 0) result = DX_WriteSyncData (dvid, (uint8_t *)&SyncData_W.addr, 2 + 5 * SyncData_W.num, NULL);
    if (SyncData_L.num > 0) result = ((SyncData_W.num > 0) ? result : true) && DX_WriteSyncData (dvid, (uint8_t *)&SyncData_L.addr, 2 + 9 * SyncData_L.num, NULL);
    return result;
  }
  return false;
}

//-------------------------------------------------
// 角度・時間指令2(時間指令)
//-------------------------------------------------
DXAPIDLL bool DXL_SetGoalAngleAndTime2 (TDeviceID dvid, uint8_t id, double angle, double sec) {
  initarray();
  if (id <= 253) {
    int idx = Devices.id2model[id];
    if (idx >= 2) {
      int32_t pmax, pmin;
      if (Devices.opmode[id] == 3) {
        pmax = ModelInfoList[idx].positionlimit.max;
        pmin = ModelInfoList[idx].positionlimit.min;
      } else {
        pmax = INT32_MAX;
        pmin = INT32_MIN;
      }
      if ((Devices.drivemode[id] & 0x4) != 0) {
        int32_t pos = max (min (ang2pos (angle, &ModelInfoList[idx]), pmax), pmin);
        int32_t profileacc = abs (max (min (sec * 1000.0, 32767), 0));
        switch (ModelInfoList[idx].devtype) {
          case devtX: {
#ifdef _MSC_VER
              __pragma (pack (push, 1))
              struct {
                int32_t ProfileVelocity;
                int32_t GoalPosition;
              } block;
              __pragma (pack (pop))
#elif defined(__GNUC__)
              struct {
                int32_t ProfileVelocity;
                int32_t GoalPosition;
              } _PACKED_ block;
#endif
              block.ProfileVelocity = profileacc;
              block.GoalPosition = pos;
              if ((Devices.drivemode[id] & 0x4) == 0)
                return DX_WriteBlockData (dvid, id, ADDRESS_X_PROF_VELOCITY, (uint8_t *)&block, 8, &Devices.err[id]);
            }
            break;
          default:
            break;
        }
      }
    }
  }
  return false;
}

DXAPIDLL bool DXL_SetGoalAnglesAndTime2 (TDeviceID dvid, const uint8_t *ids, const double *angles, int num, double sec) {
#ifdef _MSC_VER
  __pragma (pack (push, 1))
  typedef struct {
    uint8_t  id;
    uint32_t ProfileVelocity;
    uint32_t GoalPosition;
  } Tid_velo_pos_L;
  struct {
    uint8_t         num;
    uint8_t         addr;
    uint8_t         size;
    Tid_velo_pos_L  data[254];
  } SyncData_L;
  __pragma (pack (pop))
#elif defined(__GNUC__)
  typedef struct {
    uint8_t  id;
    uint32_t ProfileVelocity;
    uint32_t GoalPosition;
  } _PACKED_ Tid_velo_pos_L;
  struct {
    uint8_t         num;
    uint8_t         addr;
    uint8_t         size;
    Tid_velo_pos_L  data[254];
  } _PACKED_ SyncData_L;
#endif
  initarray();
  SyncData_L.addr = ADDRESS_X_PROF_VELOCITY;
  SyncData_L.size = 8;
  if (ids != NULL && angles != NULL) {
    for (int i = 0; i < num; i++) {
      uint8_t id = ids[i];
      if (id <= 253) {
        int idx = Devices.id2model[id];
        if (idx >= 2) {
          int32_t pmax, pmin;
          if (Devices.opmode[id] == 3) {
            pmax = ModelInfoList[idx].positionlimit.max;
            pmin = ModelInfoList[idx].positionlimit.min;
          } else {
            pmax = INT32_MAX;
            pmin = INT32_MIN;
          }
          if ((Devices.drivemode[id] & 0x4) != 0) {
            int32_t pos = max (min (ang2pos (angles[i], &ModelInfoList[idx]), pmax), pmin);
            int32_t profileacc = abs (max (min (sec * 1000.0, 32767), 0));
            switch (ModelInfoList[idx].devtype) {
              case devtX:
                SyncData_L.data[SyncData_L.num].id = id;
                SyncData_L.data[SyncData_L.num].GoalPosition = pos;
                SyncData_L.data[SyncData_L.num].ProfileVelocity = profileacc;
                SyncData_L.num++;
                break;
              default:
                break;
            }
          }
        }
      }
    }
    bool result = false;
    if (SyncData_L.num > 0) result = DX_WriteSyncData (dvid, (uint8_t *)&SyncData_L.addr, 2 + 9 * SyncData_L.num, NULL);
    return result;
  }
  return false;
}

//-------------------------------------------------
// 電流指令
//-------------------------------------------------
DXAPIDLL bool DXL_SetGoalCurrent (TDeviceID dvid, uint8_t id, double current) {
  initarray();
  if (id <= 253) {
    int idx = Devices.id2model[id];
    if (idx >= 2) {
      switch (ModelInfoList[idx].devtype) {
        case devtMX: {
            int32_t cur = current / ModelInfoList[idx].currentratio;
            if (cur < 0) cur = 0x400 | abs (cur);
            return DX_WriteWordData (dvid, id, ADDRESS_MX_GOAL_TORQULE, cur, &Devices.err[id]);
          }
          break;
        case devtX: {
            int16_t cur = current / ModelInfoList[idx].currentratio;
            return DX_WriteWordData (dvid, id, ADDRESS_X_GOAL_CURRENT, cur, &Devices.err[id]);
          }
          break;
        default:
          break;
      }
    }
  }
  return false;
}

DXAPIDLL bool DXL_SetGoalCurrents (TDeviceID dvid, const uint8_t *ids, const double *currents, int num) {
#ifdef _MSC_VER
  __pragma (pack (push, 1))
  typedef struct {
    uint8_t  id;
    uint16_t GoalCurrent;
  } Tid_cur;
  struct {
    uint8_t num;
    uint8_t addr;
    uint8_t size;
    Tid_cur data[254];
  } SyncDataMX, SyncDataX;
  __pragma (pack (pop))
#elif defined(__GNUC__)
  typedef struct {
    uint8_t  id;
    int16_t  GoalCurrent;
  } _PACKED_ Tid_cur;
  struct {
    uint8_t num;
    uint8_t addr;
    uint8_t size;
    Tid_cur data[254];
  } _PACKED_ SyncDataMX, SyncDataX;
#endif
  initarray();
  SyncDataMX.num = SyncDataX.num = 0;
  SyncDataMX.addr = ADDRESS_MX_GOAL_TORQULE;
  SyncDataMX.size = 2;
  SyncDataX.addr = ADDRESS_X_GOAL_CURRENT;
  SyncDataX.size = 2;
  if (ids != NULL && currents != NULL) {
    for (int i = 0; i < num; i++) {
      uint8_t id = ids[i];
      if (id <= 253) {
        int idx = Devices.id2model[id];
        if (idx >= 2) {
          int32_t cur = currents[i] / ModelInfoList[idx].currentratio;
          switch (ModelInfoList[idx].devtype) {
            case devtMX:
              SyncDataMX.data[SyncDataMX.num].id = id;
              if (cur < 0) cur = 0x400 | abs (cur);
              SyncDataMX.data[SyncDataMX.num].GoalCurrent = cur;
              SyncDataMX.num++;
              break;
            case devtX:
              SyncDataX.data[SyncDataX.num].id = id;
              SyncDataX.data[SyncDataX.num].GoalCurrent = cur;
              SyncDataX.num++;
              break;
            default:
              break;
          }
        }
      }
    }
    bool result = false;
    if (SyncDataMX.num > 0) result = DX_WriteSyncData (dvid, (uint8_t *)&SyncDataMX.addr, 2 + 3 * SyncDataMX.num, NULL);
    if (SyncDataX.num > 0) result = ((SyncDataMX.num > 0) ? result : true) && DX_WriteSyncData (dvid, (uint8_t *)&SyncDataX.addr, 2 + 5 * SyncDataX.num, NULL);
    return result;
  }
  return false;
}

//-------------------------------------------------
// 電流取得
//-------------------------------------------------
DXAPIDLL bool DXL_GetPresentCurrent (TDeviceID dvid, uint8_t id, double *current) {
  initarray();
  if (current != NULL) {
    if (id <= 253) {
      int idx = Devices.id2model[id];
      if (idx >= 2) {
        int16_t cur;
        switch (ModelInfoList[idx].devtype) {
          case devtDX:
          case devtAX:
          case devtRX:
            if (DX_ReadWordData (dvid, id, ADDRESS_PRESENT_LOAD, (uint16_t *)&cur, &Devices.err[id])) {
              if (cur & 0x400) cur = - (cur & 0x3ff);
              *current = (double)cur * ModelInfoList[idx].currentratio;
              return true;
            }
            break;
          case devtEX:
            if (DX_ReadWordData (dvid, id, ADDRESS_EX_SENSED_CURRENT, (uint16_t *)&cur, &Devices.err[id])) {
              cur -= 512;
              *current = (double)cur * ModelInfoList[idx].currentratio;
              return true;
            }
            break;
          case devtMX:
            switch (ModelInfoList[idx].modelno) {
              case 0x0068: // MX-12W
              case 0x001D: // MX-28
                if (DX_ReadWordData (dvid, id, ADDRESS_PRESENT_LOAD, (uint16_t *)&cur, &Devices.err[id])) {
                  if (cur & 0x400) cur = - (cur & 0x3ff);
                  *current = (double)cur * ModelInfoList[idx].currentratio;
                  return true;
                }
                break;
              case 0x0136: // MX-64
              case 0x0140: // MX-106
                if (DX_ReadWordData (dvid, id, ADDRESS_MX_CURRENT, (uint16_t *)&cur, &Devices.err[id])) {
                  cur -= 2048;
                  *current = (double)cur * ModelInfoList[idx].currentratio;
                  return true;
                }
                break;
              default:
                break;
            }
            break;
          case devtX:
            if (DX_ReadWordData (dvid, id, ADDRESS_X_PRESENT_CURRENT, (uint16_t *)&cur, &Devices.err[id])) {
              *current = (double)cur * ModelInfoList[idx].currentratio;
              return true;
            }
            break;
          default:
            break;
        }
      }
    }
  }
  return false;
}

DXAPIDLL bool DXL_GetPresentCurrents (TDeviceID dvid, const uint8_t *ids, double *currents, int num) {
  bool result = true;
  initarray();
  if (ids != NULL && currents != NULL) {
    for (int i = 0; i < num; i++) {
      int idx = Devices.id2model[ids[i]];
      if (idx >= 2) {
        if (!DXL_GetPresentCurrent (dvid, ids[i], &currents[i])) result = false;
      }
    }
  }
  return result;
}

//-------------------------------------------------
// PWM指令
//-------------------------------------------------
DXAPIDLL bool DXL_SetGoalPWM (TDeviceID dvid, uint8_t id, double pwm) {
  initarray();
  if (id <= 253) {
    int idx = Devices.id2model[id];
    if (idx >= 2) {
      switch (ModelInfoList[idx].devtype) {
        case devtX: {
            int32_t p = max (min (pwm / ModelInfoList[idx].pwmratio, ModelInfoList[idx].pwmlimit.max), ModelInfoList[idx].pwmlimit.min);
            return DX_WriteWordData (dvid, id, ADDRESS_X_GOAL_PWM, p, &Devices.err[id]);
          }
          break;
        default:
          break;
      }
    }
  }
  return false;
}

DXAPIDLL bool DXL_SetGoalPWMs (TDeviceID dvid, const uint8_t *ids, const double *pwms, int num) {
#ifdef _MSC_VER
  __pragma (pack (push, 1))
  typedef struct {
    uint8_t  id;
    uint16_t GoalPWM;
  } Tid_pwm;
  struct {
    uint8_t num;
    uint8_t addr;
    uint8_t size;
    Tid_pwm data[254];
  } SyncDataX;
  __pragma (pack (pop))
#elif defined(__GNUC__)
  typedef struct {
    uint8_t  id;
    uint16_t GoalPWM;
  } _PACKED_ Tid_pwm;
  struct {
    uint8_t num;
    uint8_t addr;
    uint8_t size;
    Tid_pwm data[254];
  } _PACKED_ SyncDataX;
#endif
  initarray();
  SyncDataX.num = 0;
  SyncDataX.addr = ADDRESS_X_GOAL_PWM;
  SyncDataX.size = 2;
  if (ids != NULL && pwms != NULL) {
    for (int i = 0; i < num; i++) {
      uint8_t id = ids[i];
      if (id <= 253) {
        int idx = Devices.id2model[id];
        if (idx >= 2) {
          int32_t pwm = max (min (pwms[i] / ModelInfoList[idx].pwmratio, ModelInfoList[idx].pwmlimit.max), ModelInfoList[idx].pwmlimit.min);
          switch (ModelInfoList[idx].devtype) {
            case devtX:
              SyncDataX.data[SyncDataX.num].id = id;
              SyncDataX.data[SyncDataX.num].GoalPWM = pwm;
              SyncDataX.num++;
              break;
            default:
              break;
          }
        }
      }
    }
    bool result = false;
//    if (SyncDataMX.num > 0) result = DX_WriteSyncData (dvid, (uint8_t *)&SyncDataMX.addr, 2 + 3 * SyncDataMX.num, NULL);
    if (SyncDataX.num > 0) result = ((SyncDataX.num > 0) ? result : true) && DX_WriteSyncData (dvid, (uint8_t *)&SyncDataX.addr, 2 + 5 * SyncDataX.num, NULL);
    return result;
  }
  return false;
}

//-------------------------------------------------
// PWM取得
//-------------------------------------------------
DXAPIDLL bool DXL_GetPresentPWM (TDeviceID dvid, uint8_t id, double *pwm) {
  initarray();
  if (pwm != NULL) {
    if (id <= 253) {
      int idx = Devices.id2model[id];
      if (idx >= 2) {
        int16_t p;
        switch (ModelInfoList[idx].devtype) {
          case devtX:
            if (DX_ReadWordData (dvid, id, ADDRESS_X_PRESENT_PWM, (uint16_t *)&p, &Devices.err[id])) {
              *pwm = (double)p * ModelInfoList[idx].pwmratio;
              return true;
            }
            break;
          default:
            break;
        }
      }
    }
  }
  return false;
}

DXAPIDLL bool DXL_GetPresentPWMs (TDeviceID dvid, const uint8_t *ids, double *pwms, int num) {
  bool result = true;
  initarray();
  if (ids != NULL && pwms != NULL) {
    for (int i = 0; i < num; i++) {
      int idx = Devices.id2model[ids[i]];
      if (idx >= 2) {
        if (!DXL_GetPresentPWM (dvid, ids[i], &pwms[i])) result = false;
      }
    }
  }
  return result;
}

//-------------------------------------------------
// ドライブモード変更
//-------------------------------------------------
/*
   modeの値はモデル依存(以下はXシリーズの場合)
    bit3: Torque On by Goal Update
    bit2: Profile Config (0:Velocity-based, 1:Time-based)
    bit1: Master/Slave Mode (0:Master, 1:Slave)
    bit0: Normal/Reverse Mode (0:CCW Positive, 1:CW Positive)
 */
DXAPIDLL bool DXL_SetDriveMode (TDeviceID dvid, uint8_t id, uint8_t mode) {
  initarray();
  if (id <= 252) {
    int idx = Devices.id2model[id];
    if (idx >= 2) {
      uint8_t rmode;
      if (DXL_GetOperatingMode (dvid, id, &rmode)) {
        if (mode == Devices.drivemode[id]) {
          return true;
        } else {
          if (DXL_SetTorqueEnable (dvid, id, false)) {
            switch (ModelInfoList[idx].devtype) {
              case devtX:
                if (DX_WriteByteData (dvid, id, ADDRESS_X_DRIVE_MODE, mode, &Devices.err[id])) {
                  if (DXL_GetOperatingMode (dvid, id, &rmode)) {
                    return (mode == Devices.drivemode[id]);
                  }
                }
                break;
              default:
                break;
            }
          }
        }
      }
    }
  }
  return false;
}

DXAPIDLL bool DXL_SetDriveModesEquival (TDeviceID dvid, const uint8_t *ids, int num, uint8_t mode) {
  bool result = true;
  initarray();
  if (ids != NULL) {
    for (int i = 0; i < num; i++) {
      int idx = Devices.id2model[ids[i]];
      if (idx >= 2) {
        if (!DXL_SetDriveMode (dvid, ids[i], mode)) result = false;
      }
    }
  }
  return result;
}

//-------------------------------------------------
// 動作モード変更
//-------------------------------------------------
/*
   modeの値はXシリーズ準拠
    0: cur
    1: velo
    3: pos
    4: expos(pos+multiturn)
    5: expos(pos+multiturn+cur)
    16:PWM
 */
DXAPIDLL bool DXL_SetOperatingMode (TDeviceID dvid, uint8_t id, uint8_t mode) {
  initarray();
  if (id <= 253) {
    int idx = Devices.id2model[id];
    if (idx >= 2) {
      uint8_t rmode = 255;
      if (DXL_GetOperatingMode (dvid, id, &rmode)) {
        if (mode == rmode) {
          return true;
        } else {
          if (DXL_SetTorqueEnable (dvid, id, false)) {
            switch (ModelInfoList[idx].devtype) {
              case devtDX:
              case devtAX:
              case devtRX:
                switch (mode) {
                  case 1: // velo
                    if (DX_WriteLongData (dvid, id, ADDRESS_CW_ANGLE_LIMIT, 0UL, &Devices.err[id])) {
                      Sleep (100);
                      if (DXL_GetOperatingMode (dvid, id, &rmode)) {
                        return (mode == rmode);
                      }
                    }
                    break;
                  case 3: // pos
                    if (DX_WriteLongData (dvid, id, ADDRESS_CW_ANGLE_LIMIT, 0x03ff0000UL, &Devices.err[id])) {
                      Sleep (100);
                      if (DXL_GetOperatingMode (dvid, id, &rmode)) {
                        return (mode == rmode);
                      }
                    }
                    break;
                }
                break;
              case devtEX:
                switch (mode) {
                  case 1: // velo
                    if (DX_WriteLongData (dvid, id, ADDRESS_CW_ANGLE_LIMIT, 0UL, &Devices.err[id])) {
                      Sleep (100);
                      if (DXL_GetOperatingMode (dvid, id, &rmode)) {
                        return (mode == rmode);
                      }
                    }
                    break;
                  case 3: // pos
                    if (DX_WriteLongData (dvid, id, ADDRESS_CW_ANGLE_LIMIT, 0x0fff0000UL, &Devices.err[id])) {
                      Sleep (100);
                      if (DXL_GetOperatingMode (dvid, id, &rmode)) {
                        return (mode == rmode);
                      }
                    }
                    break;
                  default:
                    return false;
                }
                break;
              case devtMX:
                switch (mode) {
                  case 0: // cur
                    if (DX_WriteLongData (dvid, id, ADDRESS_CW_ANGLE_LIMIT, 0x0fff0000UL, &Devices.err[id])) {
                      Sleep (100);
                      if (DX_WriteByteData (dvid, id, ADDRESS_MX_TORQUE_MODE, 1, &Devices.err[id])) {
                        if (DXL_GetOperatingMode (dvid, id, &rmode)) {
                          return (mode == rmode);
                        }
                      }
                    }
                    break;
                  case 1: // velo
                    if (DX_WriteLongData (dvid, id, ADDRESS_CW_ANGLE_LIMIT, 0UL, &Devices.err[id])) {
                      Sleep (100);
                      // MX-28以外
                      if (ModelInfoList[idx].modelno != 0x001D) {
                        if (DX_WriteByteData (dvid, id, ADDRESS_MX_TORQUE_MODE, 0, &Devices.err[id])) {
                          if (DXL_GetOperatingMode (dvid, id, &rmode)) {
                            return (mode == rmode);
                          }
                        }
                      } else if (DXL_GetOperatingMode (dvid, id, &rmode)) {
                        return (mode == rmode);
                      }
                    }
                    break;
                  case 3: // pos
                    if (DX_WriteLongData (dvid, id, ADDRESS_CW_ANGLE_LIMIT, 0x0fff0000UL, &Devices.err[id])) {
                      Sleep (100);
                      // MX-28以外
                      if (ModelInfoList[idx].modelno != 0x001D) {
                        if (DX_WriteByteData (dvid, id, ADDRESS_MX_TORQUE_MODE, 0, &Devices.err[id])) {
                          if (DXL_GetOperatingMode (dvid, id, &rmode)) {
                            return (mode == rmode);
                          }
                        }
                      } else if (DXL_GetOperatingMode (dvid, id, &rmode)) {
                        return (mode == rmode);
                      }
                    }
                    break;
                  case 4: // expos
                    if (DX_WriteLongData (dvid, id, ADDRESS_CW_ANGLE_LIMIT, 0x0fff0fffUL, &Devices.err[id])) {
                      Sleep (100);
                      if (ModelInfoList[idx].modelno != 0x001D) {
                        if (DX_WriteByteData (dvid, id, ADDRESS_MX_TORQUE_MODE, 0, &Devices.err[id])) {
                          if (DXL_GetOperatingMode (dvid, id, &rmode)) {
                            return (mode == rmode);
                          }
                        }
                      } else if (DXL_GetOperatingMode (dvid, id, &rmode)) {
                        return (mode == rmode);
                      }
                    }
                    break;
                  default:
                    return false;
                }
                break;
              case devtX:
                if (DX_WriteByteData (dvid, id, ADDRESS_X_OPERATING_MODE, mode, &Devices.err[id])) {
                  if (DXL_GetOperatingMode (dvid, id, &rmode)) {
                    return (mode == rmode);
                  }
                }
                break;
              default:
                break;
            }
          }
        }
      }
    }
  }
  return false;
}

DXAPIDLL bool DXL_SetOperatingModesEquival (TDeviceID dvid, const uint8_t *ids, int num, uint8_t mode) {
  bool result = true;
  initarray();
  if (ids != NULL) {
    for (int i = 0; i < num; i++) {
      int idx = Devices.id2model[ids[i]];
      if (idx >= 2) {
        if (!DXL_SetOperatingMode (dvid, ids[i], mode)) result = false;
      }
    }
  }
  return result;
}

//-------------------------------------------------
// 動作モード取得
//-------------------------------------------------
/*
   modeの値はXシリーズ準拠
    0: cur
    1: velo
    3: pos
    4: expos(pos+multiturn)
    5: expos(pos+multiturn+cur)
    16:PWM
 */
DXAPIDLL bool DXL_GetOperatingMode (TDeviceID dvid, uint8_t id, uint8_t *mode) {
  initarray();
  if (mode != NULL) {
    if (id <= 253) {
      int idx = Devices.id2model[id];
      uint32_t  dat_l;
      uint8_t   dat_b;
      uint8_t   rmode = 255, rdmode = 0;
      if (idx >= 2) {
        switch (ModelInfoList[idx].devtype) {
          case devtDX:
          case devtAX:
          case devtRX:
            if (DX_ReadLongData (dvid, id, ADDRESS_CW_ANGLE_LIMIT, &dat_l, &Devices.err[id])) {
              if (dat_l == 0) {
                Devices.opmode[id] = (*mode = 1);
                return true;
              } else if (dat_l == 0x03ff0000UL) {
                Devices.opmode[id] = (*mode = 3);
                return true;
              }
              Devices.drivemode[id] = 0;
            }
            break;
          case devtEX:
            if (DX_ReadLongData (dvid, id, ADDRESS_CW_ANGLE_LIMIT, &dat_l, &Devices.err[id])) {
              if (dat_l == 0) {
                Devices.opmode[id] = (*mode = 1);
                return true;
              } else if (dat_l == 0x0fff0000UL) {
                Devices.opmode[id] = (*mode = 3);
                return true;
              }
              Devices.drivemode[id] = 0;
            }
            break;
          case devtMX:
            if (DX_ReadLongData (dvid, id, ADDRESS_CW_ANGLE_LIMIT, &dat_l, &Devices.err[id]) && DX_ReadByteData (dvid, id, ADDRESS_MX_TORQUE_MODE, &dat_b, &Devices.err[id])) {
              if (dat_b == 1) {
                Devices.opmode[id] = (*mode = 0);
                return true;
              } else {
                if (dat_l == 0UL) {
                  Devices.opmode[id] = (*mode = 1);
                  return true;
                } else if (dat_l == 0x0fff0000UL) {
                  Devices.opmode[id] = (*mode = 3);
                  return true;
                } else if (dat_l == 0x0fff0fffUL) {
                  Devices.opmode[id] = (*mode = 4);
                  return true;
                }
              }
              Devices.drivemode[id] = 0;
            }
            break;
          case devtX:
            if (DX_ReadByteData (dvid, id, ADDRESS_X_OPERATING_MODE, &rmode, &Devices.err[id]) && DX_ReadByteData (dvid, id, ADDRESS_X_DRIVE_MODE, &rdmode, &Devices.err[id])) {
              Devices.opmode[id] = (*mode = rmode);
              return true;
            }
          default:
            break;
        }
      }
    }
  }
  return false;
}

//-------------------------------------------------
// 指定IDのハードウェアエラーを取得
//-------------------------------------------------
DXAPIDLL bool DXL_GetHWErrorCode (TDeviceID dvid, uint8_t id, uint8_t *hwerr) {
  initarray();
  if (hwerr != NULL) {
    if (id <= 253) {
      int idx = Devices.id2model[id];
      if (idx >= 2) {
        switch (ModelInfoList[idx].devtype) {
          case devtX:
            return DX_ReadByteData (dvid, id, ADDRESS_X_HARDWARE_ERROR_STATUS, hwerr, &Devices.err[id]);
            break;
          default:
            break;
        }
      }
    }
  }
  return false;
}

//-------------------------------------------------
// 指定IDの記録済みエラーコードを取得
//-------------------------------------------------
DXAPIDLL TErrorCode DXL_GetErrorCode (TDeviceID dvid, uint8_t id) {
  initarray();
  if (id <= 253) {
    int idx = Devices.id2model[id];
    if (idx >= 2) return Devices.err[id];
  }
  return 0xffff;
}

//-------------------------------------------------
// 指定IDのデバイズ情報を取得
//-------------------------------------------------
DXAPIDLL PDXL_ModelInfo DXL_GetModelInfo (TDeviceID dvid, uint8_t id) {
  uint16_t    model;
  uint8_t     mode, rdelay;

  initarray();
  if (DX_ReadWordData (dvid, id, ADDRESS_MODEL_NUMBER, &model, NULL)) {
    for (int i = 2; i < (sizeof (ModelInfoList) / sizeof (ModelInfoList[0])); i++) {
      if (ModelInfoList[i].modelno == model) {
        // デバイスのReturn Statusを2に、Return Delay Timeを0に変更
        if (Devices.id2model[id] == 0) {
          switch (ModelInfoList[i].devtype) {
            case devtDX:
            case devtAX:
            case devtRX:
            case devtEX:
            case devtMX:
              DX_WriteByteData (dvid, id, ADDRESS_STATUS_RET_LEVEL, 2, &Devices.err[id]);
              if (DX_ReadByteData (dvid, id, ADDRESS_RETURN_DELAY_TIME, &rdelay, &Devices.err[id])) {
                if (rdelay != 0) {
                  DXL_SetTorqueEnable (dvid, id, false);
                  DX_WriteByteData (dvid, id, ADDRESS_RETURN_DELAY_TIME, 0, &Devices.err[id]);
                  Sleep (100);
                }
              }
              break;
            case devtX:
              DX_WriteByteData (dvid, id, ADDRESS_X_STATUS_RETURN_LEVEL, 2, &Devices.err[id]);
              DXL_SetTorqueEnable (dvid, id, false);
              DX_WriteByteData (dvid, id, ADDRESS_X_RETURN_DELAY_TIME, 0, &Devices.err[id]);
              break;
            default:
              break;
          }
        }
        Devices.id2model[id] = i;
        DXL_GetOperatingMode (dvid, id, &mode);
        return (PDXL_ModelInfo)&ModelInfoList[i];
      }
    }
    Devices.id2model[id] = 1;
    return (PDXL_ModelInfo)&ModelInfoList[1];
  }
  Devices.id2model[id] = 0;
  return (PDXL_ModelInfo)&ModelInfoList[0];
}

//-------------------------------------------------
// 接続されたデバイスのリストを構成
//-------------------------------------------------
DXAPIDLL int DXL_ScanDevices (TDeviceID dvid, uint8_t *ids) {
  Devices.num = 0;
  initarray();
  for (int id = 0; id <= 253; id++) {
    Devices.id2model[id] = 0;
    Devices.err[id] = 0xffff;
    // デバイスのReturn Delay Timeを0に変更
    switch (DXL_GetModelInfo (dvid, id)->devtype) {
      case devtDX:
      case devtAX:
      case devtRX:
      case devtEX:
      case devtMX:
      case devtX:
        if (ids != NULL) {
          *ids = id;
          ids++;
        }
        Devices.num++;
        break;
      default:
        break;
    }
  }
  return Devices.num;
}

//-------------------------------------------------
// スキャン済みデバイスリストに含まれるデバイスの一覧を表示
//-------------------------------------------------
DXAPIDLL bool DXL_PrintDevicesList (int (*pf) (const char *, ...)) {
  bool result = false;
  initarray();
  for (int id = 0; id <= 253; id++) {
    if (Devices.id2model[id] != 0) pf ("[%3d] %-15s($%04X)\n", id, ModelInfoList[Devices.id2model[id]].name, ModelInfoList[Devices.id2model[id]].modelno);
    result = true;
  }
  return result;
}

//-------------------------------------------------
// スキャン済みデバイスリストを初期状態に戻す
//-------------------------------------------------
DXAPIDLL void DXL_InitDevicesList (void) {
  Devices.num = 0;
  for (int i = 0; i < 256; i++) {
    Devices.id2model[i] = 0;
    Devices.err[i] = 0xffff;
    Devices.opmode[i] = 255;
  }
}
