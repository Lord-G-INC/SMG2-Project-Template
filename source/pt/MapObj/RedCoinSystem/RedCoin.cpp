#include "pt/MapObj/RedCoinSystem/RedCoin.h"
#include "Game/MapObj/CoinHolder.h"
#include "pt/Util/ActorUtil.h"
/*
* These actors are exclusive to PT Debug.
* RedCoin, RedCoinController
*
* A long awaited project that I finally decided to scrape together.
*
* A coin type from other Mario games that activates an event once all
* red coins (usually 8) are collected. Typically a star would spawn.
*
* Some parts of this actor are initialized through ActorInfo.
*
* Must be linked to a RedCoinController through Group ID.
*
* Credits:
* Evanbowl, Lord-Giganticus, Galaxy Master, and Aurum for helping me with crash fixes.
*/

/* --- RED COIN --- */
RedCoin::RedCoin(const char* pName) : Coin(pName) {
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


    mIsCollected = false;
    mLaunchVelocity = 250.0f;
    mIsInAirBubble = false;
    mInvalidateShadows = false;
    mHasRewardedCoins = false;
    mAppearDelay = 0;
    mElapsed = 0;
    mRedCoinCounterPlayerPos = false;

    // Setup Coin
    MR::createCoinRotater();
    MR::createCoinHolder();
    MR::addToCoinHolder(this, this);
}

void RedCoin::init(const JMapInfoIter& rIter) {
    MR::processInitFunction(this, rIter, false);
    MR::joinToGroupArray(this, rIter, "RedCoinGroup", 24);
    MR::calcGravity(this);
    MR::invalidateClipping(this);

    MR::getJMapInfoArg0NoInit(rIter, &mLaunchVelocity); // Y Appear Launch Velocity. Calculates gravity.
    MR::getJMapInfoArg1NoInit(rIter, &mRedCoinCounterPlayerPos);
    MR::getJMapInfoArg2NoInit(rIter, &mAppearDelay); // SW_B Appear Spawn Delay
  
    Coin::setShadowAndPoseModeFromJMapIter(rIter); // Obj_args 3 and 4
    Coin::initShadow(rIter); // Obj_args 5 and 6

    initNerve(&NrvCoin::CoinNrvFix::sInstance, 0);

    initHitSensor(1);
    MR::addHitSensor(this, "RedCoin", 0x4A, 4, 55.0f, TVec3f(0.0f, 70.0f, 0.0f));
    mFlashingCtrl = new FlashingCtrl(this, 1);

    makeActorAppeared();

    // Can't use ActorInfo for this one...
    MR::useStageSwitchSyncAppear(this, rIter);
}

void RedCoin::initAfterPlacement() {
    if (MR::isValidSwitchB(this)) {
        MR::hideModel(this);
        MR::invalidateHitSensors(this);
    }
    else
        MR::offBind(this);

    if (mInvalidateShadows)
        MR::invalidateShadowAll(this);

    initAirBubble();

    Coin::initAfterPlacement();
}


void RedCoin::control() {
    if (MR::isOnSwitchB(this) && !mIsCollected) {
        if (mElapsed == mAppearDelay)
            appearAndMove();

    mElapsed++;
    }

    if (MR::isOnSwitchB(this))
        mLifeTime = 0x7FFFFFFF;

    if (mIsCollected)
        MR::zeroVelocity(this);
}

void RedCoin::calcAndSetBaseMtx() {
    Coin::calcAndSetBaseMtx();
}

bool RedCoin::receiveMessage(u32 msg, HitSensor* pSender, HitSensor* pReceiver) {
	if (MR::isMsgItemGet(msg) && !mIsCollected) {
		collect();
        return true;
    }
    
	return false;
}

void RedCoin::initAirBubble() {
    if (mIsInAirBubble && !MR::isValidSwitchB(this)) {
        mAirBubble = MR::createPartsModelNoSilhouettedMapObj(this, "空気アワ", "AirBubble", 0);
        mAirBubble->initFixedPosition(TVec3f(0.0f, 70.0f, 0.0f), TVec3f(0.0f, 0.0f, 0.0f), 0);
        MR::startAction(mAirBubble, "Move");
        MR::setSensorRadius(this, "RedCoin", 100.0f);
    }
}

void RedCoin::appearAndMove() {
    // I need a better way to calculate the gravity
    TVec3f coinVelocity = TVec3f(0.0f, mLaunchVelocity / 10.0f, 0.0f);
    coinVelocity.scale(coinVelocity.y, -mGravity);
    
    appearMove(mTranslation, coinVelocity, 300, 60);
    MR::startSystemSE("SE_SY_RED_COIN_APPEAR", -1, -1);
}

void RedCoin::collect() {
    mIsCollected = true;

    RedCoinController* pController = (RedCoinController*)pt::getSpecificActorFromGroup(this, "RedCoinController");
    
    if (MR::isValidSwitchA(this))
        MR::onSwitchA(this);
    
    if (mIsInAirBubble) {
        MR::emitEffect(mAirBubble, "RecoveryBubbleBreak");
        mAirBubble->kill();
    }
    
    // Only ever increment coins once.
    if (!mHasRewardedCoins && !MR::isGalaxyDarkCometAppearInCurrentStage()) {
        MR::incPlayerLife(1);
        GameSequenceFunction::getPlayResultInStageHolder()->addCoinNum(!pController->mRewardCoins ? 2 : 0);
        mHasRewardedCoins = true;
    }

    pController->startCountUp(this);

    MR::startSystemSE(pController->mHasAllRedCoins ? "SE_SY_RED_COIN_COMPLETE" : "SE_SY_RED_COIN", -1, -1);

    MR::incPlayerOxygen(mIsInAirBubble ? 2 : 1);
    MR::invalidateHitSensors(this);
    MR::invalidateShadowAll(this);
    MR::emitEffect(this, "RedCoinGet");
    MR::hideModel(this);
}