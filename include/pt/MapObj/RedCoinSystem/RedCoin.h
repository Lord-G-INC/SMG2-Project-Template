#pragma once

#include "syati.h"
#include "pt/MapObj/RedCoinSystem/RedCoinController.h"
#include "pt/MapObj/RedCoinSystem/RedCoinLayouts.h"
#include "revolution.h"
#include "Game/MapObj/Coin.h"
#include "Game/LiveActor/FlashingCtrl.h"
#include "Game/Screen/CountUpPaneRumbler.h"


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
    
        RedCoinCounterPlayer* mCoinCounterPlayer;
        LiveActorGroup* mGroup;
        f32 mLaunchVelocity; // Obj_arg0
        bool mUseConnection; // Obj_arg1
        bool mIsCollected;
        bool mIsInAirBubble;
        bool mInvalidateShadows;
        bool mCounterPlayerPos;
        bool mHasRewardedCoins;
    };