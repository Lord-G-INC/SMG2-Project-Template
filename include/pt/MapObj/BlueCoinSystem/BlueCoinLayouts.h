#if defined (ALL) || defined (SMSS)
#pragma once

#include "syati.h"
#include "Game/Screen/CountUpPaneRumbler.h"

const wchar_t counterPictureFonts[] = {
0x000E, 0x0003, 0x0055, 0x0002, 0x005A, // Blue Coin Icon
0x000E, 0x0003, 0x0010, 0x0002, 0x0010, // X Icon
0x000E, 0x0006, 0x0001, 0x0008, 0x0000, // Counter
0x0000, 0x0000, 0x0000
};


class BlueCoinCounter : public LayoutActor {
public:
        BlueCoinCounter(const char* pName);
        
        virtual void init(const JMapInfoIter& rIter);
        virtual void control();
        void exeAppear();
        void exeDisappear();
        void incCounter();

        CountUpPaneRumbler* mPaneRumbler;
};
#endif