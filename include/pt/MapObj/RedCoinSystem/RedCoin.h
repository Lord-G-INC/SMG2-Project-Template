#pragma once

#include "syati.h"
#include "pt/MapObj/RedCoinSystem/RedCoinController.h"
#include "revolution.h"
#include "Game/MapObj/Coin.h"
#include "Game/LiveActor/FlashingCtrl.h"
#include "Game/Screen/CountUpPaneRumbler.h"


    /* --- RED COIN --- */
    
    class RedCoin : public Coin {
    public:
        RedCoin(const char* pName);
        virtual void init(const JMapInfoIter& rIter);
        virtual void initAfterPlacement();
        virtual void control();
        virtual void calcAndSetBaseMtx();
        virtual bool receiveMessage(u32 msg, HitSensor* pSender, HitSensor* pReceiver);

        void collect();
        void appearAndMove();
        void initAirBubble();
    
        LiveActorGroup* mGroup;
        f32 mLaunchVelocity; // Obj_arg0
        s32 mAppearDelay; // Obj_arg4
        s32 mElapsed;
        bool mIsInAirBubble; // Obj_arg2
        bool mInvalidateShadows; // Obj_arg3
        bool mIsCollected;
        bool mHasRewardedCoins;
        bool mRedCoinCounterPlayerPos;
    };