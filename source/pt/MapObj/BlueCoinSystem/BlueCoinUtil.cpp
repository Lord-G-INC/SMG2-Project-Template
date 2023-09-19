#if defined USEBLUECOIN && !defined SM64BLUECOIN
#include "pt/MapObj/BlueCoinSystem/BlueCoin.h"
#include "pt/MapObj/BlueCoinSystem/BlueCoinLayouts.h"
#include "pt/MapObj/BlueCoinSystem/BlueCoinUtil.h"
#include "pt/Util/ActorUtil.h"
#include "Game/NPC/TalkMessageCtrl.h"

BlueCoinData* gBlueCoinData;

void* gBlueCoinBcsvTable = pt::loadArcAndFile("/SystemData/BlueCoinIDRangeTable.arc", "/BlueCoinIDRangeTable.bcsv");

#define BINSIZE 795

#define FLAGS_LOCATION 765
#define SPENT_LOCATION 789
#define TEXTBOX_LOCATION 792

namespace BlueCoinUtil {
    void loadBlueCoinData() {
        OSReport("(BlueCoinUtil) Attempting BlueCoinData.bin read.\n");
        NANDFileInfo info;
        s32 code = NANDOpen("BlueCoinData.bin", &info, 3);
        
        if (code == -12) {
            OSReport("(BlueCoinUtil) BlueCoinData.bin not found. A new one will be created on game save.\n");
        }

        if (code == 0) {
            u8* buffer = new (JKRHeap::sSystemHeap, 0x20) u8[BINSIZE];
            code = NANDRead(&info, buffer, BINSIZE);
            if (code != 0 && code != BINSIZE) {
                OSPanic(__FILE__, __LINE__, "BlueCoinData.bin read failed. NANDRead code: %d", code);
            }
            s32 flagidx = 0;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 255; j++) {
                    gBlueCoinData->collectionData[i][j] = *buffer++;
                }
            }

            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 8; j++) {
                    gBlueCoinData->flags[i][j] = (bool)buffer[flagidx];
                    flagidx++;
                }
            }

            for (s32 i = 0; i < 3; i++) {
                gBlueCoinData->spentData[i] = buffer[24+i];
                gBlueCoinData->hasSeenTextBox[i] = (bool)buffer[27+i];

                if (gBlueCoinData->spentData[i] > getTotalBlueCoinNum(i, false))
                    OSReport("(BlueCoinUtil) Discrepancy found on save file %d\n", i);
            }

            delete [] buffer;
            OSReport("(BlueCoinUtil) BlueCoinData.bin successfully read.\n");
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
                u8* buffer = new (JKRHeap::sSystemHeap, 0x20) u8[BINSIZE];
                s32 idx = 0;
                s32 flagidx = 0;

                for (int i = 0; i < 3; i++) {
                    buffer[SPENT_LOCATION+i] = gBlueCoinData->spentData[i];
                    buffer[TEXTBOX_LOCATION+i] = (bool)gBlueCoinData->hasSeenTextBox[i];

                    for (int j = 0; j < 255; j++) {
                        buffer[idx++] = (bool)gBlueCoinData->collectionData[i][j];
                    }

                    for (int j = 0; j < 8; j++) {
                        buffer[FLAGS_LOCATION+flagidx] = gBlueCoinData->flags[i][j];
                        flagidx++;
                    }
                }

                code = NANDWrite(&info, buffer, BINSIZE);

                if (code != 0 && code != BINSIZE)
                    OSPanic(__FILE__, __LINE__, "BlueCoinData.bin write failed. NANDWrite code: %d", code);

                delete [] buffer;
                OSReport("(BlueCoinUtil) BlueCoinData.bin successfully saved.\n");
                printBlueCoinSaveFileInfo();
            }
            NANDClose(&info);
        }
    }

    void printBlueCoinSaveFileInfo() {
        char flagstr[3][9];

        for (s32 i = 0; i < 3; i++) {
            flagstr[i][8] = 0;

            for (s32 j = 0; j < 8; j++) {
            flagstr[i][j] = gBlueCoinData->flags[i][j] ? 0x31 : 0x30;
            }
        }

        OSReport("Blue Coin save file info\nc0: %d, c1: %d, c2: %d\nf0: %s, f1: %s, f2: %s\ns0: %d, s1: %d, s2: %d\nm0: %s, m1: %s, m2: %s\n", 
        getTotalBlueCoinNum(0, false), 
        getTotalBlueCoinNum(1, false), 
        getTotalBlueCoinNum(2, false),
        flagstr[0],
        flagstr[1],
        flagstr[2],
        gBlueCoinData->spentData[0],
        gBlueCoinData->spentData[1],
        gBlueCoinData->spentData[2],
        gBlueCoinData->hasSeenTextBox[0] ? "Yes" : "No",
        gBlueCoinData->hasSeenTextBox[1] ? "Yes" : "No",
        gBlueCoinData->hasSeenTextBox[2] ? "Yes" : "No"
        );
    }

    void clearBlueCoinData() {
        for (int i = 0; i < 3; i++) {
            memset(gBlueCoinData->collectionData[i], 0, 255);
            gBlueCoinData->spentData[i] = 0;
            gBlueCoinData->hasSeenTextBox[i] = 0;

            for (int j = 0; j < 8; j++) {
                gBlueCoinData->flags[i][j] = 0;
            }
        }
        
        OSReport("(BlueCoinUtil) Blue Coin array cleared.\n");
    }

    void initBlueCoinArray() {
        OSReport("(BlueCoinUtil) Initializing Blue Coin array.\n");
        gBlueCoinData = new BlueCoinData;
        gBlueCoinData->collectionData = new bool*[3];
        for (int i = 0; i < 3; i++) {
            gBlueCoinData->collectionData[i] = new bool[255];
            memset(gBlueCoinData->collectionData[i], 0, 255);
            gBlueCoinData->hasSeenTextBox[i] = 0;
        }
        OSReport("(BlueCoinUtil) Blue Coin array initalization complete.\n");
    }

    s32 getCurrentFileNum() {
        SaveDataHandleSequence *saveDataHandleSequence = GameDataFunction::getSaveDataHandleSequence();
        return *(s32 *)((char *) (saveDataHandleSequence) + 0x10) - 1;
    }

    void setBlueCoinGotCurrentFile(u8 id) {
        gBlueCoinData->collectionData[getCurrentFileNum()][id] = true;
        OSReport("(BlueCoinUtil) Blue Coin ID #%d collected on file %d.\n", id, getCurrentFileNum());
    }

    bool isBlueCoinGot(u8 file, u8 id) {
        return gBlueCoinData->collectionData[file][id];
    }
    
    bool isBlueCoinGotCurrentFile(u8 id) {
        return gBlueCoinData->collectionData[getCurrentFileNum()][id];
    }

    bool hasSeenBlueCoinTextBoxCurrentFile() {
        return gBlueCoinData->hasSeenTextBox[getCurrentFileNum()];
    }  

    void setSeenBlueCoinTextBoxCurrentFile() {
        gBlueCoinData->hasSeenTextBox[getCurrentFileNum()] = true;
        OSReport("(BlueCoinUtil) gBlueCoinData->hasSeenTextBox on the current file set to true.\n");
    }

    void setOnBlueCoinFlagCurrentFile(u8 flag) {
        gBlueCoinData->flags[getCurrentFileNum()][flag] = true;
        OSReport("(BlueCoinUtil) Flag %d set on file %d, Flag num: %d\n", flag, getCurrentFileNum(), getCurrentFileNum()*flag);
    }

    bool isOnBlueCoinFlagCurrentFile(u8 flag) {
        return gBlueCoinData->flags[getCurrentFileNum()][flag];
    }

    void resetAllBlueCoin(u8 file) {
        memset(gBlueCoinData->collectionData[file - 1], 0, 255);
        gBlueCoinData->spentData[file - 1] = 0;
        gBlueCoinData->hasSeenTextBox[file - 1] = 0;

        for (int i = 0; i < 8; i++) {
            gBlueCoinData->flags[file - 1][i] = 0;
        }

        saveBlueCoinData();
        OSReport("(BlueCoinUtil) Blue Coin data for file %d reset.\n", file);
    }

    bool isBlueCoinTextBoxAppeared() {
        if (hasSeenBlueCoinTextBoxCurrentFile())
            return false;
        else
            return MR::getGameSceneLayoutHolder()->mCounterLayoutController->mPTDBlueCoinCounter->isNerve(&NrvBlueCoinCounter::NrvShowTextBox::sInstance);
    }

    void spendBlueCoinCurrentFile(u8 numcoin) {
        numcoin == 0 ? 30 : numcoin;
        if (getTotalBlueCoinNumCurrentFile(true) >= numcoin) {
            gBlueCoinData->spentData[getCurrentFileNum()] += numcoin;
            OSReport("(BlueCoinUtil) Transaction of %d Blue Coins accepted\n", numcoin);
        }
        else
            OSReport("(BlueCoinUtil) Transaction of %d Blue Coins not accepted. Too few coins.\n", numcoin);
    }

    s32 getSpentBlueCoinNum(u8 file) {
        return gBlueCoinData->spentData[file];
    }

    s32 getSpentBlueCoinNumCurrentFile() {
        return gBlueCoinData->spentData[getCurrentFileNum()];
    }

    s32 getTotalBlueCoinNum(u8 file, bool ignoreSpent) {
        s32 total = 0;
        for (s32 i = 0; i < 255; i++) {
            if (gBlueCoinData->collectionData[file][i])
                total++;
        }

        return ignoreSpent ? total -= getSpentBlueCoinNum(file) : total;
    }

    s32 getTotalBlueCoinNumCurrentFile(bool ignoreSpent) {
        return getTotalBlueCoinNum(getCurrentFileNum(), ignoreSpent);
    }

    void startCounterCountUp() {
        ((BlueCoinCounter*)MR::getGameSceneLayoutHolder()->mCounterLayoutController->mPTDBlueCoinCounter)->startCountUp();
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

    void getBlueCoinPaneNameFromTable(LayoutActor* pLayout, const char* pStageName) {
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

        s32 texID = 0;  
        MR::getCsvDataS32(&texID, &table, "TexID", targetLine);
        char paneName[10];
        snprintf(paneName, 10, "ShaCoin%02d", texID);
        OSReport("(BlueCoinIDRangeTable) Pane \"ShaCoin%02d\" selected.\n", texID);
        MR::showPaneRecursive(pLayout, (const char*)paneName);
    }

    LiveActor* createBlueCoinForSpawning(LiveActor* pSourceActor, s32 id) {
        if (id > -1) {
            BlueCoin* coin = new BlueCoin("BlueCoinS");
            coin->mID = id;
            MR::addToCoinHolder(pSourceActor, coin);
            coin->initWithoutIter();
            MR::hideModel(coin);
            MR::invalidateHitSensors(coin);
            pSourceActor->mActionKeeper->mItemGenerator = 0;
            OSReport("(BlueCoinUtil) Created Blue Coin with ID %d for the actor \"%s\".\n", id, pSourceActor->mName);
            return coin;
        }

        return 0;
    }

    void appearBlueCoin(LiveActor* pSourceActor, LiveActor* pBlueCoin) {
        if (pBlueCoin) {
            TVec3f coinVelocity = TVec3f(0.0f, 25.0f, 0.0f);
            coinVelocity.scale(coinVelocity.y, -pSourceActor->mGravity);

            MR::startSystemSE("SE_SY_PURPLE_COIN_APPEAR", -1, -1);

            ((BlueCoin*)pBlueCoin)->appearMove(pSourceActor->mTranslation, coinVelocity, 0x7FFFFFFF, 60);
        }
    }
}

// Blue coin binary management

// Delete all blue coins in a save file.
void resetAllBlueCoinOnDeleteFile(SaveDataHandleSequence* pSeq, UserFile* pFile, int fileID) {
    pSeq->restoreUserFileConfigData(pFile, fileID); // Restore original call
    BlueCoinUtil::resetAllBlueCoin(fileID);
}

kmCall(0x804D9BF8, resetAllBlueCoinOnDeleteFile);

// Save gBlueCoinData->collectionData to file.

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

void customLMSBranchConditions(TalkNodeCtrl* pCtrl, bool result) {
    u16 condType = ((u16*)pCtrl->getCurrentNodeBranch())[3];
    u16 condParam = ((u16*)pCtrl->getCurrentNodeBranch())[4];

    if (condType == 17)
        result = BlueCoinUtil::isBlueCoinGotCurrentFile(condParam);
    if (condType == 18)
        result = BlueCoinUtil::getTotalBlueCoinNumCurrentFile(false) >= condParam;
    if (condType == 19)
        result = BlueCoinUtil::getTotalBlueCoinNumCurrentFile(true) >= condParam;


    pCtrl->forwardCurrentBranchNode(result);
}

kmWrite32(0x8037B134, 0x7FC3F378); // mr r3, r30
kmWrite32(0x8037B138, 0x7CC43378); // mr r4, r6
kmCall(0x8037B13C, customLMSBranchConditions);
kmWrite32(0x8037B140, 0x4BFFFE9C); // b -0x164



bool customLMSEventFunctions(TalkNodeCtrl* pNode) {
    u16 eventType = ((u16*)pNode->getCurrentNodeEvent())[2];
    u16 eventParam = ((u16*)pNode->getCurrentNodeEvent())[5];

    if (eventType == 12)
        BlueCoinUtil::spendBlueCoinCurrentFile(eventParam);
    if (eventType == 13)
        BlueCoinUtil::startCounterCountUp();

    return pNode->isExistNextNode();
}

kmCall(0x8037B38C, customLMSEventFunctions);
#endif