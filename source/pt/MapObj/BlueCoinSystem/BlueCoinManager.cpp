#include "pt/MapObj/BlueCoinSystem/BlueCoinManager.h"

const char* SaveNames[3] = {"BlueCoinData1.bin", "BlueCoinData2.bin", "BlueCoinData3.bin"};

extern "C" s32 NANDSeek(NANDFileInfo*, s32, s32);

void BlueCoinManager::SaveData(u8 pSaveID) {
    OSReport("Blue coins are now saving!!\n");
    const char* name = SaveNames[pSaveID-1];
    NANDFileInfo info;
    NANDCreate(name, NAND_PERM_READ_WRITE, 0);
    if (NANDOpen(name, &info, 3) == 0) {
        NANDSeek(&info, 0, 0);
        u32 size = mSize*sizeof(BlueCoinInstance);
        u8* buffer = new (JKRHeap::sSystemHeap, 0x20) u8[size+4];
        memset(buffer, 0, size+4);
        memcpy(buffer, &mSize, 4);
        BlueCoinInstance* instances = (BlueCoinInstance*)(buffer+4);
        for (int i = 0; i < mSize; i++) {
            instances[i] = mInstances[i];
            OSReport("Saving Blue Coin: 0x%x, instances: %d, mInstances: %d\n", i, instances[i].mIsCollected, mInstances[i].mIsCollected);
        }
        s32 code = NANDWrite(&info, buffer, size+4);
        if (code == -8) {
            OSPanic("BlueCoinManager.cpp", 14, "The write to %s failed with code -8, please report this!", name);
        }
        OSReport("%s's size: %d\n", name, code);
        delete[] buffer;
    }
    NANDClose(&info);
}

void BlueCoinManager::GetData(u8 pSaveID) {
    const char* name = SaveNames[pSaveID-1];
    NANDFileInfo info;
    if (NANDOpen(name, &info, 3) == 0) {
        u32 fullsize;
        NANDGetLength(&info, &fullsize);
        u8* buffer = new (JKRHeap::sSystemHeap, 0x20) u8[fullsize];
        s32 code = NANDRead(&info, buffer, fullsize);
        if (code == -8) {
            OSPanic("BlueCoinManager.cpp", 35, "The read to %s failed with code -8, please report this!", name);
        }
        mSize = *(u32*)buffer;
        buffer += 4;
        delete [] mInstances;
        mInstances = new BlueCoinInstance[mSize];
        BlueCoinInstance* data = (BlueCoinInstance*)buffer;
        for (int i = 0; i < mSize; i++) {
            mInstances[i] = data[i];
            OSReport("Loading Blue Coin: 0x%x, data: %d, mInstances: %d\n", i, data[i].mIsCollected, mInstances[i].mIsCollected);
        }

        delete [] buffer;
    }
    NANDClose(&info);
}

// Blue Coin Data binary management patches

namespace BlueCoinUtil {
    void resetAllBlueCoinsAndSave(u8 fileID) {
        for (s32 i = 0; i < 254; i++) {
            gBlueCoinManager[i]->mIsCollected = false;
        }
        gBlueCoinManager.SaveData(fileID);
    }

    s32 calcFileNum() { // someone PLEASE tell me how to get the loaded save file number/ID
        const char* dataName = GameDataFunction::getSaveDataHandleSequence()->getCurrentUserFile()->getConfigDataName();
          
        if (MR::isEqualString(dataName, "user1"))
            return 1;
        if (MR::isEqualString(dataName, "user2"))
            return 2;
        if (MR::isEqualString(dataName, "user3"))
            return 3;

        return 0;
    }
}

void resetAllBlueCoinOnDeleteFile(SaveDataHandleSequence* pSeq, UserFile* pFile, int fileID) {
    pSeq->restoreUserFileConfigData(pFile, fileID);
    BlueCoinUtil::resetAllBlueCoinsAndSave(fileID);
}

kmCall(0x804D9BF8, resetAllBlueCoinOnDeleteFile);

void saveBlueCoinDataOnGameSave(UserFile* pFile) {
    OSReport("BLUE COINS ARE SAVING\n");
    gBlueCoinManager.SaveData(2);
    pFile->setCreated();
}

kmCall(0x804D9C90, saveBlueCoinDataOnGameSave);

void getBlueCoinData(const char* pName) {
    gBlueCoinManager.GetData(2);
    MR::startSystemSE(pName, -1, -1);
}

kmCall(0x8046D31C, getBlueCoinData);