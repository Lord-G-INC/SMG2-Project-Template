#include "pt/MapObj/BlueCoinSystem/BlueCoinManager.h"

const char* SaveNames[3] = {"BlueCoinData1.bin", "BlueCoinData2.bin", "BlueCoinData3.bin"};

extern "C" s32 NANDSeek(NANDFileInfo*, s32, s32);

void test() {
    GameSystemFunction::prepareResetSystem();

    if (resetted) {
        OSReport("%u\n", datas[0][1]);
    }
}

kmCall(0x804B99F4, test);

void onReset(LiveActor* pActor) {
    pActor->initHitSensor(1);
    resetted = true;
}

kmCall(0x8024F358, onReset);