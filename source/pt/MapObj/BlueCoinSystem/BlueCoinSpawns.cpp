#if defined USEBLUECOIN && !defined SM64BLUECOIN
#include "syati.h"
#include "Game/Enemy/Kuribo.h"
#include "Game/Enemy/SamboHead.h"

// KURIBO

kmWrite32(0x801A2C8C, 0x386000D8); // li r3, 0xD8
kmWrite32(0x801E4B34, 0x386000D8); // li r3, 0xD8
kmWrite32(0x802B4934, 0x386000D8); // li r3, 0xD8
kmWrite32(0x8033D4C4, 0x386000D8); // li r3, 0xD8

void KuriboSetUpBlueCoin(Kuribo* pKuribo, const JMapInfoIter& rIter, const char* pStr) {
    pKuribo->mBlueCoinArg = -1;
    MR::getJMapInfoArg2NoInit(rIter, &pKuribo->mBlueCoinArg);

    MR::processInitFunction(pKuribo, rIter, pStr, 0);

    pKuribo->mBlueCoin = (BlueCoin*)BlueCoinUtil::createBlueCoinForSpawning(pKuribo, pKuribo->mBlueCoinArg);
}

kmCall(0x801B8A68, KuriboSetUpBlueCoin);

void KuriboAppearBlueCoin(Kuribo* pKuribo, const char* pStr) {
    MR::emitEffect(pKuribo, pStr);

    if (pKuribo->mBlueCoin)
        BlueCoinUtil::appearBlueCoin(pKuribo, pKuribo->mBlueCoin);
}

kmCall(0x801B8EC8, KuriboAppearBlueCoin);

// SAMBOHEAD

kmWrite32(0x8033DC44, 0x386000C4); // li r3, 0xC4

void SamboHeadSetUpBlueCoin(SamboHead* pSamboHead, const JMapInfoIter& rIter, const char* pStr) {
    pSamboHead->mBlueCoinArg = -1;
    MR::getJMapInfoArg1NoInit(rIter, &pSamboHead->mBlueCoinArg);

    MR::processInitFunction(pSamboHead, rIter, pStr, 0);
    pSamboHead->mBlueCoin = (BlueCoin*)BlueCoinUtil::createBlueCoinForSpawning(pSamboHead, pSamboHead->mBlueCoinArg);
}

kmCall(0x801F8024, SamboHeadSetUpBlueCoin);

void SamboHeadAppearBlueCoin(SamboHead* pSamboHead, const char* pStr) {
    MR::emitEffect(pSamboHead, pStr);

    if (pSamboHead->mBlueCoin)
        BlueCoinUtil::appearBlueCoin(pSamboHead, pSamboHead->mBlueCoin);

}

kmCall(0x801F81D4, SamboHeadAppearBlueCoin);

#endif