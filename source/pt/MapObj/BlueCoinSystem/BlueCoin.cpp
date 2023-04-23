#ifdef SMSS

/*
*/
#include "pt/MapObj/BlueCoinSystem/BlueCoin.h"
#include "pt/MapObj/BlueCoinSystem/BlueCoinLayouts.h"
#include "Game/MapObj/CoinHolder.h"

BlueCoin::BlueCoin(const char* pName) : Coin(pName) {
    mID = 0;
    mLaunchVelocity = 250.0f;
    mUseConnection = false;

    MR::createCoinRotater();
    MR::createCoinHolder();
    MR::addToCoinHolder(this, this);
}

void BlueCoin::init(const JMapInfoIter& rIter) {
    MR::getJMapInfoArg0NoInit(rIter, &mID);
    MR::getJMapInfoArg1NoInit(rIter, &mLaunchVelocity);
    MR::getJMapInfoArg2NoInit(rIter, &mUseConnection);
    
    MR::processInitFunction(this, rIter, BlueCoinUtil::isBlueCoinGotCurrentFile(mID) ? "BlueCoinClear" : "BlueCoin", false);
    initEffectKeeper(2, "Coin", 0);
    MR::calcGravity(this);

    initNerve(&NrvCoin::CoinNrvFix::sInstance, 0);

    initHitSensor(1);
    MR::addHitSensor(this, "BlueCoin", 0x4A, 4, 55.0f, TVec3f(0.0f, 70.0f, 0.0f));

    setShadowAndPoseModeFromJMapIter(rIter);
    initShadow(rIter);

    mConnector = new MapObjConnector(this);

    mFlashingCtrl = new FlashingCtrl(this, 1);

    if (mUseConnection)
        MR::invalidateClipping(this);
    else
        MR::setClippingFarMax(this);

    makeActorAppeared();

    // Can't use ActorInfo for this one...
    MR::useStageSwitchSyncAppear(this, rIter);
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

void BlueCoin::calcAndSetBaseMtx() {
    if (mUseConnection) {
        mConnector->connect();
        mConnector->attachToUnder();
    }

    Coin::calcAndSetBaseMtx();
}

bool BlueCoin::receiveMessage(u32 msg, HitSensor* pSender, HitSensor* pReciver) {
    if (MR::isMsgItemGet(msg)) {
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
    MR::startSystemSE("SE_SY_PURPLE_COIN", -1, -1);
    MR::emitEffect(this, BlueCoinUtil::isBlueCoinGotCurrentFile(mID) ? "BlueCoinClearGet" : "BlueCoinGet");  
    
    if (MR::isValidSwitchA(this))
        MR::onSwitchA(this);

    if (!BlueCoinUtil::isBlueCoinGotCurrentFile(mID)) {
        BlueCoinUtil::setBlueCoinGotCurrentFile(mID);
        ((BlueCoinCounter*)MR::getGameSceneLayoutHolder()->mCounterLayoutController->mPTDBlueCoinCounter)->startCountUp();
    }

    if (!MR::isGalaxyDarkCometAppearInCurrentStage())
        MR::incCoin(1, this);

    makeActorDead();
}

bool appearCustomCoinOnDarkComet() {
    const char* name;

    asm("lwz %0, 0x4(r31)" : "=r" (name));

    if (MR::isGalaxyDarkCometAppearInCurrentStage() && !MR::isEqualString(name, "RedCoin") && !MR::isEqualString(name, "BlueCoin"))
        return true;
    
    return false;
}

kmCall(0x8028C2EC, appearCustomCoinOnDarkComet);
#endif