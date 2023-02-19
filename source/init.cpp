#include "syati.h"
#include "pt/MapObj/BlueCoinSystem/BlueCoinManager.h"

u8* datas[3];
bool resetted = false;


/*********************************************************************************************************************/
/* Call static initializers                                                                                          */
/*********************************************************************************************************************/
typedef void (*Func)();
extern Func __ctor_loc;
extern Func __ctor_end;

void init() {
    OSReport("SMG2 PT Debug by Evanbowl and the Lord-G INC team.\n");

    for (s32 i = 0; i < 3; i++) {
        u8* ptr = new u8[510];
        memset(ptr, 69, 510);
        datas[i] = ptr;
    }

    #if defined(BUILD_MONTH) && defined(BUILD_DAY) && defined(BUILD_YEAR)
        int m = BUILD_MONTH;
        int d = BUILD_DAY;
        int y = BUILD_YEAR;
        OSReport("Built on %d/%d/%d\n",m,d,y);
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