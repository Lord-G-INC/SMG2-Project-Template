#include "syati.h"
#ifdef GLE
#pragma message("GLE has been enabled, some functions/objects will be disabled.")
#endif

/*********************************************************************************************************************/
/* Call static initializers                                                                                          */
/*********************************************************************************************************************/
typedef void (*Func)();
extern Func __ctor_loc;
extern Func __ctor_end;

void init() {
    #ifdef GLE 
        OSReport("SMG2PT Debug GLE Build\n");
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
