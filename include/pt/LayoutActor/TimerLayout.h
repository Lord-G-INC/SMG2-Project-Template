#pragma once

#include "Game/Screen/LayoutActor.h"
#include "syati.h"

namespace pt {
    class TimerLayout : public LayoutActor {
    public:
    	TimerLayout();

    	virtual void init(const JMapInfoIter &rIter);
    	virtual void control();

    	wchar_t* currTimeDisplay;
    	wchar_t* savedTimeDisplay;
    	wchar_t* newRecordDisplay;
    	u32 currTime;
    	u32 savedTime;
    	u32 frameDifference;
    };
}