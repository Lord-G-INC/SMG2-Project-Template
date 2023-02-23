#include "pt/MapObj/BlueCoinSystem/BlueCoinManager.h"
#include "pt/Util/ActorUtil.h"

const char* SaveNames[3] = {"BlueCoinData1.bin", "BlueCoinData2.bin", "BlueCoinData3.bin"};

void* blueCoinBcsvTable = pt::loadArcAndFile("/SystemData/BlueCoinIDRangeTable.arc", "BlueCoinIDRangeTable.bcsv");

namespace BlueCoinUtil {
    void GetData() {
        NANDFileInfo info;
        s32 code = NANDOpen("BlueCoinData.bin", &info, 3);
        if (code == 0) {
            bool* buffer = new (JKRHeap::sSystemHeap, 0x20) bool[765];
            code = NANDRead(&info, buffer, 765);
            if (code != 0 && code != 765) {
                OSPanic("BlueCoinManager.cpp", __LINE__, "NANDRead failed! with code %d.\n", code);
            }
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 255; j++) {
                    gBlueCoinData[i][j] = *buffer++;
                }
            }
            delete [] buffer;
            OSReport("BlueCoinData.bin successfully read.\n");
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
                if (code != 0 && code != 765) {
                    OSPanic("BlueCoinManager.cpp", __LINE__, "NANDWrite failed with code %d.", code);
                }
                delete [] buffer;
                OSReport("BlueCoinData.bin successfully saved.\n");
            }
            NANDClose(&info);
        }
    }

    s32 getCurrentFileNum() { // Thank you AwesomeTMC
        SaveDataHandleSequence *saveDataHandleSequence = GameDataFunction::getSaveDataHandleSequence();
        s32* valuePtr = (s32 *)((char *) (saveDataHandleSequence) + 0x10);
        return *valuePtr - 1;
    }

    void setBlueCoinGotOnCurrentFile(u8 id, bool collected) {
        gBlueCoinData[getCurrentFileNum()][id] = collected;
    }

    void resetAllBlueCoin(u8 file) {
        memset(gBlueCoinData[file - 1], 0, 255);
        SaveData();
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
        u8 total = 0;
        for (s32 i = 0; i < 255; i++) {
            if (gBlueCoinData[file][i])
                total++;
        }
        return total;
    }

    s32 getTotalBlueCoinNumCurrentFile() {
        return getTotalBlueCoinNum(getCurrentFileNum());
    }

    s32 getTotalBlueCoinRangeNumFromBcsv(u8 file, const char* pStageName) {
        JMapInfo table = JMapInfo();
        table.attach(blueCoinBcsvTable);

        const char* tableStageName;
        s32 targetLine = -1;

        for (u8 i = 0; i < MR::getCsvDataElementNum(&table); i++) {
            MR::getCsvDataStr(&tableStageName, &table, "StageName", i);

            if (MR::isEqualString(pStageName, tableStageName)) {
                targetLine = i;
                break;
            }
        }
        
        if (targetLine >= 0) {
            s32 rangeMin;
            s32 rangeMax;
            s32 count = 0;

            MR::getCsvDataS32(&rangeMin, &table, "BlueCoinRangeMin", targetLine);
            MR::getCsvDataS32(&rangeMax, &table, "BlueCoinRangeMax", targetLine);

            for (u8 i = rangeMin; i < rangeMax + 1; i++) {
                if (isBlueCoinGot(file, i))
                    count++;
            }
            OSReport("%s found in table! Count: %d, Min: %d, Max: %d\n", tableStageName, count, rangeMin, rangeMax);
            return count;
        }
        OSReport("%s not found in table. -1 returned!\n", pStageName);
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
void saveBlueCoinDataOnGameSave(UserFile* pFile) {
    pFile->setCreated(); // Restore original call
    BlueCoinUtil::SaveData();
}

kmCall(0x804D9C90, saveBlueCoinDataOnGameSave);

// Read blue coin binary on title screen load.
void onTitleScreenLoad(LiveActor* pActor) {
    pActor->initHitSensor(1); // Restore original call

    BlueCoinUtil::resetAllBlueCoinAllFileNoSave();
    
    BlueCoinUtil::GetData();
}

kmCall(0x8024F358, onTitleScreenLoad);