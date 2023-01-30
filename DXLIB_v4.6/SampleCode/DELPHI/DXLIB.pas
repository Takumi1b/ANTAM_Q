unit DXLIB;

interface

uses
  Windows, SysUtils;

type
  puint16 = ^uint16;
  puint32 = ^uint32;
  puint64 = ^uint64;

{$IFDEF WIN64}
  TDeviceID   = UInt64;
{$ELSE}
  TDeviceID   = UInt32;
{$ENDIF}

  TErrorCode  = uint16;
  TInstruction  = uint8;

  // アラームステータス
  TAlarmStatus = packed Record
    id      :uint8;
    Status  :TErrorCode;
  end;
  PAlarmStatus = ^TAlarmStatus;

  TSyncReadParam = packed Record
    addr:   uint8;
    length: uint8;
    ids:    array[0..255] of uint8;
  end;
  PSyncReadParam = ^TSyncReadParam;

  TDXL_DevType = (devtNONE = 0, devtDX = 1, devtAX = 2, devtRX = 3, devtEX = 4, devtMX = 5, devtXL320 = 6, devtPRO = 7, devtPROP = 8, devtX = 9);

  tmaxmin_int32 = packed record
    max:int32;
    min:int32;
  end;
  tmaxmin_real = packed record
    max:real;
    min:real;
  end;
  // 全シリーズモデル情報
  TDXL_ModelInfo = packed Record
    modelno:uint16;               // デバイス固有のモデル番号
    name:array[0..15] of ansichar;
    devtype:      uint32;         // デバイスのグループ
    positionlimit:tmaxmin_int32;  // 位置の範囲
    anglelimit:   tmaxmin_real;   // 角度の範囲
    velocitylimit:tmaxmin_int32;  // 速度の範囲
    pwmlimit:     tmaxmin_int32;  // PWMの範囲
    velocityratio :real;          // 角速度変換係数 [deg/sec]
    currentratio  :real;          // 電流変換係数 [mA]
    pwmratio      :real;          // PWM変換係数 [%]
  end;
  PDXL_ModelInfo = ^TDXL_ModelInfo;

  TAngleVelocity = record
    angle:double;
    velocity:double;
  end;
  PAngleVelocity = ^TAngleVelocity;

  TDX_OpenPort = function (pcom:pansichar; br:UInt32):TDeviceID; stdcall;
  TDX_ClosePort = function (dvid:TDeviceID):BOOL; stdcall;
  TDX_SetBaudrate = function (dvid:TDeviceID; br:UInt32):BOOL; stdcall;
  TDX_Active = function (dvid:TDeviceID):BOOL; stdcall;
  TDX_SetTimeOutOffset = procedure (dvid:TDeviceID; offsettime:UInt32); stdcall;
  TGetQueryPerformanceCounter = function:double; stdcall;
  TDX_TxPacket = function (dvid:TDeviceID; id:uint8; cInst:TInstruction; var pParam:uint8; iLength:integer; var errcode:TErrorCode):integer; stdcall;
  TDX_RxPacket = function (dvid:TDeviceID; var pRcv:uint8; sz:integer; var pLength:integer; timeout:DWORD; var errcode:TErrorCode):integer; stdcall;
  TDX_ReadByteData = function (dvid:TDeviceID; id:uint8; adr:uint16; var result:uint8; var errcode:TErrorCode):BOOL; stdcall;
  TDX_WriteByteData = function (dvid:TDeviceID; id:uint8; adr:uint16; dat:uint8; var errcode:TErrorCode):BOOL; stdcall;
  TDX_ReadWordData = function (dvid:TDeviceID; id:uint8; adr:uint16; var result:uint16; var errcode:TErrorCode):BOOL; stdcall;
  TDX_WriteWordData = function (dvid:TDeviceID; id:uint8; adr:uint16; dat:uint16; var errcode:TErrorCode):BOOL; stdcall;
  TDX_ReadLongData = function (dvid:TDeviceID; id:uint8; adr:uint16; var result:uint32; var errcode:TErrorCode):BOOL; stdcall;
  TDX_WriteLongData = function (dvid:TDeviceID; id:uint8; adr:uint16; dat:uint32; var errcode:TErrorCode):BOOL; stdcall;
  TDX_ReadBlockData = function (dvid:TDeviceID; id:uint8; adr:uint16; var result:uint8; len:integer; var errcode:TErrorCode):BOOL; stdcall;
  TDX_WriteBlockData = function (dvid:TDeviceID; id:uint8; adr:uint16; var dat:uint8; len:integer; var errcode:TErrorCode):BOOL; stdcall;
  TDX_Ping = function (dvid:TDeviceID; id:uint8; var errcode:TErrorCode):BOOL; stdcall;
  TDX_Ping2 = function (dvid:TDeviceID; var num:UInt32; AlarmStatus:PAlarmStatus; var errcode:TErrorCode):BOOL; stdcall;
  TDX_ReadSyncData = function (dvid:TDeviceID; param:PSyncReadParam; var num:uint32; var dat:uint8; var errcode:TErrorCode):BOOL; stdcall;
  TDX_WriteSyncData = function (dvid:TDeviceID; var dat:uint8; size:uint32; var errcode:TErrorCode):BOOL; stdcall;
  TDX_Reset = function (dvid:TDeviceID; id:uint8; var errcode:TErrorCode):BOOL; stdcall;

  TDXL_SetLED = function (dvid:TDeviceID; id:uint8; en:bool):bool; stdcall;
  TDXL_SetTorqueEnable = function (dvid:TDeviceID; id:uint8; en:bool):bool; stdcall;
  TDXL_SetTorqueEnables = function (dvid:TDeviceID; var ids:uint8; var ens:bool; num:integer):bool; stdcall;
  TDXL_SetTorqueEnablesEquival = function (dvid:TDeviceID; var ids:uint8; num:integer; en:bool):bool; stdcall;
  TDXL_GetTorqueEnable = function (dvid:TDeviceID; id:uint8; var en:bool):bool; stdcall;
  TDXL_GetTorqueEnables = function (dvid:TDeviceID; var ids:uint8; var en:bool; num:integer):bool; stdcall;

  TDXL_SetGoalAngle = function (dvid:TDeviceID; id:uint8; angle:double):bool; stdcall;
  TDXL_SetGoalAngles = function (dvid:TDeviceID; var ids:uint8; var pangles:double; num:integer):bool; stdcall;
  TDXL_GetPresentAngle = function (dvid:TDeviceID; id:uint8; var pangle:double):bool; stdcall;
  TDXL_GetPresentAngles = function (dvid:TDeviceID; var ids:uint8; var pangles:double; num:integer):bool; stdcall;

  TDXL_StandStillAngle = function (dvid:TDeviceID; id:uint8):bool; stdcall;
  TDXL_StandStillAngles = function (dvid:TDeviceID; var ids:uint8; num:integer):bool; stdcall;

  TDXL_SetGoalVelocity = function (dvid:TDeviceID; id:uint8; velocity:double):bool; stdcall;
  TDXL_SetGoalVelocities = function (dvid:TDeviceID; var ids:uint8; var pvelocities:double; num:integer):bool; stdcall;
  TDXL_GetPresentVelocity = function (dvid:TDeviceID; id:uint8; var pvelocity:double):bool; stdcall;
  TDXL_GetPresentVelocities = function (dvid:TDeviceID; var ids:uint8; var pvelocities:double; num:integer):bool; stdcall;


  TDXL_SetGoalAngleAndVelocity = function (dvid:TDeviceID; id:uint8; angle:double; velocity:double):bool; stdcall;
  TDXL_SetGoalAnglesAndVelocities = function (dvid:TDeviceID; var ids:uint8; panglevelocity:PAngleVelocity; num:integer):bool; stdcall;
  TDXL_SetGoalAngleAndTime = function (dvid:TDeviceID; id:uint8; angle:double; sec:double):bool; stdcall;
  TDXL_SetGoalAnglesAndTime = function (dvid:TDeviceID; var ids:uint8; var pangles:double; num:integer; sec:double):bool; stdcall;
  TDXL_SetGoalAngleAndTime2 = function (dvid:TDeviceID; id:uint8; angle:double; sec:double):bool; stdcall;
  TDXL_SetGoalAnglesAndTime2 = function (dvid:TDeviceID; var ids:uint8; var pangles:double; num:integer; sec:double):bool; stdcall;

  TDXL_SetGoalCurrent = function (dvid:TDeviceID; id:uint8; current:double):bool; stdcall;
  TDXL_SetGoalCurrents = function (dvid:TDeviceID; var ids:uint8; var pcurrents:double; num:integer):bool; stdcall;
  TDXL_GetPresentCurrent = function (dvid:TDeviceID; id:uint8; var pcurrent:double):bool; stdcall;
  TDXL_GetPresentCurrents = function (dvid:TDeviceID; var ids:uint8; var pcurrents:double; num:integer):bool; stdcall;

  TDXL_SetGoalPWM = function (dvid:TDeviceID; id:uint8; pwm:double):bool; stdcall;
  TDXL_SetGoalPWMs = function (dvid:TDeviceID; var ids:uint8; var ppwms:double; num:integer):bool; stdcall;
  TDXL_GetPresentPWM = function (dvid:TDeviceID; id:uint8; var ppwm:double):bool; stdcall;
  TDXL_GetPresentPWMs = function (dvid:TDeviceID; var ids:uint8; var ppwms:double; num:integer):bool; stdcall;

  TDXL_SetDriveMode = function (dvid:TDeviceID; id:uint8; mode:uint8):bool; stdcall;
  TDXL_SetDriveModesEquival = function (dvid:TDeviceID; var ids:uint8; num:integer; mode:uint8):bool; stdcall;
  TDXL_SetOperatingMode = function (dvid:TDeviceID; id:uint8; mode:uint8):bool; stdcall;
  TDXL_SetOperatingModesEquival = function (dvid:TDeviceID; var ids:uint8; num:integer; mode:uint8):bool; stdcall;
  TDXL_GetOperatingMode = function (dvid:TDeviceID; id:uint8; var pmode:uint8):bool; stdcall;

  TDXL_GetHWErrorCode = function (dvid:TDeviceID; id:uint8; var phwerr:uint8):bool; stdcall;
  TDXL_GetErrorCode = function (dvid:TDeviceID; id:uint8):TErrorCode; stdcall;
  TDXL_GetModelInfo = function (dvid:TDeviceID; id:uint8):PDXL_ModelInfo; stdcall;
  TDXL_ScanDevices = function (dvid:TDeviceID; var pids:uint8):integer; stdcall;
//  TDXL_PrintDevicesList = function (int (*pf) (const char *, ...)):bool; stdcall;
  TDXL_InitDevicesList = procedure; stdcall;

  function DXLIB_Load:boolean;
  procedure DXLIB_Unload;
  function DXLIB_Active:boolean;

const
  INST_PING           = $01;
  INST_READ           = $02;
  INST_WRITE          = $03;
  INST_REG_WRITE      = $04;
  INST_ACTION         = $05;
  INST_RESET          = $06;
  INST_SYNC_WRITE     = $83;
  INST_SYNG_REG_WRITE = $85;

  ERR_INVALID_DEVID   = $8000;
  ERR_INVALID_ID      = $4000;
  ERR_DIFF_ID         = $2000;
  ERR_ILLEGAL_SIZE    = $1000;
  ERR_INVALID_PARAM   = $0800;
  ERR_COMM            = $0400;
  ERR_CHECKSUM        = $0200;
  ERR_TIMEOUT         = $0100;
  ERR_DX_INST         = $0040;
  ERR_DX_OVERLOAD     = $0020;
  ERR_DX_CHECKSUM     = $0010;
  ERR_DX_RANGE        = $0008;
  ERR_DX_OVERHEAT     = $0004;
  ERR_DX_ANGLE        = $0002;
  ERR_DX_OVERVOLTAGE  = $0001;
  ERR_NON             = $0000;
  FATAL_ERR           = (ERR_INVALID_DEVID or ERR_INVALID_ID or ERR_DIFF_ID or ERR_ILLEGAL_SIZE or ERR_INVALID_PARAM or ERR_COMM or ERR_CHECKSUM or ERR_TIMEOUT or ERR_DX_INST or ERR_DX_CHECKSUM);

var
  DX_OpenPort:TDX_OpenPort;
  DX_ClosePort:TDX_ClosePort;
  DX_SetBaudrate:TDX_SetBaudrate;
  DX_Active:TDX_Active;
  DX_SetTimeOutOffset:TDX_SetTimeOutOffset;
  GetQueryPerformanceCounter:TGetQueryPerformanceCounter;
  DX_TxPacket:TDX_TxPacket;
  DX_RxPacket:TDX_RxPacket;
  DX_ReadByteData:TDX_ReadByteData;
  DX_WriteByteData:TDX_WriteByteData;
  DX_ReadWordData:TDX_ReadWordData;
  DX_WriteWordData:TDX_WriteWordData;
  DX_ReadLongData:TDX_ReadLongData;
  DX_WriteLongData:TDX_WriteLongData;
  DX_ReadBlockData:TDX_ReadBlockData;
  DX_WriteBlockData:TDX_WriteBlockData;
  DX_ReadSyncData:TDX_ReadSyncData;
  DX_WriteSyncData:TDX_WriteSyncData;
  DX_Ping:TDX_Ping;
  DX_Ping2:TDX_Ping2;
  DX_Reset:TDX_Reset;

  DXL_SetTorqueEnable:TDXL_SetTorqueEnable;
  DXL_SetTorqueEnables:TDXL_SetTorqueEnables;
  DXL_SetTorqueEnablesEquival:TDXL_SetTorqueEnablesEquival;
  DXL_GetTorqueEnable:TDXL_GetTorqueEnable;
  DXL_GetTorqueEnables:TDXL_GetTorqueEnables;
  DXL_SetLED:TDXL_SetLED;
  DXL_SetGoalAngle:TDXL_SetGoalAngle;
  DXL_SetGoalAngles:TDXL_SetGoalAngles;
  DXL_GetPresentAngle:TDXL_GetPresentAngle;
  DXL_GetPresentAngles:TDXL_GetPresentAngles;
  DXL_StandStillAngle:TDXL_StandStillAngle;
  DXL_StandStillAngles:TDXL_StandStillAngles;
  DXL_SetGoalVelocity:TDXL_SetGoalVelocity;
  DXL_SetGoalVelocities:TDXL_SetGoalVelocities;
  DXL_GetPresentVelocity:TDXL_GetPresentVelocity;
  DXL_GetPresentVelocities:TDXL_GetPresentVelocities;
  DXL_SetGoalAngleAndVelocity:TDXL_SetGoalAngleAndVelocity;
  DXL_SetGoalAnglesAndVelocities:TDXL_SetGoalAnglesAndVelocities;
  DXL_SetGoalAngleAndTime:TDXL_SetGoalAngleAndTime;
  DXL_SetGoalAnglesAndTime:TDXL_SetGoalAnglesAndTime;
  DXL_SetGoalAngleAndTime2:TDXL_SetGoalAngleAndTime2;
  DXL_SetGoalAnglesAndTime2:TDXL_SetGoalAnglesAndTime2;
  DXL_SetGoalPWM:TDXL_SetGoalPWM;
  DXL_SetGoalPWMs:TDXL_SetGoalPWMs;
  DXL_GetPresentPWM:TDXL_GetPresentPWM;
  DXL_GetPresentPWMs:TDXL_GetPresentPWMs;
  DXL_SetGoalCurrent:TDXL_SetGoalCurrent;
  DXL_SetGoalCurrents:TDXL_SetGoalCurrents;
  DXL_GetPresentCurrent:TDXL_GetPresentCurrent;
  DXL_GetPresentCurrents:TDXL_GetPresentCurrents;
  DXL_SetDriveMode:TDXL_SetDriveMode;
  DXL_SetDriveModesEquival:TDXL_SetDriveModesEquival;
  DXL_SetOperatingMode:TDXL_SetOperatingMode;
  DXL_SetOperatingModesEquival:TDXL_SetOperatingModesEquival;
  DXL_GetOperatingMode:TDXL_GetOperatingMode;
  DXL_GetHWErrorCode:TDXL_GetHWErrorCode;
  DXL_GetErrorCode:TDXL_GetErrorCode;
  DXL_GetModelInfo:TDXL_GetModelInfo;
  DXL_ScanDevices:TDXL_ScanDevices;
//  DXL_PrintDevicesList:TDXL_PrintDevicesList;
  DXL_InitDevicesList:TDXL_InitDevicesList;

implementation

var
  DLLHandle:THANDLE;

function conv_s11(v:puint16):integer;
begin
  if v^ and $400 <> 0 then result := -(v^ and $3ff) else result := (v^ and $3ff);
end;

function conv_sig(v:puint16):integer;
begin
  result := int16(v^);
end;

function conv_wd(v:puint16):integer;
begin
  result := uint16(v^);
end;

function DXLIB_Load:boolean;
var
  fn:string;
begin
{$IFDEF WIN64}
  DLLHandle := LoadLibrary('dxlib_x64.dll');
{$ELSE}
  DLLHandle := LoadLibrary('dxlib_x32.dll');
{$ENDIF}
  if DLLHandle <> 0 then
  begin
    fn := 'DX_OpenPort';
    @DX_OpenPort := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DX_ClosePort';
    @DX_ClosePort := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DX_SetBaudrate';
    @DX_SetBaudrate := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DX_Active';
    @DX_Active := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DX_SetTimeOutOffset';
    @DX_SetTimeOutOffset := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'GetQueryPerformanceCounter';
    @GetQueryPerformanceCounter := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DX_TxPacket';
    @DX_TxPacket := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DX_RxPacket';
    @DX_RxPacket := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DX_ReadByteData';
    @DX_ReadByteData := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DX_WriteByteData';
    @DX_WriteByteData := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DX_ReadWordData';
    @DX_ReadWordData := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DX_WriteWordData';
    @DX_WriteWordData := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DX_ReadLongData';
    @DX_ReadLongData := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DX_WriteLongData';
    @DX_WriteLongData := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DX_ReadBlockData';
    @DX_ReadBlockData := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DX_WriteBlockData';
    @DX_WriteBlockData := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DX_ReadSyncData';
    @DX_ReadSyncData := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DX_WriteSyncData';
    @DX_WriteSyncData := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DX_Ping';
    @DX_Ping := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DX_Ping2';
    @DX_Ping2 := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DX_Reset';
    @DX_Reset := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_SetTorqueEnable';
    @DXL_SetTorqueEnable := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_SetTorqueEnables';
    @DXL_SetTorqueEnables := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_SetTorqueEnablesEquival';
    @DXL_SetTorqueEnablesEquival := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_GetTorqueEnable';
    @DXL_GetTorqueEnable := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_GetTorqueEnables';
    @DXL_GetTorqueEnables := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_SetLED';
    @DXL_SetLED := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_SetGoalAngle';
    @DXL_SetGoalAngle := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_SetGoalAngles';
    @DXL_SetGoalAngles := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_GetPresentAngle';
    @DXL_GetPresentAngle := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_GetPresentAngles';
    @DXL_GetPresentAngles := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_StandStillAngle';
    @DXL_StandStillAngle := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_StandStillAngles';
    @DXL_StandStillAngles := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_SetGoalVelocity';
    @DXL_SetGoalVelocity := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_SetGoalVelocities';
    @DXL_SetGoalVelocities := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_GetPresentVelocity';
    @DXL_GetPresentVelocity := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_GetPresentVelocities';
    @DXL_GetPresentVelocities := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_SetGoalAngleAndVelocity';
    @DXL_SetGoalAngleAndVelocity := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_SetGoalAnglesAndVelocities';
    @DXL_SetGoalAnglesAndVelocities := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_SetGoalAngleAndTime';
    @DXL_SetGoalAngleAndTime := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_SetGoalAnglesAndTime';
    @DXL_SetGoalAnglesAndTime := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_SetGoalAngleAndTime2';
    @DXL_SetGoalAngleAndTime2 := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_SetGoalAnglesAndTime2';
    @DXL_SetGoalAnglesAndTime2 := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_SetGoalPWM';
    @DXL_SetGoalPWM := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_SetGoalPWMs';
    @DXL_SetGoalPWMs := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_GetPresentPWM';
    @DXL_GetPresentPWM := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_GetPresentPWMs';
    @DXL_GetPresentPWMs := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_SetGoalCurrent';
    @DXL_SetGoalCurrent := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_SetGoalCurrents';
    @DXL_SetGoalCurrents := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_GetPresentCurrent';
    @DXL_GetPresentCurrent := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_GetPresentCurrents';
    @DXL_GetPresentCurrents := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_SetDriveMode';
    @DXL_SetDriveMode := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_SetDriveModesEquival';
    @DXL_SetDriveModesEquival := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_SetOperatingMode';
    @DXL_SetOperatingMode := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_SetOperatingModesEquival';
    @DXL_SetOperatingModesEquival := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_GetOperatingMode';
    @DXL_GetOperatingMode := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_GetHWErrorCode';
    @DXL_GetHWErrorCode := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_GetErrorCode';
    @DXL_GetErrorCode := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_GetModelInfo';
    @DXL_GetModelInfo := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_ScanDevices';
    @DXL_ScanDevices := GetProcAddress(DLLHandle, PChar(fn));
    fn := 'DXL_PrintDevicesList';
//    @DXL_PrintDevicesList := GetProcAddress(DLLHandle, PChar(fn));
//    fn := 'DXL_InitDevicesList';
    @DXL_InitDevicesList := GetProcAddress(DLLHandle, PChar(fn));
  end;
  result :=
    (DLLHandle <> 0)
    and (@DX_OpenPort <> nil)
    and (@DX_ClosePort <> nil)
    and (@DX_SetBaudrate <> nil)
    and (@DX_Active <> nil)
    and (@DX_SetTimeOutOffset <> nil)
    and (@GetQueryPerformanceCounter <> nil)
    and (@DX_TxPacket <> nil)
    and (@DX_RxPacket <> nil)
    and (@DX_ReadByteData <> nil)
    and (@DX_WriteByteData <> nil)
    and (@DX_ReadWordData <> nil)
    and (@DX_WriteWordData <> nil)
    and (@DX_ReadLongData <> nil)
    and (@DX_WriteLongData <> nil)
    and (@DX_ReadBlockData <> nil)
    and (@DX_WriteBlockData <> nil)
    and (@DX_ReadSyncData <> nil)
    and (@DX_WriteSyncData <> nil)
    and (@DX_Ping <> nil)
    and (@DX_Ping2 <> nil)
    and (@DX_Reset <> nil)
    and (@DXL_SetTorqueEnable <> nil)
    and (@DXL_SetTorqueEnables <> nil)
    and (@DXL_SetTorqueEnablesEquival <> nil)
    and (@DXL_GetTorqueEnable <> nil)
    and (@DXL_GetTorqueEnables <> nil)
    and (@DXL_SetLED <> nil)
    and (@DXL_SetGoalAngle <> nil)
    and (@DXL_SetGoalAngles <> nil)
    and (@DXL_GetPresentAngle <> nil)
    and (@DXL_GetPresentAngles <> nil)
    and (@DXL_StandStillAngle <> nil)
    and (@DXL_StandStillAngles <> nil)
    and (@DXL_SetGoalVelocity <> nil)
    and (@DXL_SetGoalVelocities <> nil)
    and (@DXL_GetPresentVelocity <> nil)
    and (@DXL_GetPresentVelocities <> nil)
    and (@DXL_SetGoalAngleAndVelocity <> nil)
    and (@DXL_SetGoalAnglesAndVelocities <> nil)
    and (@DXL_SetGoalAngleAndTime <> nil)
    and (@DXL_SetGoalAnglesAndTime <> nil)
    and (@DXL_SetGoalAngleAndTime2 <> nil)
    and (@DXL_SetGoalAnglesAndTime2 <> nil)
    and (@DXL_SetGoalPWM <> nil)
    and (@DXL_SetGoalPWMs <> nil)
    and (@DXL_GetPresentPWM <> nil)
    and (@DXL_GetPresentPWMs <> nil)
    and (@DXL_SetGoalCurrent <> nil)
    and (@DXL_SetGoalCurrents <> nil)
    and (@DXL_GetPresentCurrent <> nil)
    and (@DXL_GetPresentCurrents <> nil)
    and (@DXL_SetDriveMode <> nil)
    and (@DXL_SetDriveModesEquival <> nil)
    and (@DXL_SetOperatingMode <> nil)
    and (@DXL_SetOperatingModesEquival <> nil)
    and (@DXL_GetOperatingMode <> nil)
    and (@DXL_GetHWErrorCode <> nil)
    and (@DXL_GetErrorCode <> nil)
    and (@DXL_GetModelInfo <> nil)
    and (@DXL_ScanDevices <> nil)
//    and (@DXL_PrintDevicesList <> nil)
    and (@DXL_InitDevicesList <> nil)
end;

procedure DXLIB_Unload;
begin
  if DXLIB_Active then
  begin
    FreeLibrary(DLLHandle);
    DX_OpenPort := nil;
    DX_ClosePort := nil;
    DX_SetBaudrate := nil;
    DX_Active := nil;
    DX_SetTimeOutOffset := nil;
    GetQueryPerformanceCounter := nil;
    DX_TxPacket := nil;
    DX_RxPacket := nil;
    DX_ReadByteData := nil;
    DX_WriteByteData := nil;
    DX_ReadWordData := nil;
    DX_WriteWordData := nil;
    DX_ReadLongData := nil;
    DX_WriteLongData := nil;
    DX_ReadBlockData := nil;
    DX_WriteBlockData := nil;
    DX_ReadSyncData := nil;
    DX_WriteSyncData := nil;
    DX_Ping := nil;
    DX_Ping2 := nil;
    DX_Reset := nil;
    DXL_SetTorqueEnable := nil;
    DXL_SetTorqueEnables := nil;
    DXL_SetTorqueEnablesEquival := nil;
    DXL_GetTorqueEnable := nil;
    DXL_GetTorqueEnables := nil;
    DXL_SetLED := nil;
    DXL_SetGoalAngle := nil;
    DXL_SetGoalAngles := nil;
    DXL_GetPresentAngle := nil;
    DXL_GetPresentAngles := nil;
    DXL_StandStillAngle := nil;
    DXL_StandStillAngles := nil;
    DXL_SetGoalVelocity := nil;
    DXL_SetGoalVelocities := nil;
    DXL_GetPresentVelocity := nil;
    DXL_GetPresentVelocities := nil;
    DXL_SetGoalAngleAndVelocity := nil;
    DXL_SetGoalAnglesAndVelocities := nil;
    DXL_SetGoalAngleAndTime := nil;
    DXL_SetGoalAnglesAndTime := nil;
    DXL_SetGoalAngleAndTime2 := nil;
    DXL_SetGoalAnglesAndTime2 := nil;
    DXL_SetGoalPWM := nil;
    DXL_SetGoalPWMs := nil;
    DXL_GetPresentPWM := nil;
    DXL_GetPresentPWMs := nil;
    DXL_SetGoalCurrent := nil;
    DXL_SetGoalCurrents := nil;
    DXL_GetPresentCurrent := nil;
    DXL_GetPresentCurrents := nil;
    DXL_SetDriveMode := nil;
    DXL_SetDriveModesEquival := nil;
    DXL_SetOperatingMode := nil;
    DXL_SetOperatingModesEquival := nil;
    DXL_GetOperatingMode := nil;
    DXL_GetHWErrorCode := nil;
    DXL_GetErrorCode := nil;
    DXL_GetModelInfo := nil;
    DXL_ScanDevices := nil;
//    DXL_PrintDevicesList := nil;
    DXL_InitDevicesList := nil;
  end;
end;

function DXLIB_Active:boolean;
begin
  result := DLLHandle <> 0;
end;

begin
  DLLHandle := 0;
end.
