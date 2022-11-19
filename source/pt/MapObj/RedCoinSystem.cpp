#include "pt/MapObj/RedCoinSystem.h"
#include "Game/LiveActor/LiveActorGroup.h"
#include "Game/System/Misc/GameSceneLayoutHolder.h"
#include "Game/MapObj/CoinHolder.h"
#include "pt/Util/ActorUtil.h"

/*
* These actors are exclusive to PT Debug.
* RedCoin, RedCoinController, RedCoinAppearer
*
* A long awaited project, that I finally decided to scrape together.
* I got barely any help with this, but I finally did it.
*
* Credits:
* Evanbowl, Kairos, Lord-Giganticus, Galaxy Master, and Aurum for helping me with crash fixes.
*/

/* --- RED COIN --- */
RedCoin::RedCoin(const char* pName) : Coin(pName) {
    mIsCollected = false;
    mIsSpawnedCoin = false;
    mIsSpawned = false;
    mUseConnection = false;
    mLaunchVelocity = 25.0f;

    pt::setupCoin(this);
}

void RedCoin::init(const JMapInfoIter& rIter) {
    MR::processInitFunction(this, "RedCoin", true);
    MR::initDefaultPos(this, rIter);
    MR::connectToSceneItemStrongLight(this);
    MR::invalidateClipping(this);
    MR::initShadowVolumeCylinder(this, 50);
    MR::calcGravity(this);

    MR::getJMapInfoArg0NoInit(rIter, &mLaunchVelocity);
    MR::getJMapInfoArg1NoInit(rIter, &mUseConnection);

    MR::useStageSwitchSyncAppear(this, rIter);
    MR::useStageSwitchWriteA(this, rIter);
    MR::useStageSwitchReadB(this, rIter);

    initEffectKeeper(1, "RedCoin", false);
    initNerve(&NrvCoin::CoinNrvFix::sInstance, 0);
    initHitSensor(1);
    initSound(1, "RedCoin", &mTranslation, TVec3f(0.0f, 0.0f, 0.0f));

    MR::joinToGroupArray(this, rIter, "RedCoin", 32);

    MR::addHitSensorMapObj(this, "RedCoin", 1, 25.0f, TVec3f(0.0f, 0.0f, 0.0f));
    initBinder(0, 0, 0);

    mFlashingCtrl = new FlashingCtrl(this, 1);

    mConnector = new MapObjConnector(this);

    mCoinCounterPlayer = new RedCoinCounterPlayer("RedCoinCounterPlayer", this);
    mCoinCounterPlayer->initWithoutIter();
    
    appear();

    if (MR::isValidSwitchB(this)) {
        MR::hideModel(this);
        MR::invalidateHitSensors(this);
    }
}

void RedCoin::calcAndSetBaseMtx() {
    if (mUseConnection)
        mConnector->connect();

    Coin::calcAndSetBaseMtx();
}

bool RedCoin::receiveMessage(u32 msg, HitSensor* pSender, HitSensor* pReceiver) {
	if (MR::isMsgItemGet(msg) && !mIsCollected)
		collect();
		return false;
}

void RedCoin::control() {
    if (MR::isOnSwitchB(this) && !mIsSpawned) {
            MR::startSound(this, "SE_SY_RED_COIN_APPEAR", -1, -1);

        TVec3f coinVelocity = TVec3f(0.0f, mLaunchVelocity, 0.0f);
        coinVelocity.scale(coinVelocity.y, -mGravity);

        appearMove(mTranslation, coinVelocity, 1, 0);
        mFlashingCtrl->end();
        mLifeTime = 0x7FFFFFFF;
        MR::validateHitSensors(this);
        
        mIsSpawned = true;
    }
}

void RedCoin::collect() {

    if (!mIsSpawnedCoin)
        getRedCoinController(this)->incCountAndUpdateLayouts(this);

    mIsCollected = true;
    MR::hideModel(this);
    MR::invalidateHitSensors(this);
    MR::emitEffect(this, "RedCoinGet");
    MR::onSwitchA(this);
    mFlashingCtrl->end();
    makeActorDead();
}


/* --- RED COIN CONTROLLER --- */

RedCoinController::RedCoinController(const char* pName) : LiveActor(pName) {
    mNumCoins = 0;
    mElapsed = 0;
    mHasAllRedCoins = false;
    mCounterPlayerLayoutMode = false;
    mShouldNotRewardCoins = false;
    mRedCoinCounter = new RedCoinCounter("RedCoinCounter");
}

void RedCoinController::init(const JMapInfoIter& rIter) {
    MR::initDefaultPos(this, rIter);
    MR::connectToSceneMapObjMovement(this);
    MR::invalidateClipping(this);
    MR::joinToGroupArray(this, rIter, "RedCoin", 32);

    MR::getJMapInfoArg0NoInit(rIter, &mCounterPlayerLayoutMode);
    MR::getJMapInfoArg1NoInit(rIter, &mShouldNotRewardCoins);
    
    MR::useStageSwitchReadA(this, rIter);
    initSound(3, "RedCoin", &mTranslation, 0);

    mRedCoinCounter->initWithoutIter();
}

void RedCoinController::movement() {
    if (mHasAllRedCoins)
        mElapsed++;

        if (mElapsed == 120)
            MR::startAnim(mRedCoinCounter, "End", 0);

        if (mElapsed == 140)
            MR::onSwitchA(this);
}

void RedCoinController::incCountAndUpdateLayouts(RedCoin* pRedCoin) {
    mNumCoins += 1;
    mHasAllRedCoins = mNumCoins < MR::getGroupFromArray(this)->mNumObjs - 1 ? 0 : 1;

    MR::incCoin(mShouldNotRewardCoins ? 0 : 2, this);

    // Coin Counter update
    MR::setTextBoxNumberRecursive(mRedCoinCounter, "Counter", mNumCoins);
    MR::emitEffect(mRedCoinCounter, "RedCoinCounterLight");
    MR::startPaneAnim(mRedCoinCounter, "Counter", mHasAllRedCoins ? "FlashLoop" : "Flash", 0);
    mRedCoinCounter->mPaneRumbler->start();

    // Coin Counter Player update
    MR::setTextBoxNumberRecursive(pRedCoin->mCoinCounterPlayer, "TxtText", getRedCoinController(this)->mNumCoins);
    pRedCoin->mCoinCounterPlayer->mLytPos = mCounterPlayerLayoutMode;
    MR::startAnim(pRedCoin->mCoinCounterPlayer, "Appear", 0);
    pRedCoin->mCoinCounterPlayer->appear();

    MR::startSound(this, mHasAllRedCoins ? "SE_SY_RED_COIN_COMPLETE" : "SE_SY_RED_COIN", -1, -1);
}

/* --- LAYOUTS --- */

/* --- RED COIN COUNTER --- */

RedCoinCounter::RedCoinCounter(const char* pName) : LayoutActor(pName, 1) {
}

void RedCoinCounter::init(const JMapInfoIter& rIter) {
    initLayoutManager("RedCoinCounter", 2);
    initEffectKeeper(1, "RedCoinCounter", 0);
    MR::createAndAddPaneCtrl(this, "PCoinCounter", 1);
    MR::createAndAddPaneCtrl(this, "Counter", 1);
    MR::connectToSceneLayout(this);

    MR::setTextBoxNumberRecursive(this, "Counter", 0);
    MR::startAnim(this, "Wait", 0);

    mPaneRumbler = new CountUpPaneRumbler(this, "ShaNumber");

    appear();
    mPaneRumbler->reset();
}

void RedCoinCounter::control() {
    mPaneRumbler->update();
}



/* --- RED COIN COUNTER PLAYER --- */

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
    TVec2f* screenPos = new TVec2f();
    TVec3f playerPos = *MR::getPlayerPos();
    playerPos.y += 250;
    MR::calcScreenPosition(screenPos, mLytPos ? playerPos : mRedCoin->mTranslation);
    setTrans(*screenPos);
}



/* --- RED COIN UTIL  --- */

// Iterates through the group actors, until it finds an actor in the group named "RedCoinController".
// If the actor is found, then it is returned.

RedCoinController* getRedCoinController(LiveActor* actor) {
    LiveActorGroup* group = MR::getGroupFromArray(actor);

    if (group) {
        for (s32 i = 0; i < group->mNumObjs; i++) {
            if (!strcmp(group->getActor(i)->mName, "RedCoinController"))
                return (RedCoinController*)group->getActor(i);
        }
    }

    return 0;
}