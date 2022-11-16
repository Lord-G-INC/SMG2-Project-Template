#pragma once

#include "syati.h"
#include "revolution.h"
#include "Game/MapObj/Coin.h"
#include "Game/LiveActor/FlashingCtrl.h"
#include "Game/System/CountUpPaneRumbler.h"
#include "Game/Screen/CounterLayoutAppearer.h"

class RedCoin; // bruh


/* --- RED COIN COUNTER--- */

class RedCoinCounter : public LayoutActor {
public:
    RedCoinCounter(const char* pName);
    
    virtual void init(const JMapInfoIter& rIter);

    CountUpPaneRumbler* mCountUpPaneRumbler;
};



/* --- RED COIN COUNTER PLAYER --- */

class RedCoinCounterPlayer : public LayoutActor {
public:
    RedCoinCounterPlayer(const char* pName, RedCoin* pRedCoin);
    
    virtual void init(const JMapInfoIter& rIter);
    virtual void control();

    RedCoin* mRedCoin;
    bool mLytPos;
};



/* --- RED COIN CONTROLLER --- */

class RedCoinController : public LiveActor {
public:
    RedCoinController(const char* pName);

    virtual void init(const JMapInfoIter& rIter);
    virtual void movement();
    void incCountAndUpdateLayouts(RedCoin* rRedCoin);

    s32 mNumCoins;
    s32 mElapsed;
    RedCoinCounter* mRedCoinCounter;
    bool mHasAllRedCoins;
    bool mCounterPlayerLayoutMode;

};

/* --- RED COIN --- */

class RedCoin : public Coin {
public:
    RedCoin(const char* pName);
    void collect();
    virtual void init(const JMapInfoIter& rIter);
    virtual bool receiveMessage(u32 msg, HitSensor* pSender, HitSensor* pReceiver);

    RedCoinController* mCoinController;
    RedCoinCounterPlayer* mCoinCounterPlayer;
    LiveActorGroup* mGroup;
    bool mIsCollected;
    bool mIsSpawnedCoin;
};

namespace NrvRedCoin {
    NERVE(NrvInit);
}



/* --- RED COIN APPEARER --- */

class RedCoinAppearer : public LiveActor {
public:
    RedCoinAppearer(const char* pName);

    virtual void init(const JMapInfoIter& rIter);
    virtual void movement();

    RedCoin* mRedCoin;
    f32 mLaunchVelocity;
    bool mHasSpawned;
};

RedCoinController* getRedCoinController(LiveActor* actor);

