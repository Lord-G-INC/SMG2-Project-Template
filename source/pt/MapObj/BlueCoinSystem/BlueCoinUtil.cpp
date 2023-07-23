#if defined USEBLUECOIN && !defined SM64BLUECOIN
#include "pt/MapObj/BlueCoinSystem/BlueCoinUtil.h"
#include "pt/MapObj/BlueCoinSystem/BlueCoinLayouts.h"
#include "pt/Util/ActorUtil.h"
#include "Game/NPC/TalkMessageCtrl.h"

bool** gBlueCoinData;
bool gBlueCoinFlag;

void* gBlueCoinBcsvTable = pt::loadArcAndFile("/SystemData/BlueCoinIDRangeTable.arc", "/BlueCoinIDRangeTable.bcsv");

#define BINSIZE 766

namespace BlueCoinUtil {
    void loadBlueCoinData() {
        OSReport("(BlueCoinUtil) Attempting BlueCoinData.bin read.\n");
        NANDFileInfo info;
        s32 code = NANDOpen("BlueCoinData.bin", &info, 3);
        
        if (code == -12) {
            gBlueCoinFlag = 0;
            OSReport("(BlueCoinUtil) BlueCoinData.bin not found. A new one will be created on game save.\n");
        }

        if (code == 0) {
            bool* buffer = new (JKRHeap::sSystemHeap, 0x20) bool[BINSIZE];
            code = NANDRead(&info, buffer, BINSIZE);
            if (code != 0 && code != BINSIZE) {
                OSPanic(__FILE__, __LINE__, "BlueCoinData.bin read failed. NANDRead code: %d\n", code);
            }
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 255; j++) {
                    gBlueCoinData[i][j] = *buffer++;
                }
            }

            gBlueCoinFlag = *buffer;

            delete [] buffer;
            OSReport("(BlueCoinUtil) BlueCoinData.bin successfully saved.\n");
            printBlueCoinSaveFileInfo();
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
                bool* buffer = new (JKRHeap::sSystemHeap, 0x20) bool[BINSIZE];
                s32 idx = 0;
                for (int i = 0; i < 3; i++) {
                    for (int j = 0; j < 255; j++) {
                        buffer[idx++] = gBlueCoinData[i][j];
                    }
                }

            buffer[765] = gBlueCoinFlag;

                code = NANDWrite(&info, buffer, BINSIZE);

                if (code != 0 && code != BINSIZE)
                    OSPanic(__FILE__, __LINE__, "BlueCoinData.bin write failed. NANDWrite code: %d\n", code);

                delete [] buffer;
                OSReport("(BlueCoinUtil) BlueCoinData.bin successfully saved.\n");
                printBlueCoinSaveFileInfo();
            }
            NANDClose(&info);
        }
    }

    void printBlueCoinSaveFileInfo() {
        OSReport("f0: %d, f1: %d, f2: %d\nSeen message? %s\n", getTotalBlueCoinNum(0), getTotalBlueCoinNum(1), getTotalBlueCoinNum(2), gBlueCoinFlag ? "Yes" : "No");
    }

    void initBlueCoinArray() {
        OSReport("(BlueCoinUtil) Initializing Blue Coin array.\n");
        gBlueCoinData = new bool*[3];
        for (int i = 0; i < 3; i++) {
            gBlueCoinData[i] = new bool[255];
            memset(gBlueCoinData[i], 0, 255);
        }
        gBlueCoinFlag = 0;
        OSReport("(BlueCoinUtil) Blue Coin array initalization complete.\n");
    }

    s32 getCurrentFileNum() { // Thank you AwesomeTMC
        SaveDataHandleSequence *saveDataHandleSequence = GameDataFunction::getSaveDataHandleSequence();
        return *(s32 *)((char *) (saveDataHandleSequence) + 0x10) - 1;
    }

    void setBlueCoinGotCurrentFile(u8 id) {
        gBlueCoinData[getCurrentFileNum()][id] = true;
        OSReport("(BlueCoinUtil) Blue Coin ID #%d collected on file %d.\n", id, getCurrentFileNum());
    }

    void resetAllBlueCoin(u8 file) {
        memset(gBlueCoinData[file - 1], 0, 255);
        saveBlueCoinData();
        OSReport("(BlueCoinUtil) Blue Coin data for file %d reset.\n", file);
    }

    void clearBlueCoinData() {
        for (int i = 0; i < 3; i++) {
            memset(gBlueCoinData[i], 0, 255);
        }
        OSReport("(BlueCoinUtil) Blue Coin array cleared.\n");
    }

    bool isBlueCoinGot(u8 file, u8 id) {
        return gBlueCoinData[file][id];
    }

    bool isBlueCoinGotCurrentFile(u8 id) {
        return gBlueCoinData[getCurrentFileNum()][id];
    }

    void setOnBlueCoinFlag() {
        gBlueCoinFlag = true;
        OSReport("(BlueCoinUtil) gBlueCoinFlag set to true.\n");
    }

    bool isOnBlueCoinFlag() {
        return gBlueCoinFlag;
    }

    bool isBlueCoinTextBoxAppeared() {
        if (isOnBlueCoinFlag())
            return false;
        else
            return MR::getGameSceneLayoutHolder()->mCounterLayoutController->mPTDBlueCoinCounter->isNerve(&NrvBlueCoinCounter::NrvShowTextBox::sInstance);
    }

    s32 getTotalBlueCoinNum(u8 file) {
        s32 total = 0;
        for (s32 i = 0; i < 255; i++) {
            if (gBlueCoinData[file][i])
                total++;
        }
        return total;
    }

    bool isBlueCoinGot240(u8 file) {
        return getTotalBlueCoinNum(file) == 240;
    }

    void startCounterCountUp() {
        ((BlueCoinCounter*)MR::getGameSceneLayoutHolder()->mCounterLayoutController->mPTDBlueCoinCounter)->startCountUp();
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

    BlueCoinUtil::clearBlueCoinData();
    BlueCoinUtil::loadBlueCoinData();
}

kmCall(0x8024F358, onTitleScreenLoad);

// Misc blue coin patches

void customLMSBranchConditions(TalkNodeCtrl* pCtrl, bool cond) {
    u16 condType = ((u16*)pCtrl->getCurrentNodeBranch())[3];
    u16 condParam = ((u16*)pCtrl->getCurrentNodeBranch())[4];

    if (condType == 17)
        cond = BlueCoinUtil::isBlueCoinGotCurrentFile(condParam);
    if (condType == 18)
        cond = BlueCoinUtil::getTotalBlueCoinNumCurrentFile() >= condParam;

    pCtrl->forwardCurrentBranchNode(cond);
}

kmWrite32(0x8037B134, 0x7FC3F378); // mr r3, r30
kmWrite32(0x8037B138, 0x7CC43378); // mr r4, r6
kmCall(0x8037B13C, customLMSBranchConditions);
kmWrite32(0x8037B140, 0x4BFFFE9C); // b -0x164



bool customLMSEventFunctions(TalkNodeCtrl* pNode) {
    u16 eventType = ((u16*)pNode->getCurrentNodeEvent())[2];
    u16 eventParam = ((u16*)pNode->getCurrentNodeEvent())[3];
    OSReport("Event: %d, %d\n", eventType, eventParam);

    return pNode->isExistNextNode();
}

kmCall(0x8037B38C, customLMSEventFunctions);
#endif