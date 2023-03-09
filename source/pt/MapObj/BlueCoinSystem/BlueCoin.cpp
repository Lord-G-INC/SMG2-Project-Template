#if defined (ALL) || defined (SMSS)
#include "pt/MapObj/BlueCoinSystem/BlueCoin.h"
#include "pt/MapObj/BlueCoinSystem/BlueCoinLayouts.h"
#include "Game/MapObj/CoinHolder.h"

BlueCoin::BlueCoin(const char* pName) : Coin(pName) {
    mID = 0;
    mLaunchVelocity = 175.0f;
    mIsCollected = false;
    mIsCollectedSaved = false;
    mUseConnection = false;

    MR::createCoinRotater();
    MR::createCoinHolder();
    MR::addToCoinHolder(this, this);
}

void BlueCoin::init(const JMapInfoIter& rIter) {
    MR::getJMapInfoArg0NoInit(rIter, &mID);
    MR::getJMapInfoArg1NoInit(rIter, &mLaunchVelocity);

    mIsCollectedSaved = gBlueCoinData[BlueCoinUtil::getCurrentFileNum()][mID];
    
    MR::processInitFunction(this, rIter, mIsCollectedSaved ? "BlueCoinClear" : "BlueCoin", false);
    initEffectKeeper(2, "Coin", 0);
    MR::calcGravity(this);

    initNerve(&NrvCoin::CoinNrvFix::sInstance, 0);

    initHitSensor(1);
    MR::addHitSensor(this, "BlueCoin", 0x4A, 4, 55.0f, TVec3f(0.0f, 70.0f, 0.0f));

    mFlashingCtrl = new FlashingCtrl(this, 1);

    makeActorAppeared();

    // Can't use ActorInfo for this one...
    MR::useStageSwitchSyncAppear(this, rIter);

    setShadowAndPoseModeFromJMapIter(rIter);
    initShadow(rIter);
}

void BlueCoin::initAfterPlacement() {
    if (MR::isValidSwitchB(this)) {
        MR::hideModel(this);
        MR::invalidateHitSensors(this);
    }
}

void BlueCoin::control() {
    if (MR::isOnSwitchB(this) && MR::isHiddenModel(this))
        appearAndMove();
}

bool BlueCoin::receiveMessage(u32 msg, HitSensor* pSender, HitSensor* pReciver) {
    if (MR::isMsgItemGet(msg) && !mIsCollected) {
        collect();
        return true;
    }

    return false;
}

void BlueCoin::appearAndMove() {
    // I need a better way to calculate the gravity
    TVec3f coinVelocity = TVec3f(0.0f, mLaunchVelocity / 10.0f, 0.0f);
    coinVelocity.scale(coinVelocity.y, -mGravity);

    MR::startSystemSE("SE_SY_PURPLE_COIN_APPEAR", -1, -1);
    
    appearMove(mTranslation, coinVelocity, 0x7FFFFFFF, 60);
}

void BlueCoin::collect() {
    mIsCollected = true;
    MR::startSystemSE("SE_SY_PURPLE_COIN", -1, -1);
    MR::emitEffect(this, mIsCollectedSaved ? "BlueCoinClearGet" : "BlueCoinGet");  
    
    if (MR::isValidSwitchA(this))
        MR::onSwitchA(this);

    if (!BlueCoinUtil::isBlueCoinGotCurrentFile(mID)) {
        BlueCoinUtil::setBlueCoinGotOnCurrentFile(mID, true);
        ((BlueCoinCounter*)MR::getGameSceneLayoutHolder()->mCounterLayoutController->mPTDBlueCoinCounter)->incCounter();
    }

    MR::incCoin(1, this);

    makeActorDead();
}
#endif