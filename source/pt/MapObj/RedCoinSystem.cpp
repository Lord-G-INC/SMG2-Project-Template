#include "pt/MapObj/RedCoinSystem.h"
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

    MR::createCoinRotater();
    MR::createCoinHolder();
    MR::addToCoinHolder(this, this);
}

void RedCoin::init(const JMapInfoIter& rIter) {
    MR::processInitFunction(this, rIter, false);
    MR::joinToGroupArray(this, rIter, "RedCoin", 32);

    MR::getJMapInfoArg0NoInit(rIter, &mLaunchVelocity);
    MR::getJMapInfoArg1NoInit(rIter, &mUseConnection);
    MR::getJMapInfoArg2NoInit(rIter, &mIsInAirBubble);
    MR::getJMapInfoArg3NoInit(rIter, &mInvalidateShadows);

    initNerve(&NrvCoin::CoinNrvFix::sInstance, 0);

    initHitSensor(1);
    MR::addHitSensor(this, "RedCoin", 0x4A, 4, 55.0f, TVec3f(0.0f, 70.0f, 0.0f));

    mFlashingCtrl = new FlashingCtrl(this, 1);

    mCoinCounterPlayer = new RedCoinCounterPlayer("RedCoinCounterPlayer", this);
    mCoinCounterPlayer->initWithoutIter();

    makeActorAppeared();

    MR::useStageSwitchSyncAppear(this, rIter);
    
    if (MR::isValidSwitchB(this)) {
        MR::hideModel(this);
        MR::invalidateHitSensors(this);
    }

    if (mInvalidateShadows)
        MR::invalidateShadowAll(this);

    initAirBubble();
}

void RedCoin::control() {
    MR::calcGravity(this);
    
    if (MR::isOnSwitchB(this) && MR::isHiddenModel(this))   
        appearAndMove();
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
    mLifeTime = 0x7FFFFFFF;
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
    MR::hideModel(this);
    MR::invalidateHitSensors(this);
    MR::emitEffect(this, "RedCoinGet");
    mFlashingCtrl->end();
    makeActorDead();
}


/* --- RED COIN CONTROLLER --- */

RedCoinController::RedCoinController(const char* pName) : LiveActor(pName) {
    mNumCoins = 0;
    mElapsed = 0;
    mHasAllRedCoins = false;
    mRedCoinCounter = new RedCoinCounter("RedCoinCounter");

    mCounterPlayerLayoutMode = false;
    mShouldNotRewardCoins = false;
    mLayoutAnimType = false;
}

void RedCoinController::init(const JMapInfoIter& rIter) {
    MR::initDefaultPos(this, rIter);
    MR::connectToSceneMapObjMovement(this);
    MR::invalidateClipping(this);
    MR::joinToGroupArray(this, rIter, "RedCoin", 32);

    MR::useStageSwitchReadAppear(this, rIter);
    MR::useStageSwitchReadA(this, rIter);
    MR::useStageSwitchWriteB(this, rIter);

    MR::getJMapInfoArg0NoInit(rIter, &mCounterPlayerLayoutMode); // Should the number layout appear at the Red Coin or the player?
    MR::getJMapInfoArg1NoInit(rIter, &mLayoutAnimType); // RedCoinCounterPlayer: Picks the layout animation that should be played.
    MR::getJMapInfoArg2NoInit(rIter, &mShouldNotRewardCoins); // Should the Red Coin increment the coin counter by 2?

    initSound(1, "RedCoin", &mTranslation, 0);

    mRedCoinCounter->initWithoutIter();
    mRedCoinCounter->appear();
}

void RedCoinController::movement() {
    calcCounterVisibilty();

    if (mHasAllRedCoins) {
        mElapsed++;

        if (MR::isValidSwitchB(this))
            if (!MR::isOnSwitchB(this)) {
                MR::onSwitchB(this);
            }
    }

        if (mElapsed == 120)
            MR::startAnim(mRedCoinCounter, "End", 0);

        if (mElapsed == 140) {
            MR::onSwitchA(this);
            makeActorDead();
        }
}

void RedCoinController::incCountAndUpdateLayouts(RedCoin* pRedCoin) {
    mNumCoins += 1;
    
    mHasAllRedCoins = mNumCoins < MR::getGroupFromArray(this)->mNumObjs - 1 ? 0 : 1;

    mRedCoinCounter->updateCounter(mNumCoins, mHasAllRedCoins);
    pRedCoin->mCoinCounterPlayer->updateCounter(mNumCoins, mCounterPlayerLayoutMode, mLayoutAnimType);
    
    GameSequenceFunction::getPlayResultInStageHolder()->addCoinNum(mShouldNotRewardCoins ? 0 : 2);
}

void RedCoinController::calcCounterVisibilty() {
    bool checks = MR::isTimeKeepDemoActive() || MR::isNormalTalking() || MR::isPlayerDead() || MR::isPowerStarGetDemoActive();
    if (checks)
        MR::hideLayout(mRedCoinCounter);
    else {
        if (mRedCoinCounter->mIsValidAppear)
            mRedCoinCounter->appearIfHidden();
    }

    if (MR::isValidSwitchAppear(this)) {
        if (MR::isOnSwitchAppear(this))
            mRedCoinCounter->appearIfHidden();
    }
    else {
        if (MR::isValidSwitchB(this)) {
            if (!MR::isOnSwitchB(this) && mNumCoins > 0 && !mHasAllRedCoins)
                MR::hideLayout(mRedCoinCounter);
        else {
            if (mRedCoinCounter->mIsValidAppear) 
                MR::showLayout(mRedCoinCounter);
            }
        }
    }  
}

/* --- LAYOUTS --- */

/* --- RED COIN COUNTER --- */

/*
* Layout created by RedCoinController that displays the current
* number of red coins collected in the Red Coin Group.
*/
RedCoinCounter::RedCoinCounter(const char* pName) : LayoutActor(pName, 0) {
    mIsValidAppear = false;
}

void RedCoinCounter::init(const JMapInfoIter& rIter) {
    initLayoutManager("RedCoinCounter", 2);
    initEffectKeeper(0, 0, 0);

    MR::createAndAddPaneCtrl(this, "Counter", 1);
    MR::setTextBoxNumberRecursive(this, "Counter", 0);
    MR::connectToSceneLayout(this);
    MR::hideLayout(this);

    mPaneRumbler = new CountUpPaneRumbler(this, "Counter");
    mPaneRumbler->mRumbleCalculator->mRumbleStrength = 8.0f;
    mPaneRumbler->reset();
}

void RedCoinCounter::control() {
    mPaneRumbler->update();
}

void RedCoinCounter::appearIfHidden() { 
    if (MR::isHiddenLayout(this)) {
        MR::showLayout(this);
        MR::startAnim(this, "Appear", 0);
        MR::startAnim(this, "Wait", 1);
    }
}

void RedCoinCounter::updateCounter(s32 count, bool hasAllCoins) {

    if (count == 1) {
        mIsValidAppear = true;
        appearIfHidden();
    }

    MR::setTextBoxNumberRecursive(this, "Counter", count);
    MR::startPaneAnim(this, "Counter", hasAllCoins ? "FlashLoop" : "Flash", 0);
    MR::emitEffect(this, "RedCoinCounterLight");
    mPaneRumbler->start();
}

/* --- RED COIN COUNTER PLAYER --- */

/*
* Layout created by RedCoin that displays the current
* number of red coins collected on collection of a 
* Red Coin, and the position of the layout is at
* the collected coin.
*/

RedCoinCounterPlayer::RedCoinCounterPlayer(const char* pName, RedCoin* pRedCoin) : LayoutActor(pName, 1) {
    mRedCoin = pRedCoin;
    bool mLytPos;
}

void RedCoinCounterPlayer::init(const JMapInfoIter& rIter) {
    initLayoutManager("RedCoinCounterPlayer", 1);
    MR::connectToSceneLayout(this);
    MR::setTextBoxNumberRecursive(this, "TxtText", 0);
}

void RedCoinCounterPlayer::control() {
    TVec2f screenPos;
    TVec3f pos = mLytPos ? *MR::getPlayerPos() : mRedCoin->mTranslation;
    pos.y += mLytPos ? 250 : 150;
    MR::calcScreenPosition(&screenPos, pos);
    setTrans(screenPos);

    if (MR::isAnimStopped(this, 0))
        kill();
}

void RedCoinCounterPlayer::updateCounter(s32 count, bool layoutPos, bool layoutAnim) {
    MR::setTextBoxNumberRecursive(this, "TxtText", count);
    mLytPos = layoutPos;
    MR::startAnim(this, layoutAnim ? "Appear" : "AppearNew", 0);
    appear();

}

/* --- RED COIN UTIL  --- */

// Iterates through the group actors, until it finds an actor in the group named "RedCoinController".
// If the actor is found, then it is returned.

RedCoinController* getRedCoinControllerFromGroup(LiveActor* actor) {
    LiveActorGroup* group = MR::getGroupFromArray(actor);

    if (group) {
        for (s32 i = 0; i < group->mNumObjs; i++) {
            if (!strcmp(group->getActor(i)->mName, "RedCoinController"))
                return (RedCoinController*)group->getActor(i);
        }
    }

    return 0;
}