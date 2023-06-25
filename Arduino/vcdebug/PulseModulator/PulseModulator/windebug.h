// PulseModulator.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include <ctime>
#pragma once
#define ASSERT(EXPRESSION) {if(!(EXPRESSION)){printf("ASSERT!\n");}}

void randomSeed(int v) {};
int random(int r) {
    return 0x55;
};
void digitalWrite(int pin, int v) {
    printf("DW:%d\n",v);
}
time_t micros()
{
    return clock();
}

