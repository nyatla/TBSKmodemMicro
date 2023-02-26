"""	Pure MicrPythonな近接音響データ送信サンプルコードです。
    RP2040の　GP2にブザーかスピーカーをつないでね。
    念のためダイオードでクリッピングしてください。
    TBSKmodemの受信側は16000Hz(160bps)又は8000Hz(bps)を選択してください。
    https://nyatla.jp/tbskmodem/ 
"""
from array import array
from machine import Pin
from utime import ticks_diff,ticks_us,ticks_add
import gc
import random

class BitArray:#(Iterator[int]):
    """ ビットを格納するイテレータ
    """
    def __init__(self,buf:array=None,size:int=None):
       self.buf=buf if buf is not None else array("B",[0]*((size+7)//8))
       self.size=size if size is not None else len(self.buf)*8
       self._c=0
    def __getitem__(self,i:int):
        return (self.buf[i//8]>>(7-i%8)) &0x01
    def __setitem__(self,i:int,v:int):
        n=(v&0x01)<<(7-i%8)
        self.buf[i//8]= self.buf[i//8]&(~n) if n==0 else self.buf[i//8]|n
    def __iter__(self):
        return self
    @micropython.native
    def __next__(self)->int:
        c=self._c
        if c>=self.size:
            raise StopIteration()
        r=self.buf[c//8]>>(7-c%8)
        self._c=c+1
        return r& 0x01

def genPnBits(size:int,seed:int=122):
    """ 長さがsizeのPNトーンを生成します。
    """
    random.seed(seed)
    a=array("B",[random.randint(0,255) for _ in range((size+7)//8)])
    return BitArray(a,size)
def genRawBits(l)->"BitArray":
    """ bit配列(List[int])からBitArrayを生成します。
    """
    a=array("B",[0]*((len(l)+7)//8))
    for i in range(len(l)):
        a[i//8]=a[i//8]|((l[i]&0x01)<<(7-i%8))
    return BitArray(a,len(l))
def genDataBits(src:array)->"BitArray":
    bs=BitArray(src)
    a=array("B",[0]*((bs.size+1+7)//8)) # [0] is initial bit
    lastbit=0
    for i in range(bs.size):
        if bs[i]==0:
            lastbit=(lastbit+1)%2
        a[(i+1)//8]=a[(i+1)//8]|(lastbit<<(7-(i+1)%8))
    return BitArray(a,bs.size+1)


class TbskPulseModulator:
    """TBSK変調した矩形波パターンを生成します。
    """
    def __init__(self,preamble_size:int=4,symbol_ticks:int=100):        
        self._preamble_size=preamble_size
        b=[1]*preamble_size
        c=[i%2 for i in range(preamble_size)]
        d=[(1+c[-1])%2,(1+c[-1])%2,c[-1],]
        self._preamble=genRawBits([0,1]+b+[1]+c+d)
        self._tone=genPnBits(symbol_ticks)
    @micropython.native
    def modulete(self,src:array):#->Generator[int,None,None]:
        tb=self._tone.buf
        tl=self._tone.size
        data=genDataBits(src)
        
        #@micropython.native
        def G():
            iter1:ptr=iter(self._preamble)
            iter2:ptr=iter(data)
            for i in range(tl):
                yield (i%3) & 0x01
            for i in iter1:
                for j in range(tl):
                    yield (i ^ (tb[j//8]>>(7-j%8))) #int
            for i in iter2:
                for j in range(tl):
                    yield (i ^ (tb[j//8]>>(7-j%8))) #int
            for i in range(tl): #suffix
                yield (i%3) & 0x01
        return G()
    @micropython.native
    def gpioTx(self,bits:array,carrier:int,pin:Pin):
        bits=self.modulete(bits)
        INTERVAL_US=(1000000//carrier) #8kHz period setting
        INTERVAL_DC=0 if 1000000%carrier==0 else (int)(carrier/(1000000%carrier))
        target :int= ticks_us()
        starget:int=target
        c:int=0
        #start_1=ticks_us()
        for i in bits:
            target:int=ticks_add(target,INTERVAL_US)
            while True:
                k:int=ticks_diff(target,ticks_us())
                if k>12:
                    continue
                elif k<-12:
                    target=target-k
                if INTERVAL_DC!=0 and c%INTERVAL_DC==0:
                    target=target+1
                pin.value(i) #125us毎に呼び出されるのを期待している。
                break
            c=c+1        



tmm=TbskPulseModulator()
gc.collect()
gc.disable()
tmm.gpioTx(
    array("B",b"Hello TBSKmodem from Micro Python."),
    16000,Pin(2, Pin.OUT, Pin.PULL_DOWN))
gc.enable()

