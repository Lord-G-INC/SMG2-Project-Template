	#pragma once

	#include "syati.h"
    #include "Game/Screen/CountUpPaneRumbler.h"
    #include "pt/MapObj/RedCoinSystem/RedCoin.h"
    #include "pt/MapObj/RedCoinSystem/RedCoinLayouts.h"
    #include "nw4r/ut/TextWriterBase.h"

    class RedCoinController : public LiveActor {
    public:
        RedCoinController(const char* pName);
    
        virtual void init(const JMapInfoIter& rIter);
        virtual void movement();
        void startCountUp(LiveActor* pRedCoin);
        void calcCounterVisibility();
        void calcCounterTimer();
    
        RedCoinCounter* mRedCoinCounter;
        RedCoinCounterPlayer* mRedCoinPlayerCounter;
        s32 mNumCoins;
        s32 mElapsed;

        bool mRewardCoins; // Obj_arg0
        bool mHasAllRedCoins;
        bool mIsValidCounterAppear;
    };