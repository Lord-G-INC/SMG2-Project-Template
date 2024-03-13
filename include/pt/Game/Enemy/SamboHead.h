#include "Game/Enemy/SamboHead.h"
#include "pt/MapObj/BlueCoinSystem/BlueCoin.h"

class SamboHeadExt : public SamboHead {
public:
    SamboHeadExt(const char* pName);
    BlueCoin* mBlueCoin;
    s32 mBlueCoinArg;
};