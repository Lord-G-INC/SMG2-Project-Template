#pragma once

#include "syati.h"
#include "Game/LayoutActor/TimeLimitLayout.h"

class RedCoinSwitch : public LiveActor {
public:
    RedCoinSwitch(const char* pName);
    virtual void init(const JMapInfoIter& rIter);
    virtual bool receiveMessage(u32 msg, HitSensor* pSender, HitSensor* pReceiver);
    virtual void control();
    void exeOn();

    TimeLimitLayout* mTimeLimitLayout;
    bool isOn;
    s32 mTime;
    s32 mCollectedPowerStar;
};