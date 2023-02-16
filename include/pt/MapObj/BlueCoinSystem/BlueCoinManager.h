#pragma once
#include "syati.h"

struct BlueCoinInstance {
    u8 mID;
    bool mIsCollected;
};

class BlueCoinManager {
    u32 mSize;
    BlueCoinInstance* mInstances;
    public:
    BlueCoinManager(u32 mSize) : mSize(mSize) { 
        OSReport("BlueCoinManager.h: mSize is %d\n", mSize);
        mInstances = new BlueCoinInstance[mSize];
        memset(mInstances, 0, sizeof(BlueCoinInstance)*mSize);
        u16 id = 0;
        for (int i = 0; i < mSize; i++) {
            mInstances[i].mID = (id + 1) == 255 ? 254 : id++;
        }
    }
    BlueCoinManager() {}
    ~BlueCoinManager() {
        delete [] mInstances;
    }
    BlueCoinInstance* operator[](u8 pos) {
        return &mInstances[pos];
    }
    void SaveData(u8);
    void GetData(u8);
};

static BlueCoinManager gBlueCoinManager;