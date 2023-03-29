#pragma once

#include "syati.h"

class PTimerSwitch : public  LiveActor {
public:

    PTimerSwitch(const char* pName);
    void reset();
    virtual void init(const JMapInfoIter& rIter);
    virtual void initAfterPlacement();
    virtual void appear();
    virtual void kill();
    virtual void calcAnim();
    virtual void control();
    virtual void calcAndSetBaseMtx();
    virtual bool receiveMsgPlayerAttack(u32 msg, HitSensor* pSender, HitSensor* pReceiver);
    virtual bool receiveOtherMsg(u32 msg, HitSensor* pSender, HitSensor* pReceiver);
    bool trySwitchDown();
    bool tryOn();
    void exeOff();
    void exeSwitchDown();
    void exeOn();

    MapObjConnector* mConnector;
    s32 _94;
    bool _98;
};

namespace NrvPTimerSwitch {
    NERVE(PTimerSwitchNrvOn);
    NERVE(PTimerSwitchNrvSwitchDown);
    NERVE(PTimerSwitchNrvOff);
};