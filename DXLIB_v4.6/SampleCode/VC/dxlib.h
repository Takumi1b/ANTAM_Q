/*----------------------------------------------------------*
    dxlib.h
    Dynamixelプロトコル用API V4.6
                                       Last Edit '22 03/01
   Copyright (c) 2005, 2022 BestTechnology CO.,LTD.
 *----------------------------------------------------------*/

#ifndef _DXLIB_H_INCLUDE
#define _DXLIB_H_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

#ifndef _MSC_VER
#include <stdbool.h>
#endif

#include <stdint.h>

#define BROADCASTING_ID     (0xfe)

#define DXMAXLENGTH         (250)

#ifndef _WIN32
#ifdef __MAKE_LIB__
typedef int                             HANDLE;
typedef pthread_mutex_t                 CRITICAL_SECTION;

#define EnterCriticalSection(mtx)       pthread_mutex_lock(mtx)
#define LeaveCriticalSection(mtx)       pthread_mutex_unlock(mtx)
#define DeleteCriticalSection(mtx)      pthread_mutex_destroy(mtx)
#define InitializeCriticalSection(mtx)  pthread_mutex_init(mtx,NULL)
#define SetEvent(evt)                   pthread_cond_signal(evt)
#define INVALID_int_VALUE               -1
#define INVALID_HANDLE_VALUE            -1
#endif
#endif

// Device ID
#ifdef _WIN32
#ifdef _WIN64
typedef uint64_t TDeviceID;
#else
typedef uint32_t TDeviceID;
#endif
#define _PACKED_  __attribute__((gcc_struct,__packed__))
#else
#ifdef __APPLE__
typedef uint64_t TDeviceID;
#define _PACKED_  __attribute__((__packed__))
#else
#if defined(__x86_64__) | defined(__aarch64__ )
typedef uint64_t TDeviceID;
#if defined(__x86_64__)
#define _PACKED_  __attribute__((gcc_struct,__packed__))
#else
#define _PACKED_  __attribute__((__packed__))
#endif
#else
typedef uint32_t TDeviceID;
#if defined(__x86__)
#define _PACKED_  __attribute__((gcc_struct,__packed__))
#else
#define _PACKED_  __attribute__((__packed__))
#endif
#endif
#endif
#endif

// Kind of instruction
typedef enum {INST_PING = 1, INST_READ = 2, INST_WRITE = 3, INST_REG_WRITE = 4, INST_ACTION = 5, INST_RESET = 6, INST_REBOOT = 8, INST_SYNC_WRITE = 0x83, INST_SYNG_REG_WRITE = 0x85} TInstruction;

// Error flags
typedef uint16_t TErrorCode;
#define ERR_INVALID_DEVID     (1 << 15)
#define ERR_INVALID_ID        (1 << 14)
#define ERR_DIFF_ID           (1 << 13)
#define ERR_ILLEGAL_SIZE      (1 << 12)
#define ERR_INVALID_PARAM     (1 << 11)
#define ERR_COMM              (1 << 10)
#define ERR_CHECKSUM          (1 << 9)
#define ERR_TIMEOUT           (1 << 8)
#define ERR_DX_INST           (1 << 6)
#define ERR_DX_OVERLOAD       (1 << 5)
#define ERR_DX_CHECKSUM       (1 << 4)
#define ERR_DX_RANGE          (1 << 3)
#define ERR_DX_OVERHEAT       (1 << 2)
#define ERR_DX_ANGLE          (1 << 1)
#define ERR_DX_OVERVOLTAGE    (1 << 0)
#define ERR_NON               (0)

// Alarm status
#ifdef _MSC_VER
__pragma (pack (push, 1))
typedef struct {
  uint8_t     id;
  TErrorCode  Status;
} TDxAlarmStatus;

typedef struct {
  uint8_t addr;
  uint8_t length;
  uint8_t ids[256];
} TSyncReadParam;
__pragma (pack (pop))
#elif defined(__GNUC__)
typedef struct {
  uint8_t     id;
  TErrorCode  Status;
} _PACKED_ TDxAlarmStatus;

typedef struct {
  uint8_t addr;
  uint8_t length;
  uint8_t ids[256];
} _PACKED_ TSyncReadParam;
#endif

// デバイスの種別
typedef enum {
  devtNONE, devtDX, devtAX, devtRX, devtEX, devtMX, devtXL320, devtPRO, devtPROP, devtX
} TDXL_DevType;

// 全シリーズモデル情報
#ifdef _MSC_VER
__pragma (pack (push, 1))
typedef struct {
  uint16_t  modelno;          // デバイス固有のモデル番号
  char      name[16];
  TDXL_DevType devtype;       // デバイスのグループ
  struct {                    // 位置の範囲
    int32_t max;
    int32_t min;
  }         positionlimit;
  struct {                    // 角度の範囲
    double  max;
    double  min;
  }         anglelimit;
  struct {                    // 速度の範囲
    int32_t max;
    int32_t min;
  }         velocitylimit;
  struct {                    // PWMの範囲
    int32_t max;
    int32_t min;
  }         pwmlimit;
  double    velocityratio;    // 角速度変換係数 [deg/sec]
  double    currentratio;     // 電流変換係数 [mA]
  double    pwmratio;         // PWM変換係数 [%]
} TDXL_ModelInfo, *PDXL_ModelInfo;
__pragma (pack (pop))
#elif defined(__GNUC__)
typedef struct {
  uint16_t  modelno;          // デバイス固有のモデル番号
  char      name[16];
  TDXL_DevType devtype;       // デバイスのグループ
  struct {                    // 位置の範囲
    int32_t max;
    int32_t min;
  } _PACKED_ positionlimit;
  struct {                    // 角度の範囲
    double  max;
    double  min;
  } _PACKED_ anglelimit;
  struct {                    // 速度の範囲
    int32_t max;
    int32_t min;
  } _PACKED_ velocitylimit;
  struct {                    // PWMの範囲
    int32_t max;
    int32_t min;
  } _PACKED_ pwmlimit;
  double    velocityratio;    // 角速度変換係数 [deg/sec]
  double    currentratio;     // 電流変換係数 [mA]
  double    pwmratio;         // PWM変換係数 [%]
} _PACKED_ TDXL_ModelInfo, *PDXL_ModelInfo;
#endif

//
typedef struct {
  double  angle;
  double  velocity;
} TAngleVelocity, *PAngleVelocity;

typedef struct {
  double  angle;
  double  sec;
} TAngleTime, *PAngleTime;

#ifdef _WIN32
#ifdef __cplusplus
#ifdef __MAKE_LIB__
#define DXAPIDLL extern "C" __declspec(dllexport)
#else
#define DXAPIDLL extern "C" __declspec(dllimport)
#endif
#else
#ifdef __MAKE_LIB__
#define DXAPIDLL __declspec(dllexport)
#else
#define DXAPIDLL __declspec(dllimport)
#endif
#endif
#else
#define DXAPIDLL
#define WINAPI
#endif

#ifndef _DYNAMICLOAD
// When static DLL calls is used the following prototype declaration
DXAPIDLL TDeviceID WINAPI DX_OpenPort (char *name, uint32_t baud);
DXAPIDLL bool WINAPI DX_ClosePort (TDeviceID dvid);
DXAPIDLL bool WINAPI DX_SetBaudrate (TDeviceID dvid, uint32_t baud);
DXAPIDLL bool WINAPI DX_Active (TDeviceID dvid);
DXAPIDLL void WINAPI DX_SetTimeOutOffset (TDeviceID dvid, uint32_t t);
DXAPIDLL double WINAPI GetQueryPerformanceCounter (void);
DXAPIDLL bool WINAPI DX_TxPacket (TDeviceID dvid, uint8_t id, TInstruction inst, const uint8_t *param, uint32_t len, TErrorCode *err);
DXAPIDLL bool WINAPI DX_RxPacket (TDeviceID dvid, uint8_t *rdata, uint32_t rdatasize, uint32_t *rlen, uint32_t timeout, TErrorCode *err);
DXAPIDLL bool WINAPI DX_ReadByteData (TDeviceID dvid, uint8_t id, uint16_t adr, uint8_t *rdata, TErrorCode *err);
DXAPIDLL bool WINAPI DX_WriteByteData (TDeviceID dvid, uint8_t id, uint16_t adr, uint8_t dat, TErrorCode *err);
DXAPIDLL bool WINAPI DX_ReadWordData (TDeviceID dvid, uint8_t id, uint16_t adr, uint16_t *rdata, TErrorCode *err);
DXAPIDLL bool WINAPI DX_WriteWordData (TDeviceID dvid, uint8_t id, uint16_t adr, uint16_t dat, TErrorCode *err);
DXAPIDLL bool WINAPI DX_ReadLongData (TDeviceID dvid, uint8_t id, uint16_t adr, uint32_t *rdata, TErrorCode *err);
DXAPIDLL bool WINAPI DX_WriteLongData (TDeviceID dvid, uint8_t id, uint16_t adr, uint32_t dat, TErrorCode *err);
DXAPIDLL bool WINAPI DX_ReadBlockData (TDeviceID dvid, uint8_t id, uint16_t adr, uint8_t *rdata, uint32_t len, TErrorCode *err);
DXAPIDLL bool WINAPI DX_WriteBlockData (TDeviceID dvid, uint8_t id, uint16_t adr, const uint8_t *dat, uint32_t len, TErrorCode *err);
DXAPIDLL bool WINAPI DX_Ping (TDeviceID dvid, uint8_t id, TErrorCode *err);
DXAPIDLL bool WINAPI DX_Ping2 (TDeviceID dvid, uint32_t *num, TDxAlarmStatus *AlarmStatus, TErrorCode *err);
//DXAPIDLL bool WINAPI DX_ReadSyncData (TDeviceID dvid, uint8_t *dat, uint32_t size, uint8_t *retdat, TErrorCode *err);
DXAPIDLL bool WINAPI DX_ReadSyncData (TDeviceID dvid, const TSyncReadParam *param, uint32_t *num, uint8_t *dat, TErrorCode *err);
DXAPIDLL bool WINAPI DX_WriteSyncData (TDeviceID dvid, uint8_t *dat, uint32_t size, TErrorCode *err);
DXAPIDLL bool WINAPI DX_Reset (TDeviceID dvid, uint8_t id, TErrorCode *err);
DXAPIDLL bool WINAPI DX_Reboot (TDeviceID dvid, uint8_t id, TErrorCode *err);

DXAPIDLL bool WINAPI DXL_SetLED (TDeviceID dvid, uint8_t id, bool en);
DXAPIDLL bool WINAPI DXL_SetTorqueEnable (TDeviceID dvid, uint8_t id, bool en);
DXAPIDLL bool WINAPI DXL_SetTorqueEnables (TDeviceID dvid, const uint8_t *ids, const bool *ens, int num);
DXAPIDLL bool WINAPI DXL_SetTorqueEnablesEquival (TDeviceID dvid, const uint8_t *ids, int num, bool en);
DXAPIDLL bool WINAPI DXL_GetTorqueEnable (TDeviceID dvid, uint8_t id, bool *en);
DXAPIDLL bool WINAPI DXL_GetTorqueEnables (TDeviceID dvid, const uint8_t *ids, bool *en, int num);
DXAPIDLL bool WINAPI DXL_SetGoalAngle (TDeviceID dvid, uint8_t id, double angle);
DXAPIDLL bool WINAPI DXL_SetGoalAngles (TDeviceID dvid, const uint8_t *ids, const double *angles, int num);
DXAPIDLL bool WINAPI DXL_GetPresentAngle (TDeviceID dvid, uint8_t id, double *angle);
DXAPIDLL bool WINAPI DXL_GetPresentAngles (TDeviceID dvid, const uint8_t *ids, double *angles, int num);
DXAPIDLL bool WINAPI DXL_StandStillAngle (TDeviceID dvid, uint8_t id);
DXAPIDLL bool WINAPI DXL_StandStillAngles (TDeviceID dvid, const uint8_t *ids, int num);
DXAPIDLL bool WINAPI DXL_SetGoalVelocity (TDeviceID dvid, uint8_t id, double velocity);
DXAPIDLL bool WINAPI DXL_SetGoalVelocities (TDeviceID dvid, const uint8_t *ids, const double *velocities, int num);
DXAPIDLL bool WINAPI DXL_GetPresentVelocity (TDeviceID dvid, uint8_t id, double *velocity);
DXAPIDLL bool WINAPI DXL_GetPresentVelocities (TDeviceID dvid, const uint8_t *ids, double *velocities, int num);
DXAPIDLL bool WINAPI DXL_SetGoalAngleAndVelocity (TDeviceID dvid, uint8_t id, double angle, double velocity);
DXAPIDLL bool WINAPI DXL_SetGoalAnglesAndVelocities (TDeviceID dvid, const uint8_t *ids, PAngleVelocity anglevelocity, int num);
DXAPIDLL bool WINAPI DXL_SetGoalAngleAndTime (TDeviceID dvid, uint8_t id, double angle, double sec);
DXAPIDLL bool WINAPI DXL_SetGoalAnglesAndTime (TDeviceID dvid, const uint8_t *ids, const double *angles, int num, double sec);
DXAPIDLL bool WINAPI DXL_SetGoalAngleAndTime2 (TDeviceID dvid, uint8_t id, double angle, double sec);
DXAPIDLL bool WINAPI DXL_SetGoalAnglesAndTime2 (TDeviceID dvid, const uint8_t *ids, const double *angles, int num, double sec);
DXAPIDLL bool WINAPI DXL_SetGoalCurrent (TDeviceID dvid, uint8_t id, double current);
DXAPIDLL bool WINAPI DXL_SetGoalCurrents (TDeviceID dvid, const uint8_t *ids, const double *currents, int num);
DXAPIDLL bool WINAPI DXL_GetPresentCurrent (TDeviceID dvid, uint8_t id, double *current);
DXAPIDLL bool WINAPI DXL_GetPresentCurrents (TDeviceID dvid, const uint8_t *ids, double *currents, int num);
DXAPIDLL bool WINAPI DXL_SetGoalPWM (TDeviceID dvid, uint8_t id, double pwm);
DXAPIDLL bool WINAPI DXL_SetGoalPWMs (TDeviceID dvid, const uint8_t *ids, const double *pwms, int num);
DXAPIDLL bool WINAPI DXL_GetPresentPWM (TDeviceID dvid, uint8_t id, double *pwm);
DXAPIDLL bool WINAPI DXL_GetPresentPWMs (TDeviceID dvid, const uint8_t *ids, double *pwms, int num);
DXAPIDLL bool WINAPI DXL_SetDriveMode (TDeviceID dvid, uint8_t id, uint8_t mode);
DXAPIDLL bool WINAPI DXL_SetDriveModesEquival (TDeviceID dvid, const uint8_t *ids, int num, uint8_t mode);
DXAPIDLL bool WINAPI DXL_SetOperatingMode (TDeviceID dvid, uint8_t id, uint8_t mode);
DXAPIDLL bool WINAPI DXL_SetOperatingModesEquival (TDeviceID dvid, const uint8_t *ids, int num, uint8_t mode);
DXAPIDLL bool WINAPI DXL_GetOperatingMode (TDeviceID dvid, uint8_t id, uint8_t *mode);
DXAPIDLL bool WINAPI DXL_GetHWErrorCode (TDeviceID dvid, uint8_t id, uint8_t *hwerr);
DXAPIDLL TErrorCode WINAPI DXL_GetErrorCode (TDeviceID dvid, uint8_t id);
DXAPIDLL PDXL_ModelInfo WINAPI DXL_GetModelInfo (TDeviceID dvid, uint8_t id);
DXAPIDLL int WINAPI DXL_ScanDevices (TDeviceID dvid, uint8_t *ids);
DXAPIDLL bool WINAPI DXL_PrintDevicesList (int (*pf) (const char *, ...));
DXAPIDLL void WINAPI DXL_InitDevicesList (void);

#else

#ifdef _WIN32
// When the dynamic DLL calls is used the following prototype declaration
TDeviceID (WINAPI *DX_OpenPort) (char *name, uint32_t baud) = NULL;
bool (WINAPI *DX_ClosePort) (TDeviceID dvid) = NULL;
bool (WINAPI *DX_SetBaudrate) (TDeviceID dvid, uint32_t baud) = NULL;
bool (WINAPI *DX_Active) (TDeviceID dvid) = NULL;
void (WINAPI *DX_SetTimeOutOffset) (TDeviceID dvid, uint32_t offsettime) = NULL;
double (WINAPI *GetQueryPerformanceCounter) (void) = NULL;
bool (WINAPI *DX_TxPacket) (TDeviceID dvid, uint8_t id, TInstruction inst, const uint8_t *param, uint32_t len, TErrorCode *err) = NULL;
bool (WINAPI *DX_RxPacket) (TDeviceID dvid, uint8_t *rdata, uint32_t rdatasize, uint32_t *rlen, uint32_t timeout, TErrorCode *err) = NULL;
bool (WINAPI *DX_ReadByteData) (TDeviceID dvid, uint8_t id, uint16_t adr, uint8_t *rdata, TErrorCode *err) = NULL;
bool (WINAPI *DX_WriteByteData) (TDeviceID dvid, uint8_t id, uint16_t adr, uint8_t dat, TErrorCode *err) = NULL;
bool (WINAPI *DX_ReadWordData) (TDeviceID dvid, uint8_t id, uint16_t adr, uint16_t *rdata, TErrorCode *err) = NULL;
bool (WINAPI *DX_WriteWordData) (TDeviceID dvid, uint8_t id, uint16_t adr, uint16_t dat, TErrorCode *err) = NULL;
bool (WINAPI *DX_ReadLongData) (TDeviceID dvid, uint8_t id, uint16_t adr, uint32_t *rdata, TErrorCode *err) = NULL;
bool (WINAPI *DX_WriteLongData) (TDeviceID dvid, uint8_t id, uint16_t adr, uint32_t dat, TErrorCode *err) = NULL;
bool (WINAPI *DX_ReadBlockData) (TDeviceID dvid, uint8_t id, uint16_t adr, uint8_t *rdata, uint32_t len, TErrorCode *err) = NULL;
bool (WINAPI *DX_WriteBlockData) (TDeviceID dvid, uint8_t id, uint16_t adr, const uint8_t *dat, uint32_t len, TErrorCode *err) = NULL;
bool (WINAPI *DX_Ping) (TDeviceID dvid, uint8_t id, TErrorCode *err) = NULL;
bool (WINAPI *DX_Ping2) (TDeviceID dvid, uint32_t *num, TDxAlarmStatus *AlarmStatus, TErrorCode *err) = NULL;
//bool (WINAPI *DX_ReadSyncData) (TDeviceID dvid, uint8_t *dat, uint32_t size, uint8_t *retdat, TErrorCode *err) = NULL;
bool (WINAPI *DX_ReadSyncData) (TDeviceID dvid, const TSyncReadParam *param, uint32_t *num, uint8_t *dat, TErrorCode *err) = NULL;
bool (WINAPI *DX_WriteSyncData) (TDeviceID dvid, uint8_t *dat, uint32_t size, TErrorCode *err) = NULL;
bool (WINAPI *DX_Reset) (TDeviceID dvid, uint8_t id, TErrorCode *err) = NULL;
bool (WINAPI *DX_Reboot) (TDeviceID dvid, uint8_t id, TErrorCode *err) = NULL;

bool (WINAPI *DXL_SetLED) (TDeviceID dvid, uint8_t id, bool en) = NULL;
bool (WINAPI *DXL_SetTorqueEnable) (TDeviceID dvid, uint8_t id, bool en) = NULL;
bool (WINAPI *DXL_SetTorqueEnables) (TDeviceID dvid, const uint8_t *ids, const bool *ens, int num) = NULL;
bool (WINAPI *DXL_SetTorqueEnablesEquival) (TDeviceID dvid, const uint8_t *ids, int num, bool en) = NULL;
bool (WINAPI *DXL_GetTorqueEnable) (TDeviceID dvid, uint8_t id, bool *en) = NULL;
bool (WINAPI *DXL_GetTorqueEnables) (TDeviceID dvid, const uint8_t *ids, bool *en, int num) = NULL;
bool (WINAPI *DXL_SetGoalAngle) (TDeviceID dvid, uint8_t id, double angle) = NULL;
bool (WINAPI *DXL_SetGoalAngles) (TDeviceID dvid, const uint8_t *ids, const double *angles, int num) = NULL;
bool (WINAPI *DXL_GetPresentAngle) (TDeviceID dvid, uint8_t id, double *angle) = NULL;
bool (WINAPI *DXL_GetPresentAngles) (TDeviceID dvid, const uint8_t *ids, double *angles, int num) = NULL;
bool (WINAPI *DXL_StandStillAngle) (TDeviceID dvid, uint8_t id);
bool (WINAPI *DXL_StandStillAngles) (TDeviceID dvid, const uint8_t *ids, int num);
bool (WINAPI *DXL_SetGoalVelocity) (TDeviceID dvid, uint8_t id, double velocity) = NULL;
bool (WINAPI *DXL_SetGoalVelocities) (TDeviceID dvid, const uint8_t *ids, const double *velocities, int num) = NULL;
bool (WINAPI *DXL_GetPresentVelocity) (TDeviceID dvid, uint8_t id, double *velocity) = NULL;
bool (WINAPI *DXL_GetPresentVelocities) (TDeviceID dvid, const uint8_t *ids, double *velocities, int num) = NULL;
bool (WINAPI *DXL_SetGoalAngleAndVelocity) (TDeviceID dvid, uint8_t id, double angle, double velocity) = NULL;
bool (WINAPI *DXL_SetGoalAnglesAndVelocities) (TDeviceID dvid, const uint8_t *ids, PAngleVelocity anglevelocity, int num) = NULL;
bool (WINAPI *DXL_SetGoalAngleAndTime) (TDeviceID dvid, uint8_t id, double angle, double sec) = NULL;
bool (WINAPI *DXL_SetGoalAnglesAndTime) (TDeviceID dvid, const uint8_t *ids, const double *angles, int num, double sec) = NULL;
bool (WINAPI *DXL_SetGoalAngleAndTime2) (TDeviceID dvid, uint8_t id, double angle, double sec) = NULL;
bool (WINAPI *DXL_SetGoalAnglesAndTime2) (TDeviceID dvid, const uint8_t *ids, const double *angles, int num, double sec) = NULL;
bool (WINAPI *DXL_SetGoalCurrent) (TDeviceID dvid, uint8_t id, double current) = NULL;
bool (WINAPI *DXL_SetGoalCurrents) (TDeviceID dvid, const uint8_t *ids, const double *currents, int num) = NULL;
bool (WINAPI *DXL_GetPresentCurrent) (TDeviceID dvid, uint8_t id, double *current) = NULL;
bool (WINAPI *DXL_GetPresentCurrents) (TDeviceID dvid, const uint8_t *ids, double *currents, int num) = NULL;
bool (WINAPI *DXL_SetGoalPWM) (TDeviceID dvid, uint8_t id, double pwm) = NULL;
bool (WINAPI *DXL_SetGoalPWMs) (TDeviceID dvid, const uint8_t *ids, const double *pwms, int num) = NULL;
bool (WINAPI *DXL_GetPresentPWM) (TDeviceID dvid, uint8_t id, double *pwm) = NULL;
bool (WINAPI *DXL_GetPresentPWMs) (TDeviceID dvid, const uint8_t *ids, double *pwms, int num) = NULL;
bool (WINAPI *DXL_SetDriveMode) (TDeviceID dvid, uint8_t id, uint8_t mode) = NULL;
bool (WINAPI *DXL_SetDriveModesEquival) (TDeviceID dvid, const uint8_t *ids, int num, uint8_t mode) = NULL;
bool (WINAPI *DXL_SetOperatingMode) (TDeviceID dvid, uint8_t id, uint8_t mode) = NULL;
bool (WINAPI *DXL_SetOperatingModesEquival) (TDeviceID dvid, const uint8_t *ids, int num, uint8_t mode) = NULL;
bool (WINAPI *DXL_GetOperatingMode) (TDeviceID dvid, uint8_t id, uint8_t *mode) = NULL;
bool (WINAPI *DXL_GetHWErrorCode) (TDeviceID dvid, uint8_t id, uint8_t *hwerr) = NULL;
TErrorCode (WINAPI *DXL_GetErrorCode) (TDeviceID dvid, uint8_t id) = NULL;
PDXL_ModelInfo (WINAPI *DXL_GetModelInfo) (TDeviceID dvid, uint8_t id) = NULL;
int (WINAPI *DXL_ScanDevices) (TDeviceID dvid, uint8_t *ids) = NULL;
bool (WINAPI *DXL_PrintDevicesList) (int (*pf) (const char *, ...)) = NULL;
void (WINAPI *DXL_InitDevicesList) (void) = NULL;

static HMODULE hModule = NULL;

#ifdef _WIN32
#ifdef _WIN64
#ifdef _MSC_VER
#define DLLNAME   L"dxlib_x64.dll"
#else
#define DLLNAME   "dxlib_x64.dll"
#endif
#else
#ifdef _MSC_VER
#define DLLNAME   L"dxlib_x32.dll"
#else
#define DLLNAME   "dxlib_x32.dll"
#endif
#endif
#else
#ifdef __x86_64__
#define DLLNAME   "dxlib_x64.dll"
#else
#define DLLNAME   "dxlib_x32.dll"
#endif
#endif

// Load the DLL
bool LoadDLL (void) {
  hModule = LoadLibrary (DLLNAME);
  if (hModule == NULL) {
    return false;
  }

  * (FARPROC *)&DX_OpenPort                = GetProcAddress (hModule, "DX_OpenPort");
  * (FARPROC *)&DX_ClosePort               = GetProcAddress (hModule, "DX_ClosePort");
  * (FARPROC *)&DX_SetBaudrate             = GetProcAddress (hModule, "DX_SetBaudrate");
  * (FARPROC *)&DX_Active                  = GetProcAddress (hModule, "DX_Active");
  * (FARPROC *)&DX_SetTimeOutOffset        = GetProcAddress (hModule, "DX_SetTimeOutOffset");
  * (FARPROC *)&GetQueryPerformanceCounter = GetProcAddress (hModule, "GetQueryPerformanceCounter");
  * (FARPROC *)&DX_TxPacket                = GetProcAddress (hModule, "DX_TxPacket");
  * (FARPROC *)&DX_RxPacket                = GetProcAddress (hModule, "DX_RxPacket");
  * (FARPROC *)&DX_ReadByteData            = GetProcAddress (hModule, "DX_ReadByteData");
  * (FARPROC *)&DX_WriteByteData           = GetProcAddress (hModule, "DX_WriteByteData");
  * (FARPROC *)&DX_ReadWordData            = GetProcAddress (hModule, "DX_ReadWordData");
  * (FARPROC *)&DX_WriteWordData           = GetProcAddress (hModule, "DX_WriteWordData");
  * (FARPROC *)&DX_ReadLongData            = GetProcAddress (hModule, "DX_ReadLongData");
  * (FARPROC *)&DX_WriteLongData           = GetProcAddress (hModule, "DX_WriteLongData");
  * (FARPROC *)&DX_ReadBlockData           = GetProcAddress (hModule, "DX_ReadBlockData");
  * (FARPROC *)&DX_WriteBlockData          = GetProcAddress (hModule, "DX_WriteBlockData");
  * (FARPROC *)&DX_ReadSyncData            = GetProcAddress (hModule, "DX_ReadSyncData");
  * (FARPROC *)&DX_WriteSyncData           = GetProcAddress (hModule, "DX_WriteSyncData");
  * (FARPROC *)&DX_Ping                    = GetProcAddress (hModule, "DX_Ping");
  * (FARPROC *)&DX_Ping2                   = GetProcAddress (hModule, "DX_Ping2");
  * (FARPROC *)&DX_Reset                   = GetProcAddress (hModule, "DX_Reset");
  * (FARPROC *)&DX_Reboot                  = GetProcAddress (hModule, "DX_Reboot");

  * (FARPROC *)&DXL_SetTorqueEnable           = GetProcAddress (hModule, "DXL_SetTorqueEnable");
  * (FARPROC *)&DXL_SetTorqueEnables          = GetProcAddress (hModule, "DXL_SetTorqueEnables");
  * (FARPROC *)&DXL_SetTorqueEnablesEquival   = GetProcAddress (hModule, "DXL_SetTorqueEnablesEquival");
  * (FARPROC *)&DXL_GetTorqueEnable           = GetProcAddress (hModule, "DXL_GetTorqueEnable");
  * (FARPROC *)&DXL_GetTorqueEnables          = GetProcAddress (hModule, "DXL_GetTorqueEnables");
  * (FARPROC *)&DXL_SetLED                    = GetProcAddress (hModule, "DXL_SetLED");
  * (FARPROC *)&DXL_SetGoalAngle              = GetProcAddress (hModule, "DXL_SetGoalAngle");
  * (FARPROC *)&DXL_SetGoalAngles             = GetProcAddress (hModule, "DXL_SetGoalAngles");
  * (FARPROC *)&DXL_GetPresentAngle           = GetProcAddress (hModule, "DXL_GetPresentAngle");
  * (FARPROC *)&DXL_GetPresentAngles          = GetProcAddress (hModule, "DXL_GetPresentAngles");
  * (FARPROC *)&DXL_StandStillAngle           = GetProcAddress (hModule, "DXL_StandStillAngle");
  * (FARPROC *)&DXL_StandStillAngles          = GetProcAddress (hModule, "DXL_StandStillAngles");
  * (FARPROC *)&DXL_SetGoalVelocity           = GetProcAddress (hModule, "DXL_SetGoalVelocity");
  * (FARPROC *)&DXL_SetGoalVelocities         = GetProcAddress (hModule, "DXL_SetGoalVelocities");
  * (FARPROC *)&DXL_GetPresentVelocity        = GetProcAddress (hModule, "DXL_GetPresentVelocity");
  * (FARPROC *)&DXL_GetPresentVelocities      = GetProcAddress (hModule, "DXL_GetPresentVelocities");
  * (FARPROC *)&DXL_SetGoalAngleAndVelocity   = GetProcAddress (hModule, "DXL_SetGoalAngleAndVelocity");
  * (FARPROC *)&DXL_SetGoalAnglesAndVelocities = GetProcAddress (hModule, "DXL_SetGoalAnglesAndVelocities");
  * (FARPROC *)&DXL_SetGoalAngleAndTime       = GetProcAddress (hModule, "DXL_SetGoalAngleAndTime");
  * (FARPROC *)&DXL_SetGoalAnglesAndTime      = GetProcAddress (hModule, "DXL_SetGoalAnglesAndTime");
  * (FARPROC *)&DXL_SetGoalAngleAndTime2      = GetProcAddress (hModule, "DXL_SetGoalAngleAndTime2");
  * (FARPROC *)&DXL_SetGoalAnglesAndTime2     = GetProcAddress (hModule, "DXL_SetGoalAnglesAndTime2");
  * (FARPROC *)&DXL_SetGoalPWM                = GetProcAddress (hModule, "DXL_SetGoalPWM");
  * (FARPROC *)&DXL_SetGoalPWMs               = GetProcAddress (hModule, "DXL_SetGoalPWMs");
  * (FARPROC *)&DXL_GetPresentPWM             = GetProcAddress (hModule, "DXL_GetPresentPWM");
  * (FARPROC *)&DXL_GetPresentPWMs            = GetProcAddress (hModule, "DXL_GetPresentPWMs");
  * (FARPROC *)&DXL_SetGoalCurrent            = GetProcAddress (hModule, "DXL_SetGoalCurrent");
  * (FARPROC *)&DXL_SetGoalCurrents           = GetProcAddress (hModule, "DXL_SetGoalCurrents");
  * (FARPROC *)&DXL_GetPresentCurrent         = GetProcAddress (hModule, "DXL_GetPresentCurrent");
  * (FARPROC *)&DXL_GetPresentCurrents        = GetProcAddress (hModule, "DXL_GetPresentCurrents");
  * (FARPROC *)&DXL_SetDriveMode              = GetProcAddress (hModule, "DXL_SetDriveMode");
  * (FARPROC *)&DXL_SetDriveModesEquival      = GetProcAddress (hModule, "DXL_SetDriveModesEquival");
  * (FARPROC *)&DXL_SetOperatingMode          = GetProcAddress (hModule, "DXL_SetOperatingMode");
  * (FARPROC *)&DXL_SetOperatingModesEquival  = GetProcAddress (hModule, "DXL_SetOperatingModesEquival");
  * (FARPROC *)&DXL_GetOperatingMode          = GetProcAddress (hModule, "DXL_GetOperatingMode");
  * (FARPROC *)&DXL_GetHWErrorCode            = GetProcAddress (hModule, "DXL_GetHWErrorCode");
  * (FARPROC *)&DXL_GetErrorCode              = GetProcAddress (hModule, "DXL_GetErrorCode");
  * (FARPROC *)&DXL_GetModelInfo              = GetProcAddress (hModule, "DXL_GetModelInfo");
  * (FARPROC *)&DXL_ScanDevices               = GetProcAddress (hModule, "DXL_ScanDevices");
  * (FARPROC *)&DXL_PrintDevicesList          = GetProcAddress (hModule, "DXL_PrintDevicesList");
  * (FARPROC *)&DXL_InitDevicesList           = GetProcAddress (hModule, "DXL_InitDevicesList");

  return
    (DX_OpenPort                != NULL) &&
    (DX_ClosePort               != NULL) &&
    (DX_SetBaudrate             != NULL) &&
    (DX_Active                  != NULL) &&
    (DX_SetTimeOutOffset        != NULL) &&
    (GetQueryPerformanceCounter != NULL) &&
    (DX_TxPacket                != NULL) &&
    (DX_RxPacket                != NULL) &&
    (DX_ReadByteData            != NULL) &&
    (DX_WriteByteData           != NULL) &&
    (DX_ReadWordData            != NULL) &&
    (DX_WriteWordData           != NULL) &&
    (DX_ReadLongData            != NULL) &&
    (DX_WriteLongData           != NULL) &&
    (DX_ReadBlockData           != NULL) &&
    (DX_WriteBlockData          != NULL) &&
    (DX_ReadSyncData            != NULL) &&
    (DX_WriteSyncData           != NULL) &&
    (DX_Ping                    != NULL) &&
    (DX_Ping2                   != NULL) &&
    (DX_Reset                   != NULL) &&
    (DX_Reboot                  != NULL) &&

    (DXL_SetTorqueEnable           != NULL) &&
    (DXL_SetTorqueEnables          != NULL) &&
    (DXL_SetTorqueEnablesEquival   != NULL) &&
    (DXL_GetTorqueEnable           != NULL) &&
    (DXL_GetTorqueEnables          != NULL) &&
    (DXL_SetLED                    != NULL) &&
    (DXL_SetGoalAngle              != NULL) &&
    (DXL_SetGoalAngles             != NULL) &&
    (DXL_GetPresentAngle           != NULL) &&
    (DXL_GetPresentAngles          != NULL) &&
    (DXL_StandStillAngle           != NULL) &&
    (DXL_StandStillAngles          != NULL) &&
    (DXL_SetGoalVelocity           != NULL) &&
    (DXL_SetGoalVelocities         != NULL) &&
    (DXL_GetPresentVelocity        != NULL) &&
    (DXL_GetPresentVelocities      != NULL) &&
    (DXL_SetGoalAngleAndVelocity   != NULL) &&
    (DXL_SetGoalAnglesAndVelocities != NULL) &&
    (DXL_SetGoalAngleAndTime       != NULL) &&
    (DXL_SetGoalAnglesAndTime      != NULL) &&
    (DXL_SetGoalAngleAndTime2      != NULL) &&
    (DXL_SetGoalAnglesAndTime2     != NULL) &&
    (DXL_SetGoalCurrent            != NULL) &&
    (DXL_SetGoalCurrents           != NULL) &&
    (DXL_GetPresentCurrent         != NULL) &&
    (DXL_GetPresentCurrents        != NULL) &&
    (DXL_SetGoalPWM                != NULL) &&
    (DXL_SetGoalPWMs               != NULL) &&
    (DXL_GetPresentPWM             != NULL) &&
    (DXL_GetPresentPWMs            != NULL) &&
    (DXL_SetDriveMode              != NULL) &&
    (DXL_SetDriveModesEquival      != NULL) &&
    (DXL_SetOperatingMode          != NULL) &&
    (DXL_SetOperatingModesEquival  != NULL) &&
    (DXL_GetOperatingMode          != NULL) &&
    (DXL_GetHWErrorCode            != NULL) &&
    (DXL_GetErrorCode              != NULL) &&
    (DXL_GetModelInfo              != NULL) &&
    (DXL_ScanDevices               != NULL) &&
    (DXL_PrintDevicesList          != NULL) &&
    (DXL_InitDevicesList           != NULL);
}

// Unload the DLL
void UnloadDLL (void) {
  if (hModule != NULL) {
    FreeLibrary (hModule);
    DX_OpenPort                = NULL;
    DX_ClosePort               = NULL;
    DX_SetBaudrate             = NULL;
    DX_Active                  = NULL;
    DX_SetTimeOutOffset        = NULL;
    GetQueryPerformanceCounter = NULL;
    DX_TxPacket                = NULL;
    DX_RxPacket                = NULL;
    DX_ReadByteData            = NULL;
    DX_WriteByteData           = NULL;
    DX_ReadWordData            = NULL;
    DX_WriteWordData           = NULL;
    DX_ReadLongData            = NULL;
    DX_WriteLongData           = NULL;
    DX_ReadBlockData           = NULL;
    DX_WriteBlockData          = NULL;
    DX_ReadSyncData            = NULL;
    DX_WriteSyncData           = NULL;
    DX_Ping                    = NULL;
    DX_Ping2                   = NULL;
    DX_Reset                   = NULL;
    DX_Reboot                  = NULL;

    DXL_SetTorqueEnable           = NULL;
    DXL_SetTorqueEnables          = NULL;
    DXL_SetTorqueEnablesEquival   = NULL;
    DXL_GetTorqueEnable           = NULL;
    DXL_GetTorqueEnables          = NULL;
    DXL_SetLED                    = NULL;
    DXL_SetGoalAngle              = NULL;
    DXL_SetGoalAngles             = NULL;
    DXL_GetPresentAngle           = NULL;
    DXL_GetPresentAngles          = NULL;
    DXL_StandStillAngle           = NULL;
    DXL_StandStillAngles          = NULL;
    DXL_SetGoalVelocity           = NULL;
    DXL_SetGoalVelocities         = NULL;
    DXL_GetPresentVelocity        = NULL;
    DXL_GetPresentVelocities      = NULL;
    DXL_SetGoalAngleAndVelocity   = NULL;
    DXL_SetGoalAnglesAndVelocities = NULL;
    DXL_SetGoalAngleAndTime       = NULL;
    DXL_SetGoalAnglesAndTime      = NULL;
    DXL_SetGoalAngleAndTime2      = NULL;
    DXL_SetGoalAnglesAndTime2     = NULL;
    DXL_SetGoalCurrent            = NULL;
    DXL_SetGoalCurrents           = NULL;
    DXL_GetPresentCurrent         = NULL;
    DXL_GetPresentCurrents        = NULL;
    DXL_SetGoalPWM                = NULL;
    DXL_SetGoalPWMs               = NULL;
    DXL_GetPresentPWM             = NULL;
    DXL_GetPresentPWMs            = NULL;
    DXL_SetDriveMode              = NULL;
    DXL_SetDriveModesEquival      = NULL;
    DXL_SetOperatingMode          = NULL;
    DXL_SetOperatingModesEquival  = NULL;
    DXL_GetOperatingMode          = NULL;
    DXL_GetHWErrorCode            = NULL;
    DXL_GetErrorCode              = NULL;
    DXL_GetModelInfo              = NULL;
    DXL_ScanDevices               = NULL;
    DXL_PrintDevicesList          = NULL;
    DXL_InitDevicesList           = NULL;
  }
}
#else
#error Not support dynamic load!!
#endif

#endif

#ifdef __cplusplus
}
#endif

#endif //_DXLIB_H_INCLUDE
