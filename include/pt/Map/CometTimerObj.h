#pragma once 

#include "syati.h"
#include "Game/LayoutActor/TimeLimitLayout.h"

class CometTimerObj : public LiveActor {
public:
    CometTimerObj(const char* pName);
    virtual void init(const JMapInfoIter& rIter);
    virtual void movement();
    void hideLayoutAndSwitchOn();

    TimeLimitLayout* mLayout;
    s32 mTime;
    bool mKillPlayer;
    bool mIsAppeared;
};