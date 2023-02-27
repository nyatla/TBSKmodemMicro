# GetStarted

RaspberyPi picoのMicroPythonでTBSKmodemMicroを使う方法です。

現在のライブラリには受信機能はありませんので、PCやスマートフォンで動作する他の実装の受信機能でテストしてください。

+ [TBSKmodem Live demo](https://nyatla.jp/tbskmodem/)
+ [TBSKmodem for Python](https://github.com/nyatla/TBSKmodem)
+ [TBSKmodem for Processing](https://github.com/nyatla/TBSKmodem-for-Processing)



# セットアップ

## 手動セットアップ

1. https://github.com/nyatla/TBSKmodemMicro/releasesからTbskModemMicro-MicroPython.zipをダウンロードします。
2. Thonnyを起動して、MicropythonをセットアップしたRaspberry Pi Picoを接続します。
3. ライブラリをRaspberry pi picoへコピーします。次のようにファイルを配置します。

+tbskmicro
 +-__init__.py
 +-TBSKPulseModulator.py




# 送信テスト

TBSKmodemの送信機能には、デジタル出力が可能なGPIOピンが1Pinと、Micropythonをある程度の速度で実行できる環境が必要です。
出力デバイスには、パッシブ圧電素子、スピーカーなどが利用できます。

## RP2040


1. 以下のように配線します。
2. getstartedにあるSendHello.pyを開きます。
3. 実行するとスピーカーから音が鳴り、１度だけメッセージを送信します。
4. 受信プログラムに"Hello TBSKmodem from Micro Python."と表示されれば成功です。

送信したメッセージは、ライブデモ、または他のプラットフォームのTBSKmodemで復調することができます。
受信機側の設定は、16kHz、100tick/symbolです。

RP2040では、8kHzと16kHzでの動作を確認済みです。


## その他のボード

Raspbery pi pico以外では、配線図を参考にブザーを接続し、21行目のピン番号を変更することで実行できると思います。

