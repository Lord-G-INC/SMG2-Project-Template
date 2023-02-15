#pragma once

#include "syati.h"

class DisplayStar : public LiveActor {
public:
    DisplayStar(const char* pName);
    virtual void init(const JMapInfoIter& rIter);
    virtual void control();
    void setupColor(s32 scenario);
    
    TMtx34f mMtx;
    s32 mScenario;
    s32 mFrame;
    bool mIsGrandStar;
    bool mIsRotate;
};