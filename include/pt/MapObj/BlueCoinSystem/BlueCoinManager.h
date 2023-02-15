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
        mInstances = new BlueCoinInstance[mSize];
        memset(mInstances, 0, sizeof(BlueCoinInstance)*mSize);
        u8 id = 0;
        for (int i = 0; i < mSize; i++) {
            mInstances[i].mID = (id + 1) == 0 ? 255 : id++;
        }
    }
    ~BlueCoinManager() {
        delete [] mInstances;
    }
    BlueCoinInstance* operator[](u8 pos) {
        return &mInstances[pos];
    }
};

static BlueCoinManager gBlueCoinManager = BlueCoinManager(255);