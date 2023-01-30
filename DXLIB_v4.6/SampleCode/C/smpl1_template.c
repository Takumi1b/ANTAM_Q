/*
  ライブラリを使用するための雛形
*/

// dllを動的にロードする場合は「_DYNAMICLOAD」をdxlib.hをインクルードする前に宣言
//#define _DYNAMICLOAD

#include <stdio.h>

// Dynamixelライブラリのヘッダ
#include  "dxlib.h"
// サンプル用のポート及びIDのマクロ宣言
#include  "dxmisc.h"

int main (void) {
  // デバイスID (COMポート毎のユニーク値)
  TDeviceID  dev;

#if defined(_DYNAMICLOAD) && defined(_WIN32)
  // DLLをロード
  if (LoadDLL()) {
#endif

    // 指定されたパラメータでポートを開く
    // 成功すれば0以外のユニークな値(デバイスID)が返される
    // 以後このデバイスIDを使用する
    dev = DX_OpenPort (_COMPORT, _BAUDRATE);

    // devが0でなければポートを開く事に成功
    if (dev) {
      printf ("Successful opening of %s\n", _COMPORT);

      // ----ここにやりたい事を書く------------


      // --------------------------------------

      // 使い終わったポートを閉じる
      DX_ClosePort (dev);

    } else {
      printf ("Failed to open %s\n", _COMPORT);
    }
#if defined(_DYNAMICLOAD) && defined(_WIN32)
    // DLLをアンロード
    UnloadDLL();
  }
#endif

  printf ("Fin\n");
}
