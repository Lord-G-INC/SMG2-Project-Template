#pragma once

#include "syati.h"
#include "Game/Screen/CountUpPaneRumbler.h"

class BlueCoinCounter : public LayoutActor {
public:
        BlueCoinCounter(const char* pName);
        
        virtual void init(const JMapInfoIter& rIter);
        //virtual void control();

        CountUpPaneRumbler* mPaneRumbler;
};