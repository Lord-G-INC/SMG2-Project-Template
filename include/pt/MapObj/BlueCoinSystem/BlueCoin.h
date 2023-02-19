#pragma once
#include "syati.h"
#include "pt/MapObj/BlueCoinSystem/BlueCoinManager.h"

class BlueCoin : public Coin {
    BlueCoinInstance* mInstance;
    public:
    BlueCoin(const char*);
    virtual void init(const JMapInfoIter&);
    virtual bool receiveMessage(u32 msg, HitSensor* pSender, HitSensor* pReceiver);
    void collect();

    s32 mID;
    bool mIsCollected;
};