#include "pt/MapObj/BlueCoinSystem/BlueCoin.h"
#include "Game/MapObj/CoinHolder.h"

BlueCoin::BlueCoin(const char* pName) : Coin(pName) {
    mID = 0;
    mIsCollected = 0;

    MR::createCoinRotater();
    MR::createCoinHolder();
    MR::addToCoinHolder(this, this);
}

void BlueCoin::init(const JMapInfoIter& rIter) {
    MR::getJMapInfoArg0NoInit(rIter, &mID);
    
    MR::initDefaultPos(this, rIter);
    MR::processInitFunction(this, rIter, "BlueCoin", false);
    MR::connectToSceneMapObjStrongLight(this);
    MR::calcGravity(this);
    MR::initShadowVolumeCylinder(this, 50.0f);

    initNerve(&NrvCoin::CoinNrvFix::sInstance, 0);

    initHitSensor(1);
    MR::addHitSensor(this, "BlueCoin", 0x4A, 4, 55.0f, TVec3f(0.0f, 70.0f, 0.0f));

    mFlashingCtrl = new FlashingCtrl(this, 1);

    mConnector = new MapObjConnector(this);
    mConnector->attach(mTranslation);

    makeActorAppeared();

    // Can't use ActorInfo for this one...
    MR::useStageSwitchSyncAppear(this, rIter);
}

bool BlueCoin::receiveMessage(u32 msg, HitSensor* pSender, HitSensor* pReciver) {
    if (MR::isMsgItemGet(msg) && !mIsCollected) {
        collect();
        return true;
    }

    if (mIsCollected && resetted) {
        OSReport("%u\n", gBlueCoinData[0][1]);
    }
    return false;
}

void BlueCoin::collect() {
    mIsCollected = true;
    //OSReport("COIN %d: %d\n", mID, gBlueCoinManager[80]->mIsCollected);
    MR::incCoin(1, this);
    makeActorDead();
}