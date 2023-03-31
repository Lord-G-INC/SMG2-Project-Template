#pragma once

#include "syati.h"

class LavaSteam : public LiveActor {
public:
    LavaSteam(const char* pName);
    virtual void init(const JMapInfoIter& rIter);
    virtual void initAfterPlacement();
    virtual void attackSensor(HitSensor* pSender, HitSensor* pReceiver);
    virtual void startClipped();
    virtual void endClipped();
    void startSteam();
    void exeWait();
    void exeSteam();

    TVec3f _90;
    TVec3f _9C;
};

namespace NrvLavaSteam {
    NERVE(HostTypeSteamEnd);
    NERVE(HostTypeSteam);
    NERVE(HostTypeWait);
    NERVE(HostTypeWaitForSwitchOn);
};