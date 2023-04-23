#ifdef SMG63
    #pragma once

    #include "syati.h"

    class MarioDisappointmentArea : public AreaObj {
    public:
        MarioDisappointmentArea(const char* pName);
        virtual void init(const JMapInfoIter& rIter);
        virtual void movement();
        virtual const char* getManagerName() const;
    };
#endif
