#if defined (ALL) || defined (SMSS)
#pragma once
#include "syati.h"
#include "pt/MapObj/BlueCoinSystem/BlueCoinUtil.h"

class BlueCoin : public Coin {
    public:
    BlueCoin(const char*);
    virtual void init(const JMapInfoIter&);
    virtual void initAfterPlacement();
    virtual void control();
    virtual bool receiveMessage(u32 msg, HitSensor* pSender, HitSensor* pReceiver);
    void appearAndMove();
    void collect();

    s32 mID;
    f32 mLaunchVelocity;
    bool mIsCollected;
    bool mIsCollectedSaved;
    bool mUseConnection;
};
#endif