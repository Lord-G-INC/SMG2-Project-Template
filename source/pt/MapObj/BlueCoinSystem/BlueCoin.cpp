#include "pt/MapObj/BlueCoinSystem/BlueCoin.h"
#include "Game/MapObj/CoinHolder.h"

BlueCoin::BlueCoin(const char* pName) : Coin(pName) {
    mID = 0;
    MR::createCoinRotater();
    MR::createCoinHolder();
    MR::addToCoinHolder(this, this);
}

void BlueCoin::init(const JMapInfoIter& rIter) {
    MR::initDefaultPos(this, rIter);
    MR::processInitFunction(this, rIter, false);
    MR::calcGravity(this);
    MR::getJMapInfoArg0NoInit(rIter, (s32*)&mID);
    mInstance = gBlueCoinManager[mID];
    initNerve(&NrvCoin::CoinNrvFix::sInstance, 0);
    initHitSensor(1);
    MR::addHitSensor(this, "BlueCoin", 0x4A, 4, 55.0f, TVec3f(0.0f, 70.0f, 0.0f));
    mFlashingCtrl = new FlashingCtrl(this, 1);
    mConnector = new MapObjConnector(this);
    mConnector->attach(mTranslation);
    makeActorAppeared();
    MR::useStageSwitchSyncAppear(this, rIter);
}

bool BlueCoin::receiveMessage(u32 msg, HitSensor* pSender, HitSensor* pReciver) {
    if (MR::isMsgItemGet(msg)) {
        mInstance->mIsCollected = true;
        // TODO: Make the coin clear instead, like a star.
        MR::hideModel(this);
    }
    return false;
}