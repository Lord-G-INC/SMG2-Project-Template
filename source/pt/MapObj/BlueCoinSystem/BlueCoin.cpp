#include "pt/MapObj/BlueCoinSystem/BlueCoin.h"
#include "Game/MapObj/CoinHolder.h"

BlueCoin::BlueCoin(const char* pName) : Coin(pName) {
    mID = 0;
    mIsCollected = false;
    mIsCollectedSaved = false;

    MR::createCoinRotater();
    MR::createCoinHolder();
    MR::addToCoinHolder(this, this);
}

void BlueCoin::init(const JMapInfoIter& rIter) {
    MR::getJMapInfoArg0NoInit(rIter, &mID);
    mIsCollectedSaved = gBlueCoinData[BlueCoinUtil::getCurrentFileNum()][mID];
    OSReport("COIN INIT %d: %d, FILE NUM: %d\n", mID, gBlueCoinData[1][mID], BlueCoinUtil::getCurrentFileNum());
    
    MR::initDefaultPos(this, rIter);
    MR::processInitFunction(this, rIter, mIsCollectedSaved ? "BlueCoinClear" : "BlueCoin", false);
    initEffectKeeper(2, "Coin", 0);
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

    return false;
}

void BlueCoin::collect() {
    mIsCollected = true;
    gBlueCoinData[BlueCoinUtil::getCurrentFileNum()][mID] = true;
    MR::emitEffect(this, mIsCollectedSaved ? "BlueCoinClearGet" : "BlueCoinGet");
    BlueCoinUtil::setBlueCoinGotOnCurrentFile(mID, true);
    MR::incCoin(1, this);
    makeActorDead();
}