#pragma once

#include "syati.h"
#include "Game/Screen/CountUpPaneRumbler.h"

class RedCoinCounter : public LayoutActor {
public:
    RedCoinCounter(const char* pName);
    virtual void init(const JMapInfoIter& rIter);
    //virtual void appear();
    virtual void control();
    void setStarIcon(s32 starID, s32 iconID);
    void startCountUp(s32 hasAllCoins);
    void exeAppear();
    void exeAppearWithUpdate();
    void exeDisappear();
    void exeCountUp();
    void exeCountUpComplete();

    CountUpPaneRumbler* mPaneRumbler;
    s32 mRedCoinCount;
};

namespace NrvRedCoinCounter {
    NERVE(NrvAppear);
    NERVE(NrvAppearWithUpdate);
    NERVE(NrvDisappear);
    NERVE(NrvCountUp);
    NERVE(NrvCountUpComplete);
    NERVE(NrvHide); 
};