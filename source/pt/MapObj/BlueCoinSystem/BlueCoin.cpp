#ifdef USEBLUECOIN
#include "pt/MapObj/BlueCoinSystem/BlueCoin.h"
#include "Game/MapObj/CoinHolder.h"

BlueCoin::BlueCoin(const char* pName) : Coin(pName) {
    mCoinHostInfo = 0;
    mFlashingCtrl = 0;
	mAirBubble = 0; 
	mConnector = 0;
	mShadowDropPos = TVec3f(0.0f, 0.0f, 0.0f);
	_AC = TVec3f(0.0f, 0.0f, 0.0f);
	_B8 = TVec3f(0.0f, 0.0f, 0.0f);
	mLifeTime = 600;
	mCannotTime = 0;
	mShadowType = -1;
	mInWater = 0;
	mShadowCalcOn = 0;
	mIgnoreGravity = 0;
	mCalcShadowPrivateGravity = 0;
	mIsPurple = 0;
	mIsInBubble = 0;

    mID = 0;
    mLaunchVelocity = 250.0f;

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
    if (MR::isValidInfo(rIter)) {
        MR::getJMapInfoArg0NoInit(rIter, &mID);
        MR::getJMapInfoArg1NoInit(rIter, &mLaunchVelocity);
        Coin::setShadowAndPoseModeFromJMapIter(rIter);
        Coin::initShadow(rIter);

        MR::processInitFunction(this, rIter, BlueCoinUtil::isBlueCoinGotCurrentFile(mID) ? "BlueCoinClear" : "BlueCoin", false);
        MR::setGroupClipping(this, rIter, 64);
    }
    else
        MR::processInitFunction(this, BlueCoinUtil::isBlueCoinGotCurrentFile(mID) ? "BlueCoinClear" : "BlueCoin", false);
    #endif

    initEffectKeeper(2, "Coin", 0);
    MR::calcGravity(this);

    initNerve(&NrvCoin::CoinNrvFix::sInstance, 0);
    initHitSensor(1);

    #ifdef SMG63
        MR::addHitSensor(this, "BlueCoin", 0x4A, 4, 120.0f, TVec3f(0.0f, 0.0f, 0.0f));
    #else
        MR::addHitSensor(this, "BlueCoin", 0x4A, 4, 55.0f, TVec3f(0.0f, 70.0f, 0.0f));
    #endif

    MR::initShadowVolumeSphere(this, 50.0f);
    MR::setShadowDropPositionPtr(this, 0, &mShadowDropPos);
    MR::setShadowDropLength(this, 0, 1000.0f);

    mFlashingCtrl = new FlashingCtrl(this, 1);

    makeActorAppeared();

    if (MR::isValidInfo(rIter))
        MR::useStageSwitchSyncAppear(this, rIter);
}

void BlueCoin::initAfterPlacement() {
    if (MR::isValidSwitchB(this)) {
        MR::hideModel(this);
        MR::invalidateHitSensors(this);
    }
    else
        MR::offBind(this);

    Coin::initAfterPlacement();
}

void BlueCoin::control() {
    if (MR::isOnSwitchB(this) && MR::isHiddenModel(this))
        appearAndMove();
}

void BlueCoin::calcAndSetBaseMtx() {
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
    if (MR::isValidSwitchA(this))
        MR::onSwitchA(this);

    #ifdef SM64BLUECOIN
    MR::emitEffect(this, "BlueCoinGet"); 
    #else
    
    #if defined SMG63 && !defined ALL 
        MR::emitEffect(this, "BlueCoinGet"); 
        MR::startSystemSE("SE_SY_TICO_COIN", -1, -1);
    #else
        MR::emitEffect(this, BlueCoinUtil::isBlueCoinGotCurrentFile(mID) ? "BlueCoinClearGet" : "BlueCoinGet"); 
        MR::startSystemSE("SE_SY_PURPLE_COIN", -1, -1);     
    #endif

    if (!BlueCoinUtil::isBlueCoinGotCurrentFile(mID)) {
        BlueCoinUtil::setBlueCoinGotCurrentFile(mID);
        BlueCoinUtil::startCounterCountUp();
    }
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
#endif