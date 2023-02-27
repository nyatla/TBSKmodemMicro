# GetStarted

Arduino IDEでTBSKmodemMicroを使う方法です。

現在のライブラリには受信機能はありませんので、PCやスマートフォンで動作する他の実装の受信機能でテストしてください。

+ [TBSKmodem Live demo](https://nyatla.jp/tbskmodem/)
+ [TBSKmodem for Python](https://github.com/nyatla/TBSKmodem)
+ [TBSKmodem for Processing](https://github.com/nyatla/TBSKmodem-for-Processing)



# セットアップ

## 手動セットアップ

1. https://github.com/nyatla/TBSKmodemMicro/releasesからTbskModemMicro.zipをダウンロードします。
2. Arduino IDEを起動し、スケッチ→ライブラリをインクルード→Zip形式のライブラリをインストール...を選択します。
3. ダウンロードしたライブラリを選択します。



# 送信テスト

1秒おきにテキストメッセージを送信するスケッチのテスト方法です。
送信したメッセージは、ライブデモ、または他のプラットフォームのTBSKmodemで復調することができます。
受信機側の設定は、16kHz、100tick/symbolです。


## RaspberryPi pico

1. 以下のように配線します。
2. Arduino IDEのファイル→スケッチ例→TbskModemMicroから、SendHelloを選択して開きます。
3. RaspberryPi picoを書き込みモードで起動します。BOOTSELを押しながら電源を入れます。
4. スケッチを書き込みます。
5. 書き込みが完了すると、１秒おきに"Hello Arduino"を送信しはじめます。
6. Livedemo、または他のTBSKmodemで信号を受信してください。



## その他のボードでのテスト

1. 配線図を参考に、DigitalOutが可能なピンにブザー、またはスピーカーを配線します。
2. スケッチの出力ピン番号を書き換えます。
3. スケッチをボードに書き込みます。


