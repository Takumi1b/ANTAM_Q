# ANTAM_Q
小型生物用の行動計測用ソフトウェア

# DEMO
[![紹介動画](https://user-images.githubusercontent.com/103201314/220066637-5e15a2f1-559a-4158-a9ae-d4c85d7267ba.png)](https://youtu.be/c_XIWnayPXA)

# Requirement
* Qt Creator(Qt6 6.4.2)
* QSerialPort
* Opencv 4.7.0
* Dynamixel library 
* Webカメラ
* Dynamixelサーボモータ(MX28シリーズ等)

# Installation
リポジトリからダウンロード
```bash
git clone https://github.com/Takumi1b/ANTAM_Q.git
```
Qt Creatorでビルド

# Usage
![Usage](https://user-images.githubusercontent.com/103201314/215585186-4f1a1137-b643-4371-916d-66bea5abaede.png
 "UI説明")
1. 生映像、射影変換後の映像、設定用映像の表示
2. 二値化処理後の映像表示
3. Raw Image:生映像表示, Back View:射影変換後の映像表示, Set Image:設定用映像の表示
4. 台形補正用設定パラメータ　①上をクリックして座標取得、setで座標決定、Set Projective Transformation Imageで射影変換映像の取得
5. 二値化処理の閾値
6. 色抽出用パラメータ
7. PIDゲイン調整用パラメータ
8. 重心座標のオフセット用パラメータ
9. キャリブレーションモードの選択(キャリブレーションモード時、モータOFF,格子、重心点表示)
10. パラメータ, センサデータ, 映像データ保存用ファイルパスの設定
11. モータの個数設定(３もしくは４)
12. 計測開始
13. 終了
14. シリアルモニタ、各種設定を行いConnectで接続、Disconnctで切断、PortRefreshでシリアルポート更新、sendでテキストボックス内の文字列を送信　

# Author
* TanabeTakumi
* 京都産業大学大学院　先端情報学研究科
* i2286095@cc.kyoto-su.ac.jp
