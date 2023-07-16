#ifdef USEBLUECOIN
#include "pt/MapObj/BlueCoinSystem/BlueCoin.h"
#include "Game/MapObj/CoinHolder.h"

BlueCoin::BlueCoin(const char* pName) : Coin(pName) {
    mID = 0;
    mLaunchVelocity = 250.0f;
    mUseConnection = false;

    mIsPurple = false;
    mShadowCalcOn = true;
    mIgnoreGravity = false;
    mCalcShadowPrivateGravity = true;

    MR::createCoinRotater();
    MR::createCoinHolder();
    MR::addToCoinHolder(this, this);
}

void BlueCoin::init(const JMapInfoIter& rIter) {
    #ifdef SM64BLUECOIN
    MR::getJMapInfoArg0NoInit(rIter, &mLaunchVelocity);
    MR::getJMapInfoArg1NoInit(rIter, &mUseConnection);
    MR::processInitFunction(this, rIter, "BlueCoin", false);
    #else
    MR::getJMapInfoArg0NoInit(rIter, &mID);
    MR::getJMapInfoArg1NoInit(rIter, &mLaunchVelocity);
    MR::getJMapInfoArg2NoInit(rIter, &mUseConnection);
    MR::processInitFunction(this, rIter, BlueCoinUtil::isBlueCoinGotCurrentFile(mID) ? "BlueCoinClear" : "BlueCoin", false);
    #endif

    initEffectKeeper(2, "Coin", 0);
    MR::calcGravity(this);

    initNerve(&NrvCoin::CoinNrvFix::sInstance, 0);
    initHitSensor(1);
    MR::addHitSensor(this, "BlueCoin", 0x4A, 4, 55.0f, TVec3f(0.0f, 70.0f, 0.0f));

    MR::initShadowVolumeSphere(this, 50.0f);
    MR::setShadowDropPositionPtr(this, 0, &mShadowDropPos);
    MR::setShadowDropLength(this, 0, 1000.0f);

    mConnector = new MapObjConnector(this);

    mFlashingCtrl = new FlashingCtrl(this, 1);

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

    MR::startSystemSE("SE_SY_TIMECOIN_APPEAR", -1, -1);
    
    appearMove(mTranslation, coinVelocity, 0x7FFFFFFF, 60);
}

void BlueCoin::collect() {
    if (MR::isValidSwitchA(this))
        MR::onSwitchA(this);

    #ifdef SM64BLUECOIN
    MR::emitEffect(this, "BlueCoinGet"); 
    #else
    MR::emitEffect(this, BlueCoinUtil::isBlueCoinGotCurrentFile(mID) ? "BlueCoinClearGet" : "BlueCoinGet");  

    if (!BlueCoinUtil::isBlueCoinGotCurrentFile(mID)) {
        BlueCoinUtil::setBlueCoinGotCurrentFile(mID);
        BlueCoinUtil::startCounterCountUp();
    }
    MR::startSystemSE("SE_SY_BLUE_COIN_GET", -1, -1); 
    #endif

    if (!MR::isGalaxyDarkCometAppearInCurrentStage()) {
        #ifdef SM64BLUECOIN
        MR::incCoin(5, this);
        #else
        MR::incCoin(1, this);
        #endif
    }

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