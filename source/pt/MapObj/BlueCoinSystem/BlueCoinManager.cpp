#include "pt/MapObj/BlueCoinSystem/BlueCoinManager.h"

const char* SaveNames[3] = {"BlueCoinData1.bin", "BlueCoinData2.bin", "BlueCoinData3.bin"};

extern "C" s32 NANDSeek(NANDFileInfo*, s32, s32);

namespace BlueCoinUtil {
    void GetData() {
        NANDFileInfo info;
        s32 code = NANDOpen("BlueCoinData.bin", &info, 3);
        if (code == 0) {
            bool* buffer = new (JKRHeap::sSystemHeap, 0x20) bool[765];
            code = NANDRead(&info, buffer, 765);
            if (code == -8) {
                OSPanic("BlueCoinManager.cpp", 13, "NANDRead failed! Please report this!");
            }
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 255; j++) {
                    gBlueCoinData[i][j] = *buffer++;
                }
            }
            delete [] buffer;
        }
        NANDClose(&info);
    }
    void SaveData() {
        s32 code = NANDCreate("BlueCoinData.bin", NAND_PERM_READ_WRITE, 0);
        if (code == 0 || code == -6) {
            NANDFileInfo info;
            code = NANDOpen("BlueCoinData.bin", &info, 3);
            if (code == 0) {
                NANDSeek(&info, 0, 0);
                bool* buffer = new (JKRHeap::sSystemHeap, 0x20) bool[765];
                size_t idx = 0;
                for (int i = 0; i < 3; i++) {
                    for (int j = 0; j < 255; j++) {
                        buffer[idx++] = gBlueCoinData[i][j];
                    }
                }
                code = NANDWrite(&info, buffer, 765);
                if (code == -8) {
                    OSPanic("BlueCoinManager.cpp", 39, "NANDWrite failed! Please report this!");
                }
                delete [] buffer;
            }
            NANDClose(&info);
        }
    }
}


void test() {
    GameSystemFunction::prepareResetSystem();

    if (resetted) {
        OSReport("%u\n", gBlueCoinData[0][1]);
    }
}

kmCall(0x804B99F4, test);

void onReset(LiveActor* pActor) {
    pActor->initHitSensor(1);
    resetted = true;
}

kmCall(0x8024F358, onReset);