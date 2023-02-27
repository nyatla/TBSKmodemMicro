# GetStarted

RaspberyPi picoのMicroPythonでTBSKmodemMicroを使う方法です。

現在のライブラリには受信機能はありませんので、PCやスマートフォンで動作する他の実装の受信機能でテストしてください。

+ [TBSKmodem Live demo](https://nyatla.jp/tbskmodem/)
+ [TBSKmodem for Python](https://github.com/nyatla/TBSKmodem)
+ [TBSKmodem for Processing](https://github.com/nyatla/TBSKmodem-for-Processing)



# セットアップ

## 手動セットアップ

1. [github](https://github.com/nyatla/TBSKmodemMicro/releases)からTbskModemMicro.zipをダウンロードします。
3. Thonnyを起動して、MicropythonをセットアップしたRaspberry Pi Picoを接続します。
4. ライブラリをRaspberry pi picoへコピーします。MicroPython/RP2040/tbskmicroにあるファイルを、次のように配置します。

<img src="https://user-images.githubusercontent.com/2483108/221583781-5c0df85d-7106-4ed6-89b8-efc91524fbb5.png" width="30%">



# 送信テスト

TBSKmodemの送信機能には、デジタル出力が可能なGPIOピンが1Pinと、Micropythonをある程度の速度で実行できる環境が必要です。

出力デバイスには、パッシブ圧電素子、スピーカーなどが利用できます。

## RP2040


1. 以下のように配線します。GP2ピンとGNDに圧電素子を接続し、保護用のクリッピングダイオードをVCCとGNDに接続します。
<img src="https://user-images.githubusercontent.com/2483108/221571659-862a3138-616e-430c-8516-73b77d2e412f.png" width="30%">

2. 受信用のプログラムを起動します。PC,スマートフォンならば、[ライブデモ](https://nyatla.jp/tbskmodem/)が便利です。
3. getstartedにあるSendHello.pyを開きます。
4. 実行するとスピーカーから音が鳴り、１度だけメッセージを送信します。
5. 受信プログラムに**Hello TBSKmodem from Micro Python.**と表示されれば成功です。


送信したメッセージは、ライブデモ、または他のプラットフォームのTBSKmodemで復調することができます。

受信機側の設定は、16kHz、100tick/symbolです。

RP2040では、8kHzと16kHzでの動作を確認済みです。


## その他のボード

Raspbery pi pico以外では、配線図を参考にブザーを接続し、[21行目](https://github.com/nyatla/TBSKmodemMicro/blob/a169376b739dd30758e67d53046daccb8f1ce12c/MicroPython/RP2040/getstarted/SendHello.py#L21)のピン番号を変更することで実行できると思います。

