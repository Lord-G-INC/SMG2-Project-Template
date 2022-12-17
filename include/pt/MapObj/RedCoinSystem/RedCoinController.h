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
        void calcCounterVisibilty();
        void resetAllRedCoins();
    
        RedCoinCounter* mRedCoinCounter;
        s32 mNumCoins;
        s32 mElapsed;
        bool mHasAllRedCoins;
        bool mCounterPlayerLayoutMode; // Obj_arg0
        bool mShouldNotRewardCoins; // Obj_arg1
        bool mLayoutAnimType; // Obj_arg2
        s32 mPowerStarCheck; // Obj_arg3
        s32 mIconID; // Obj_arg4
    };
    
    RedCoinController* getRedCoinControllerFromGroup(LiveActor* actor);