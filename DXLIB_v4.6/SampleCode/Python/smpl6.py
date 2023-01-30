#!/usr/bin/env python
# -*- coding: utf-8 -*-

# 複数軸へ角度と遷移時間を指令

import sys, time
from dxlib import *  # dxlibをインポート

COMPort = b'\\\\.\\COM3'  # 任意のCOMポート名に修正の事
Baudrate = 57143          # Dynamixelのボーレートと合わせる事
AXISNUM = 8               # 全軸数

#---------------------------------------------
# 角度・遷移時間指令と時間待ち関数
#---------------------------------------------
def SetVeloTime(dev, ids, angs, sec):
  for ang in angs: print(ang),
  print
  # 角度と遷移時間を指令
  DXL_SetGoalAnglesAndTime(dev, ids, (c_double * AXISNUM)(*angs), len(ids), sec)
  time.sleep(sec)

#---------------------------------------------
# (1) ポートを開いてdevを取得(必須)
dev = DX_OpenPort(COMPort, Baudrate)
if dev != None:
  # (2) ID一覧設定 (AXISNUM分のIDを列挙すること)
  IDs = (c_uint8 * AXISNUM)(1,2,3,4,5,6,7,8)
  # (3) ID一覧分のDynamixelを検索しモデル名を表示
  for id in IDs:
    print (id, DXL_GetModelInfo(dev,id).contents.name.decode())

  # (4) ID一覧分のDynamixelをPositionモード=3に変更
  DXL_SetOperatingModesEquival(dev, IDs, AXISNUM, 3)
  # (5) ID一覧分のDynamixelをトルクイネーブル(必須)
  DXL_SetTorqueEnablesEquival(dev, IDs, AXISNUM, True)

  # (6) 角度[deg]と時間[sec]を順次指令 (指定時間待ち付き)
  SetVeloTime(dev, IDs, (c_double * AXISNUM)(-90,-90,-90,-90,-90,-90,-90,-90), 5)
  SetVeloTime(dev, IDs, (c_double * AXISNUM)(90, 90, 90, 90, 90, 90, 90, 90), 1.5)
  SetVeloTime(dev, IDs, (c_double * AXISNUM)(0, 0, 0, 0, 0, 0, 0, 0), 0.5)
  SetVeloTime(dev, IDs, (c_double * AXISNUM)(180, 180, 180, 180, 180, 180, 180, 180), 3.5)
  SetVeloTime(dev, IDs, (c_double * AXISNUM)(0, 0, 0, 0, 0, 0, 0, 0), 1)

  time.sleep(1)

  # (7) ID一覧分のDynamixelをトルクディスエーブル
  DXL_SetTorqueEnablesEquival(dev, IDs, AXISNUM, False)

  # (8) ポートを閉じる(必須)
  DX_ClosePort(dev)
else:
  print('Could not open COM port.')
