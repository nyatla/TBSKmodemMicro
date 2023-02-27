"""	Pure MicrPythonな近接音響データ送信サンプルコードです。
    
    RP2040のGP2にブザーかスピーカーを接続してください。
    念のためダイオードでクリッピングすることをお勧めします。
    TBSKmodemの受信側は16000Hz(160bps)を選択してください。
    https://nyatla.jp/tbskmodem/ 
"""
from array import array
from machine import Pin
import gc
from tbskmicro import TbskPulseModulator

led=Pin(25, Pin.OUT, Pin.PULL_DOWN)

tmm=TbskPulseModulator()
led.value(1)
gc.collect()
gc.disable()
#Critical zone
tmm.write(
	Pin(2, Pin.OUT, Pin.PULL_DOWN),16000,
	array("B",b"Hello TBSKmodem from Micro Python.")
)
#/Critical zone
gc.enable()
led.value(0)

