	#pragma once

	#include "syati.h"
    #include "Game/Screen/CountUpPaneRumbler.h"
    //#include "pt/MapObj/RedCoinSystem/RedCoinCounter.h"
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
    
        LayoutActor* mRedCoinCounter;
        CountUpPaneRumbler* mRumbler;
        s32 mNumCoins;
        s32 mElapsed;
        s32 mPowerStarCheck; // Obj_arg1
        s32 mIconID; // Obj_arg2
        bool mShouldNotRewardCoins; // Obj_arg0
        bool mHasAllRedCoins;
        bool mIsValidCounterAppear;
    };