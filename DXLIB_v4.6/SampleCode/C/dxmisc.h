/*
  dxmisc.h
   サンプルプログラムで使用するポート名・ボーレート・IDのデフォルト値
   その他Dynamixelのモデルから種別取得とLinux上のSleepを提供
   Windows/Linux/macOSで使用可
*/

#ifndef _DXMISC
#define _DXMISC

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#ifndef _WIN32  // LinuxやmacOS時
#include <time.h>
#include <sys/time.h>
#endif

// マクロ定義
#if defined _WIN32 || defined __CYGWIN__ // Windows/MSYS2時
#define _COMPORT    "\\\\.\\COM3"   // ポート名
#define _COMPORT2   "\\\\.\\COM4"
#else       // LinuxやmacOS時
#define _COMPORT    "/dev/ttyUSB0"  // ポート名
#define _COMPORT2   "/dev/ttyUSB1"
#endif
#define _BAUDRATE   (57143)         // ボーレート[bps]
#define _TARGETID   (1)             // 対象ID
#define _TARGETID2  (2)             // 対象ID

#ifndef _WIN32  // LinuxやmacOS時
//------------------------------------
// ミリ秒スリープ
//------------------------------------
void Sleep (int milliseconds) {
  struct timespec ts;
  ts.tv_sec = milliseconds / 1000;
  ts.tv_nsec = (milliseconds % 1000) * 1000000;
  nanosleep(&ts, NULL);
}
#endif

//------------------------------------
// モデル番号からシリーズを特定
//------------------------------------
TDXL_DevType CheckType (uint16_t modelno) {
  switch (modelno) {
    case 0x0071:  // DX-113
    case 0x0074:  // DX-116
    case 0x0075:  // DX-117
      return devtDX;

    case 0x002C:  // AX-12W
    case 0x000C:  // AX-12
    case 0x0012:  // AX-18
      return devtAX;

    case 0x000A:  // RX-10
    case 0x0018:  // RX-24F
    case 0x001C:  // RX-28
    case 0x0040:  // RX-64
    case 0x006B:  // EX-106+
      return devtEX;

    case 0x0068:  // MX-12W
    case 0x001D:  // MX-28
    case 0x0136:  // MX-64
    case 0x0140:  // MX-106
      return devtMX;

    case 0x001E:  // MX-28(2.0)
    case 0x0137:  // MX-64(2.0)
    case 0x0141:  // MX-106(2.0)

    case 0x0424:  // XL430-W250
    case 0x0442:  // 2XL430-W250
    case 0x0488:  // 2XC430-W250
    case 0x042E:  // XC430-W150
    case 0x0438:  // XC430-W240
    case 0x0406:  // XM430-W210
    case 0x03F2:  // XH430-W210
    case 0x041A:  // XH430-V210
    case 0x03FC:  // XM430-W350
    case 0x03E8:  // XH430-W350
    case 0x0410:  // XH430-V350
    case 0x046A:  // XM540-W150
    case 0x0456:  // XH540-W150
    case 0x047E:  // XH540-V150
    case 0x0460:  // XM540-W270
    case 0x044C:  // XH540-W270
    case 0x0474:  // XH540-V270
    case 0x049C:  // XW540-T140
    case 0x0492:  // XW540-T260
      return devtX;

    // その他
    default:
      return devtNONE;
  }
}

#ifdef __cplusplus
}
#endif

#endif //_DXMISC
