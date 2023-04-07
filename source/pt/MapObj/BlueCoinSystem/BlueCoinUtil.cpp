#ifdef SMSS
#include "pt/MapObj/BlueCoinSystem/BlueCoinUtil.h"
#include "pt/Util/ActorUtil.h"

bool** gBlueCoinData;
void* gBlueCoinBcsvTable = pt::loadArcAndFile("/SystemData/BlueCoinIDRangeTable.arc", "/BlueCoinIDRangeTable.bcsv");

namespace BlueCoinUtil {
    void loadBlueCoinData() {
        OSReport("(BlueCoinUtil) Attempting BlueCoinData.bin read.\n");
        NANDFileInfo info;
        s32 code = NANDOpen("BlueCoinData.bin", &info, 3);
        
        if (code == -12)
            OSReport("(BlueCoinUtil) BlueCoinData.bin not found. A new one will be created at save time.\n");

        if (code == 0) {
            bool* buffer = new (JKRHeap::sSystemHeap, 0x20) bool[765];
            code = NANDRead(&info, buffer, 765);
            if (code != 0 && code != 765) {
                OSPanic("BlueCoinUtil.cpp", __LINE__, "BlueCoinData.bin read failed! NANDRead code: %d\n", code);
            }
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 255; j++) {
                    gBlueCoinData[i][j] = *buffer++;
                }
            }
            delete [] buffer;
            OSReport("(BlueCoinUtil) BlueCoinData.bin successfully read.\n");
        }
        NANDClose(&info);
    }

    void saveBlueCoinData() {
        s32 code = NANDCreate("BlueCoinData.bin", NAND_PERM_READ_WRITE, 0);
        if (code == 0 || code == -6) {
            NANDFileInfo info;
            code = NANDOpen("BlueCoinData.bin", &info, 3);
            if (code == 0) {
                NANDSeek(&info, 0, 0);
                bool* buffer = new (JKRHeap::sSystemHeap, 0x20) bool[765];
                s32 idx = 0;
                for (int i = 0; i < 3; i++) {
                    for (int j = 0; j < 255; j++) {
                        buffer[idx++] = gBlueCoinData[i][j];
                    }
                }

                code = NANDWrite(&info, buffer, 765);

                if (code != 0 && code != 765)
                    OSPanic("BlueCoinUtil.cpp", __LINE__, "BlueCoinData.bin write failed! NANDWrite code: %d\n\n", code);

                delete [] buffer;
                OSReport("(BlueCoinUtil) BlueCoinData.bin successfully saved.\n");
            }
            NANDClose(&info);
        }
    }

    void initBlueCoinArray() {
        OSReport("(BlueCoinUtil) Initalizing Blue Coin array.\n");
        gBlueCoinData = new bool*[3];
        for (int i = 0; i < 3; i++) {
            gBlueCoinData[i] = new bool[255];
            memset(gBlueCoinData[i], 0, 255);
        }
        OSReport("(BlueCoinUtil) Blue Coin array initalization success.\n");
    }
    
    s32 getCurrentFileNum() { // Thank you AwesomeTMC
        SaveDataHandleSequence *saveDataHandleSequence = GameDataFunction::getSaveDataHandleSequence();
        s32* valuePtr = (s32 *)((char *) (saveDataHandleSequence) + 0x10);
        return *valuePtr - 1;
    }

    void setBlueCoinGotCurrentFile(u8 id) {
        gBlueCoinData[getCurrentFileNum()][id] = true;
    }

    void resetAllBlueCoin(u8 file) {
        memset(gBlueCoinData[file - 1], 0, 255);
        saveBlueCoinData();
    }

    void resetAllBlueCoinAllFileNoSave() {
        for (int i = 0; i < 3; i++) {
            memset(gBlueCoinData[i], 0, 255);
        }
    }

    bool isBlueCoinGot(u8 file, u8 id) {
        return gBlueCoinData[file][id];
    }

    bool isBlueCoinGotCurrentFile(u8 id) {
        return gBlueCoinData[getCurrentFileNum()][id];
    }

    s32 getTotalBlueCoinNum(u8 file) {
        s32 total = 0;
        for (s32 i = 0; i < 255; i++) {
            if (gBlueCoinData[file][i])
                total++;
        }
        return total;
    }

    bool isBlueCoinGot240(u8 fileID) {
        return getTotalBlueCoinNum(fileID) == 240 ? true : false;
    }

    s32 getTotalBlueCoinNumCurrentFile() {
        return getTotalBlueCoinNum(getCurrentFileNum());
    }

    s32 getBlueCoinRangeData(const char* pStageName, bool collectedCoinsOnly) {
        JMapInfo table = JMapInfo();
        table.attach(gBlueCoinBcsvTable);

        const char* tableStageName;
        s32 targetLine = -1;

        if (!pStageName)
            pStageName = MR::getCurrentStageName();

        for (s32 i = 0; i < MR::getCsvDataElementNum(&table); i++) {
            MR::getCsvDataStr(&tableStageName, &table, "StageName", i);

            if (MR::isEqualString(pStageName, tableStageName)) {
                targetLine = i;
                break;
            }
        }
        
        if (targetLine > -1) {
            s32 rangeMin;
            s32 rangeMax;
            s32 count = 0;

            MR::getCsvDataS32(&rangeMin, &table, "BlueCoinRangeMin", targetLine);
            MR::getCsvDataS32(&rangeMax, &table, "BlueCoinRangeMax", targetLine);

            for (u32 i = rangeMin; i < rangeMax + 1; i++) {
                if (collectedCoinsOnly) {
                    if (isBlueCoinGotCurrentFile(i))
                        count++;
                }
                else
                    count++;
            }

            OSReport("(BlueCoinIDRangeTable) Stage name: \"%s\", Range: %d through %d, Line: %d, Collected Coins Only: %d, Total: %d\n", pStageName, rangeMin, rangeMax, targetLine, collectedCoinsOnly, count);
            return count;
        }
        
        OSReport("(BlueCoinIDRangeTable) Stage name \"%s\" not found in table. -1 returned!\n", pStageName);
        return -1;
    }
}

// Blue coin binary management

// Delete all blue coins in a save file.
void resetAllBlueCoinOnDeleteFile(SaveDataHandleSequence* pSeq, UserFile* pFile, int fileID) {
    pSeq->restoreUserFileConfigData(pFile, fileID); // Restore original call
    BlueCoinUtil::resetAllBlueCoin(fileID);
}

kmCall(0x804D9BF8, resetAllBlueCoinOnDeleteFile);

// Save gBlueCoinData to file.

void saveBlueCoinDataOnGameSave(const char* pName) {
    MR::startSystemSE(pName, -1, -1);
    BlueCoinUtil::saveBlueCoinData();
    
}
kmCall(0x804DAFD0, saveBlueCoinDataOnGameSave);

// Read blue coin binary on title screen load.
void onTitleScreenLoad(LiveActor* pActor) {
    pActor->initHitSensor(1); // Restore original call

    BlueCoinUtil::resetAllBlueCoinAllFileNoSave();
    BlueCoinUtil::loadBlueCoinData();
}

kmCall(0x8024F358, onTitleScreenLoad);

#endif