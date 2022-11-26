#pragma once

#include "syati.h"
#include "revolution.h"
#include "Game/MapObj/Coin.h"
#include "Game/LiveActor/FlashingCtrl.h"
#include "Game/System/CountUpPaneRumbler.h"
#include "Game/Screen/CounterLayoutAppearer.h"

class RedCoinCounter;
class RedCoinCounterPlayer;
class RedCoinController;


/* --- RED COIN --- */

class RedCoin : public Coin {
public:
    RedCoin(const char* pName);
    virtual void init(const JMapInfoIter& rIter);
    virtual void control();
    virtual void calcAndSetBaseMtx();
    virtual bool receiveMessage(u32 msg, HitSensor* pSender, HitSensor* pReceiver);

    void collect();
    void appearAndMove();
    void initAirBubble();

    RedCoinController* mCoinController;
    RedCoinCounterPlayer* mCoinCounterPlayer;
    LiveActorGroup* mGroup;
    f32 mLaunchVelocity; // Obj_arg0
    bool mUseConnection; // Obj_arg1
    bool mIsCollected;
    bool mIsInAirBubble;
};

/* --- RED COIN CONTROLLER --- */

class RedCoinController : public LiveActor {
public:
    RedCoinController(const char* pName);

    virtual void init(const JMapInfoIter& rIter);
    virtual void movement();
    void incCountAndUpdateLayouts(RedCoin* rRedCoin);

    RedCoinCounter* mRedCoinCounter;
    s32 mNumCoins;
    s32 mElapsed;
    bool mHasAllRedCoins;
    bool mCounterPlayerLayoutMode; // Obj_arg0
    bool mShouldNotRewardCoins; // Obj_arg1
    bool mLayoutAnim;
};


/* --- RED COIN COUNTER--- */

class RedCoinCounter : public LayoutActor {
public:
    RedCoinCounter(const char* pName);
    
    virtual void init(const JMapInfoIter& rIter);
    virtual void control();
    virtual void appear();
    void updateCounter(s32 count, bool hasAllCoins);

    CountUpPaneRumbler* mPaneRumbler;
};



/* --- RED COIN COUNTER PLAYER --- */

class RedCoinCounterPlayer : public LayoutActor {
public:
    RedCoinCounterPlayer(const char* pName, RedCoin* pRedCoin);
    
    virtual void init(const JMapInfoIter& rIter);
    virtual void control();
    void updateCounter(s32 count, bool layoutPos, bool layoutAnim);

    RedCoin* mRedCoin;
    bool mLytPos;
};

RedCoinController* getRedCoinControllerFromGroup(LiveActor* actor);

