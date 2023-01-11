	#pragma once

	#include "syati.h"
    #include "pt/MapObj/RedCoinSystem/RedCoinLayouts.h"
    #include "pt/MapObj/RedCoinSystem/RedCoin.h"

    class RedCoinController : public LiveActor {
    public:
        RedCoinController(const char* pName);
    
        virtual void init(const JMapInfoIter& rIter);
        virtual void movement();
        void incCountAndUpdateLayouts(LiveActor* pActor);
        void resetAllRedCoins();
    
        RedCoinCounter* mRedCoinCounter;
        s32 mNumCoins;
        s32 mElapsed;
        s32 mPowerStarCheck; // Obj_arg1
        s32 mIconID; // Obj_arg2
        bool mShouldNotRewardCoins; // Obj_arg0
        bool mHasAllRedCoins;
    };
    
    RedCoinController* getRedCoinControllerFromGroup(LiveActor* actor);