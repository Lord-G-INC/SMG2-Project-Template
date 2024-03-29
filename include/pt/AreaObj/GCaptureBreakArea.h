#pragma once

#include "Game/AreaObj/AreaObj.h"

namespace pt {
    class GCaptureBreakArea : public AreaObj
    {
    public:
        GCaptureBreakArea(const char *pName);
        virtual ~GCaptureBreakArea();
        virtual void init(const JMapInfoIter &rIter);
        virtual void movement();
        virtual const char *getManagerName() const;

        bool mKeepPlayerMomentum;
    };
}