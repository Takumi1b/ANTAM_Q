#include <windows.h>

#define BROADCASTING_ID     (0xfe)
#define DXMAXLENGTH         (143)

typedef signed char int8_t;
typedef unsigned char   uint8_t;
typedef short  int16_t;
typedef unsigned short  uint16_t;
typedef int  int32_t;
typedef unsigned   uint32_t;
typedef long long  int64_t;
typedef unsigned long long   uint64_t;

// デバイスID
#ifdef _WIN64
typedef uint64_t TDeviceID;
#else
typedef uint32_t TDeviceID;
#endif

// インストラクション
typedef enum {INST_PING = 1, INST_READ = 2, INST_WRITE = 3, INST_REG_WRITE = 4, INST_ACTION = 5, INST_RESET = 6, INST_SYNC_WRITE = 0x83, INST_SYNG_REG_WRITE = 0x85} TInstruction;

// エラーフラグ
typedef uint16_t TErrorCode;
#define ERR_INVALID_DEVID   (1 << 15)
#define ERR_INVALID_ID      (1 << 14)
#define ERR_DIFF_ID         (1 << 13)
#define ERR_ILLEGAL_SIZE    (1 << 12)
#define ERR_INVALID_PARAM   (1 << 11)
#define ERR_COMM            (1 << 10)
#define ERR_CHECKSUM        (1 << 9)
#define ERR_TIMEOUT         (1 << 8)
#define ERR_DX_INST         (1 << 6)
#define ERR_DX_OVERLOAD     (1 << 5)
#define ERR_DX_CHECKSUM     (1 << 4)
#define ERR_DX_RANGE        (1 << 3)
#define ERR_DX_OVERHEAT     (1 << 2)
#define ERR_DX_ANGLE        (1 << 1)
#define ERR_DX_OVERVOLTAGE  (1 << 0)
#define ERR_NON             (0)

// アラームステータス
typedef struct {
  uint8_t       id;
  TErrorCode  Status;
} TDxAlarmStatus;


TDeviceID __declspec (dllimport) WINAPI DX_OpenPort (char *name, uint32_t baud);
bool __declspec (dllimport) WINAPI DX_ClosePort (TDeviceID dvid);
bool __declspec (dllimport) WINAPI DX_SetBaudrate (TDeviceID dvid, uint32_t baud);
bool __declspec (dllimport) WINAPI DX_Active (TDeviceID dvid);
void __declspec (dllimport) WINAPI DX_SetTimeOutOffset (TDeviceID dvid, uint32_t t);
double __declspec (dllimport) WINAPI GetQueryPerformanceCounter (void);

bool __declspec (dllimport) WINAPI DX_TxPacket (TDeviceID dvid, uint8_t id, TInstruction inst, uint8_t *param, uint32_t len, TErrorCode *err);
bool __declspec (dllimport) WINAPI DX_RxPacket (TDeviceID dvid, uint8_t *rdata, uint32_t rdatasize, uint32_t *rlen, uint32_t timeout, TErrorCode *err);
bool __declspec (dllimport) WINAPI DX_ReadByteData (TDeviceID dvid, uint8_t id, uint16_t adr, uint8_t *rdata, TErrorCode *err);
bool __declspec (dllimport) WINAPI DX_WriteByteData (TDeviceID dvid, uint8_t id, uint16_t adr, uint8_t dat, TErrorCode *err);

bool __declspec (dllimport) WINAPI DX_ReadWordData (TDeviceID dvid, uint8_t id, uint16_t adr, uint16_t *rdata, TErrorCode *err);
bool __declspec (dllimport) WINAPI DX_WriteWordData (TDeviceID dvid, uint8_t id, uint16_t adr, uint16_t dat, TErrorCode *err);

bool __declspec (dllimport) WINAPI DX_ReadBlockData (TDeviceID dvid, uint8_t id, uint16_t adr, uint8_t *rdata, uint32_t len, TErrorCode *err);
bool __declspec (dllimport) WINAPI DX_WriteBlockData (TDeviceID dvid, uint8_t id, uint16_t adr, uint8_t *dat, uint32_t len, TErrorCode *err);

bool __declspec (dllimport) WINAPI DX_WriteSyncData (TDeviceID dvid, uint8_t *dat, uint32_t size, TErrorCode *err);

bool __declspec (dllimport) WINAPI DX_Ping (TDeviceID dvid, uint8_t id, TErrorCode *err);
bool __declspec (dllimport) WINAPI DX_Ping2 (TDeviceID dvid, uint32_t *num, TDxAlarmStatus *AlarmStatus, TErrorCode *err);


bool __declspec (dllimport) WINAPI DX_Reset (TDeviceID dvid, uint8_t id, TErrorCode *err);
bool __declspec (dllimport) WINAPI DX_Reboot (TDeviceID dvid, uint8_t id, TErrorCode *err);
