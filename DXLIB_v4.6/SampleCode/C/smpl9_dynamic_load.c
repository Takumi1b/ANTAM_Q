/*
  DLLが無くてもひとまず死なない

  ターゲット:DX/AX/RX/EX/MXシリーズ
*/

#include  <stdio.h>
#ifdef _WIN32
#include  <conio.h>
#else
#include  "kbhit.h"
#endif

#define _DYNAMICLOAD  // ★★dxlib.hをインクルードする前に宣言★★
#include  "dxlib.h"
#include  "dxmisc.h"
#include  "dxmemmap.h"

//===========================================================
// メイン
//===========================================================
int main (void) {
  TErrorCode  err;
  TDeviceID   dev;

 #if defined(_DYNAMICLOAD) && defined(_WIN32)
 // DLLをロード
  if (LoadDLL ()) {
#endif

    if ((dev = DX_OpenPort (_COMPORT, _BAUDRATE))) {
      printf ("Successful opening of %s\n", _COMPORT);

      while (!kbhit () && DX_Active (dev)) {
        uint16_t  dat;
        uint8_t   moving; 

        // 最大トルクを約30%に制限
        DX_WriteWordData (dev, _TARGETID, ADDRESS_TORQUE_LIMIT, 300, &err);

        // 400の位置へ移動
        if (DX_WriteWordData (dev, _TARGETID, ADDRESS_GOAL_POSITION, 400, &err)) {
          do {
            // 移動中の現在位置を読み出す
            if (DX_ReadWordData (dev, _TARGETID, ADDRESS_PRESENT_POSITION, &dat, &err)) printf ("\rpos:%4d", dat);
            else break;
            if (!DX_ReadByteData (dev, _TARGETID, ADDRESS_MOVING, &moving, &err)) break;
            Sleep (20);
          } while (moving != 0 && !kbhit ());
        }

        // 600の位置へ移動
        if (DX_WriteWordData (dev, _TARGETID, ADDRESS_GOAL_POSITION, 600, &err)) {
          do {
            // 移動中の現在位置を読み出す
            if (DX_ReadWordData (dev, _TARGETID, ADDRESS_PRESENT_POSITION, &dat, &err)) printf ("\rpos:%4d", dat);
            else break;
            if (!DX_ReadByteData (dev, _TARGETID, ADDRESS_MOVING, &moving, &err)) break;
            Sleep (20);
          } while (moving != 0 && !kbhit ());
        }
      }

      // 制御トルクの発生を停止
      DX_WriteByteData (dev, _TARGETID, ADDRESS_TORQUE_ENABLE, 0, &err);

      DX_ClosePort (dev);
    } else {
      printf ("Open error\n");
    }

#if defined(_DYNAMICLOAD) && defined(_WIN32)
    // DLLをアンロード
    UnloadDLL();
  } else printf ("Fail to load DLL!\n");
#endif

  printf ("Fin\n");
}
