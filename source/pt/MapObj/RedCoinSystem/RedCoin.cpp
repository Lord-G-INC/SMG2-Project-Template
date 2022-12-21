#include "pt/MapObj/RedCoinSystem/RedCoin.h"
#include "Game/LiveActor/LiveActorGroup.h"
#include "Game/MapObj/CoinHolder.h"
#include "pt/Util/ActorUtil.h"
#include "Game/System/AllData/GameSequenceFunction.h"

/*
* These actors are exclusive to PT Debug.
* RedCoin, RedCoinController, RedCoinAppearer
*
* A long awaited project, that I finally decided to scrape together.
*
* A coin type from other Mario games that activates an event once all
* red coins (usually 8) are collected. Typically a star would spawn.
*
* Credits:
* Evanbowl, Lord-Giganticus, Galaxy Master, and Aurum for helping me with crash fixes.
*
/* --- RED COIN --- */
RedCoin::RedCoin(const char* pName) : Coin(pName) {
    mIsCollected = false;
    mLaunchVelocity = 25.0f;
    mUseConnection = false;
    mIsInAirBubble = false;
    mInvalidateShadows = false;
    mCounterPlayerPos = false;
    mHasRewardedCoins = false;

    MR::createCoinRotater();
    MR::createCoinHolder();
    MR::addToCoinHolder(this, this);
}

void RedCoin::init(const JMapInfoIter& rIter) {
    MR::processInitFunction(this, rIter, false);
    MR::joinToGroupArray(this, rIter, "RedCoin", 64);

    MR::getJMapInfoArg0NoInit(rIter, &mLaunchVelocity);
    MR::getJMapInfoArg1NoInit(rIter, &mUseConnection);
    MR::getJMapInfoArg2NoInit(rIter, &mIsInAirBubble);
    MR::getJMapInfoArg3NoInit(rIter, &mInvalidateShadows);
    MR::getJMapInfoArg4NoInit(rIter, &mCounterPlayerPos);
    
    initNerve(&NrvCoin::CoinNrvFix::sInstance, 0);

    initHitSensor(1);
    MR::addHitSensor(this, "RedCoin", 0x4A, 4, 55.0f, TVec3f(0.0f, 70.0f, 0.0f));

    mFlashingCtrl = new FlashingCtrl(this, 1);

    mConnector = new MapObjConnector(this);
    mConnector->attach(mTranslation);

    mCoinCounterPlayer = new RedCoinCounterPlayer("RedCoinCounterPlayer");
    mCoinCounterPlayer->initWithoutIter();

    makeActorAppeared();

    MR::useStageSwitchSyncAppear(this, rIter);

    if (!mUseConnection)
        MR::offBind(this);

    if (MR::isValidSwitchB(this)) {
        MR::hideModel(this);
        MR::invalidateHitSensors(this);
    }

    if (mInvalidateShadows)
        MR::invalidateShadowAll(this);

    initAirBubble();
}

void RedCoin::control() {
    mCoinCounterPlayer->calcScreenPos(mCounterPlayerPos ? *MR::getPlayerPos() : mTranslation, mCounterPlayerPos ? 250.0f : 150.0f);

    MR::calcGravity(this);
    
    if (MR::isOnSwitchB(this) && MR::isHiddenModel(this) && !mIsCollected)
        appearAndMove();
    
    if (mIsCollected)
        mVelocity = TVec3f(0.0f, 0.0f, 0.0f);
}

void RedCoin::calcAndSetBaseMtx() {
    if (mUseConnection) {
        mConnector->connect();
        mConnector->attachToUnder();
    }

    Coin::calcAndSetBaseMtx();
}

bool RedCoin::receiveMessage(u32 msg, HitSensor* pSender, HitSensor* pReceiver) {
	if (MR::isMsgItemGet(msg) && !mIsCollected)
		collect();
		return false;
}

void RedCoin::initAirBubble() {
    if (mIsInAirBubble && !mUseConnection && !MR::isValidSwitchB(this)) {
        mAirBubble = MR::createPartsModelNoSilhouettedMapObj(this, "空気アワ", "AirBubble", 0);
        mAirBubble->initFixedPosition(TVec3f(0.0f, 70.0f, 0.0f), TVec3f(0.0f, 0.0f, 0.0f), 0);
        MR::startAction(mAirBubble, "Move");
        MR::setSensorRadius(this, "RedCoin", 100.0f);
    }
}

void RedCoin::appearAndMove() {
    MR::startSound(this, "SE_SY_RED_COIN_APPEAR", -1, -1);

    TVec3f coinVelocity = TVec3f(0.0f, mLaunchVelocity, 0.0f);
    coinVelocity.scale(coinVelocity.y, -mGravity);

    appearMove(mTranslation, coinVelocity, 1, 0);
    setCannotTime(100);
    setLife(0x7FFFFFFF);
    mFlashingCtrl->end();
    MR::validateHitSensors(this);
}

void RedCoin::collect() {
    if (MR::isValidSwitchA(this))
        MR::onSwitchA(this);
    
    if (mIsInAirBubble) {
        MR::emitEffect(mAirBubble, "RecoveryBubbleBreak");
        mAirBubble->kill();
    }
    
    MR::incPlayerOxygen(mIsInAirBubble ? 2 : 1);

    getRedCoinControllerFromGroup(this)->incCountAndUpdateLayouts(this);
    MR::startSound(this, getRedCoinControllerFromGroup(this)->mHasAllRedCoins ? "SE_SY_RED_COIN_COMPLETE" : "SE_SY_RED_COIN", -1, -1);

    mIsCollected = true;
    MR::invalidateHitSensors(this);
    MR::invalidateShadowAll(this);
    MR::emitEffect(this, "RedCoinGet");
    MR::incPlayerLife(1);
    MR::hideModel(this);
}