#include "syati.h"
#include "pt/MapObj/BlueCoinSystem/BlueCoinUtil.h"
#include "Game/Util.h"

//bool** gBlueCoinData;


/*********************************************************************************************************************/
/* Call static initializers                                                                                          */
/*********************************************************************************************************************/
typedef void (*Func)();
extern Func __ctor_loc;
extern Func __ctor_end;

void init() {
    const char* name = "Unknown"; 

    #ifdef ALL
    name = "ALL";
    #elif SMSS
    name = "SMSS (GLE)";
    #elif CA
    name = "CA (GLE)";
    #elif SMG63
    name = "SMG63 (GLE)";
    #elif GLE
    name = "GLE";
    #elif NOGLE
    name = "NOGLE";
    #endif

    OSReport("SMG2PTD INIT: Created by Evanbowl and the Lord-G Inc Team\nBuilt: %s, %s\nTarget: %s\n", __DATE__, __TIME__, name);

    #ifdef SMSS
        BlueCoinUtil::initBlueCoinArray();
    #endif

    for (Func* f = &__ctor_loc; f < &__ctor_end; f++) {
        (*f)();
    }


}

#if defined(TWN) || defined(KOR)
    kmBranch(0x804B6A64, init);
#else
    kmBranch(0x804B69F4, init);
#endif