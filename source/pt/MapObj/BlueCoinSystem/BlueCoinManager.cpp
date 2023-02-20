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
                    OSReport("Getting Coin: 0x%x, Collected: %d\n", i, gBlueCoinData[i][j]);
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
                        OSReport("Saving Coin: 0x%x, Collected: %d\n", i, gBlueCoinData[i][j]);
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

    s32 getCurrentFileNum() { // Thank you AwesomeTMC
        SaveDataHandleSequence *saveDataHandleSequence = GameDataFunction::getSaveDataHandleSequence();
        s32 *valuePtr = (s32 *)((char *) (saveDataHandleSequence) + 0x10);
        return *valuePtr - 1;
    }

    void setBlueCoinGotOnCurrentFile(u8 id, bool collected) {
        gBlueCoinData[BlueCoinUtil::getCurrentFileNum()][id] = collected;
    }

    void resetAllBlueCoin(u8 file) {
        memset(gBlueCoinData[file - 1], 0, 255);
        SaveData();
    }

    bool isBlueCoinGot(u8 file, u8 id) {
        return gBlueCoinData[file][id];
    }
}

// Delete all blue coins in a save file.
void resetAllBlueCoinOnDeleteFile(SaveDataHandleSequence* pSeq, UserFile* pFile, int fileID) {
    pSeq->restoreUserFileConfigData(pFile, fileID);
    BlueCoinUtil::resetAllBlueCoin(fileID);
}

kmCall(0x804D9BF8, resetAllBlueCoinOnDeleteFile);

// Save gBlueCoinData to file.
void saveBlueCoinDataOnGameSave(UserFile* pFile) {
    pFile->setCreated(); // Restore original call
    BlueCoinUtil::SaveData();
}

kmCall(0x804D9C90, saveBlueCoinDataOnGameSave);

// Read blue coin binary on title screen load.
void onTitleScreenLoad(LiveActor* pActor) {
    pActor->initHitSensor(1); // Restore original call
    BlueCoinUtil::GetData();
}

kmCall(0x8024F358, onTitleScreenLoad);