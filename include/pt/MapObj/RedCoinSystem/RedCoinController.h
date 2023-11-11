	#pragma once

	#include "syati.h"
    #include "Game/Screen/CountUpPaneRumbler.h"
    #include "pt/MapObj/RedCoinSystem/RedCoin.h"
    #include "pt/MapObj/RedCoinSystem/RedCoinLayouts.h"

    class RedCoinController : public LiveActor {
    public:
        RedCoinController(const char* pName);
    
        virtual void init(const JMapInfoIter& rIter);
        virtual void movement();
        void startCountUp(LiveActor* pRedCoin);
        //void resetAllRedCoins();
        void calcCounterVisibility();
        void calcCounterTimer();
        void calcRedCoinCounterPlayerPos();
    
        RedCoinCounter* mRedCoinCounter;
        LayoutActor* mRedCoinCounterPlayer;
        LiveActor* mLastRedCoin;
        s32 mNumCoins;
        s32 mElapsed;
        s32 mPowerStarCheck; // Obj_arg1
        s32 mIconID; // Obj_arg2

        bool mRewardCoins; // Obj_arg0
        bool mHasAllRedCoins;
        bool mIsValidCounterAppear;
    };