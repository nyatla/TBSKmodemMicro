# GetStarted

Arduino IDEでTBSKmodemMicroを使う方法です。

対向デバイスにはPCやスマートフォンで動作する他の実装をお勧めします。

+ [TBSKmodem Live demo](https://nyatla.jp/tbskmodem/)
+ [TBSKmodem for Python](https://github.com/nyatla/TBSKmodem)
+ [TBSKmodem for Processing](https://github.com/nyatla/TBSKmodem-for-Processing)



# セットアップ

## 手動セットアップ

1. [github](https://github.com/nyatla/TBSKmodemMicro/releases)からTbskModemMicro-Ardiuno.zipをダウンロードします。
2. Arduino IDEを起動し、スケッチ→ライブラリをインクルード→Zip形式のライブラリをインストール...を選択します。
3. ダウンロードしたライブラリを選択します。
4. ライブラリを有効化するためにArduino IDEを再起動してください。



# 送信テスト

1秒おきにテキストメッセージを送信するスケッチのテスト方法です。

送信したメッセージは、ライブデモ、または他のプラットフォームのTBSKmodemで復調することができます。

受信機側の設定は、16kHz、100tick/symbolです。


## RaspberryPi pico

1. 以下のように配線します。GP2ピンとGNDに圧電素子を接続し、保護用のクリッピングダイオードをVCCとGNDに接続します。
<img src="https://user-images.githubusercontent.com/2483108/221571659-862a3138-616e-430c-8516-73b77d2e412f.png" width="30%">

2. 受信用のプログラムを起動します。PC,スマートフォンならば、[ライブデモ](https://nyatla.jp/tbskmodem/)が便利です。
3. Arduino IDEのファイル→スケッチ例→TbskModemMicroから、SendHelloを選択して開きます。
4. RaspberryPi picoを書き込みモードで起動します。BOOTSELを押しながら電源を入れます。
5. スケッチを書き込みます。
6. 書き込みが完了すると、１秒おきに信号を送信しはじめます。（スピーカーが鳴ります。）
6. Livedemo、または他のTBSKmodemで信号を受信してください。<b>Hello arduino</b>と表示されれば成功です。



## その他のボードでのテスト

1. 配線図を参考に、DigitalOutが可能なピンにブザー、またはスピーカーを配線します。
2. スケッチの出力ピン番号を書き換えます。
3. スケッチをボードに書き込みます。


