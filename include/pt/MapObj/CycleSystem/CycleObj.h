#pragma once

#include "syati.h"

class CycleObj : public LiveActor {
public:
    CycleObj(const char *pName);

    virtual void init(const JMapInfoIter &rIter);

    void activate();
    void deactivate();

    const char *getBgmName();

    s32 cycleObjIndex;
    s32 stageBgmInfoIndex;
    s32 bgmChannel;
    s32 musicFadeTime;

};