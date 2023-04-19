	#pragma once

	#include "syati.h"
    #include "Game/Screen/CountUpPaneRumbler.h"
    #include "pt/MapObj/RedCoinSystem/RedCoin.h"

    class RedCoinController : public LiveActor {
    public:
        RedCoinController(const char* pName);
    
        virtual void init(const JMapInfoIter& rIter);
        virtual void movement();
        void incCountAndUpdateLayouts();
        void resetAllRedCoins();
        void appearCounterIfHidden();
        void updateCounter();
        void calcCounterVisibility();
        void calcCounterTimer();
    
        LayoutActor* mRedCoinCounter;
        CountUpPaneRumbler* mRumbler;
        s32 mNumCoins;
        s32 mElapsed;
        s32 mPowerStarCheck; // Obj_arg1
        s32 mIconID; // Obj_arg

        s32 mTimerModeMinutes; // Obj_arg4
        s32 mTimerModeSeconds; // Obj_arg5

        s32 mTimerMinutes;
        s32 mTimerSeconds;
        s32 mTimerMilliseconds;

        bool mShouldNotRewardCoins; // Obj_arg0
        bool mHasAllRedCoins;
        bool mIsValidCounterAppear;
        bool mIsTimerMode; // Obj_arg3
        bool mIsUp;
    };