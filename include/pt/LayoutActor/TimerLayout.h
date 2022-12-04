#pragma once

#include "Game/Screen/LayoutActor.h"
#include "syati.h"
#include "Game/Screen/GameSceneLayoutHolder.h"

#ifdef ALL
namespace pt {
    class TimerLayout : public LayoutActor {
    public:
    	TimerLayout();

    	virtual void init(const JMapInfoIter &rIter);
    	virtual void control();
		void exeAppear();
		void exeDisappear();
		void exeOnStarGet();

    	wchar_t* currTimeDisplay;
    	wchar_t* savedTimeDisplay;
    	wchar_t* newRecordDisplay;
    	u32 currTime;
    	u32 savedTime;
    	u32 frameDifference;
    };

	namespace NrvTimerLayout {
		NERVE(NrvWait);
		NERVE(NrvAppear);
		NERVE(NrvDisappear);
		NERVE(NrvOnStarGet);
	}

}
#endif