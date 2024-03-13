#include "Game/Enemy/Kuribo.h"
#include "pt/MapObj/BlueCoinSystem/BlueCoin.h"

class KuriboExt : public Kuribo {
public:
    KuriboExt(const char* pName);
    BlueCoin* mBlueCoin;
    s32 mBlueCoinArg;
};