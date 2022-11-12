#include "pt/MapObj/RedCoinSystem.h"
#include "Game/LiveActor/LiveActorGroup.h"
#include "Game/System/Misc/GameSceneLayoutHolder.h"
/*
* These actors are exclusive to PT Debug.
* RedCoin, RedCoinController, RedCoinAppearer
*
* A long awaited project, that I finally decided to scrape together.
* I got barely any help with this, but I finally did it.
*
* Credits:
* Evanbowl, Kairos, Lord-Giganticus, Galaxy Master
*/

/* --- ACTORS --- */


/* --- RED COIN --- */
RedCoin::RedCoin(const char* pName) : Coin(pName) {
    mIsCollected = false;
    mIsSpawnedCoin = false;
}

void RedCoin::init(const JMapInfoIter& rIter) {
    MR::processInitFunction(this, "RedCoin", true);
    MR::initDefaultPos(this, rIter);
    MR::connectToSceneItemStrongLight(this);
    MR::invalidateClipping(this);
    MR::useStageSwitchWriteA(this, rIter);
    MR::initShadowVolumeCylinder(this, 50);
    MR::calcGravity(this);

    initEffectKeeper(1, "RedCoin", false);
    initNerve(&NrvRedCoin::NrvInit::sInstance, 0);
    initHitSensor(1);

    MR::joinToGroupArray(this, rIter, "RedCoin", 32);

    MR::addHitSensorMapObj(this, "RedCoin", 1, 25.0f, TVec3f(0.0f, 0.0f, 0.0f));
    initBinder(0, 0, 0);

    makeActorAppeared();

    mCoinCounterPlayer = new RedCoinCounterPlayer("RedCoinCounterPlayer", this);
    mCoinCounterPlayer->initWithoutIter();
}

bool RedCoin::receiveMessage(u32 msg, HitSensor* pSender, HitSensor* pReceiver) {
	if (MR::isMsgItemGet(msg) && !mIsCollected)
		collect();
		return false;
}

void RedCoin::collect() {

    if (!mIsSpawnedCoin) { // Only red coins that do NOT spawn trigger these functions.
        getRedCoinController(this)->incCountAndUpdateLayouts(this);
    }

    MR::hideModel(this);
    MR::incCoin(2, this);
    MR::invalidateHitSensors(this);
    MR::emitEffect(this, "RedCoinGet");
    makeActorDead();
}

namespace NrvRedCoin {
	void NrvInit::execute(Spine* pSpine) const {
		RedCoin* pActor = (RedCoin*)pSpine->mExecutor;
	}

    NrvInit(NrvInit::sInstance);
}



/* --- RED COIN APPEARER --- */

RedCoinAppearer::RedCoinAppearer(const char* pName) : LiveActor(pName) {
    mRedCoin = new RedCoin("RedCoinAppeared");
    mLaunchVelocity = 25.0f;
    mHasSpawned = false;
}

void RedCoinAppearer::init(const JMapInfoIter& rIter) {
    MR::initDefaultPos(this, rIter);
    MR::connectToSceneMapObjMovement(this);
    MR::useStageSwitchWriteA(this, rIter);
    MR::calcGravity(this);
    MR::getJMapInfoArg0NoInit(rIter, &mLaunchVelocity);

    MR::joinToGroupArray(this, rIter, "RedCoin", 32);

    initSound(1, "RedCoinA", &mTranslation, 0);

    mRedCoin->initWithoutIter();
    mRedCoin->mTranslation.set(mTranslation);

    MR::invalidateHitSensors(mRedCoin);
    MR::hideModel(mRedCoin);
}

void RedCoinAppearer::movement() {
    if (MR::isOnSwitchA(this) && !mHasSpawned) {
        TVec3f coinVelocity = TVec3f(0.0f, mLaunchVelocity, 0.0f);
        coinVelocity.scale(coinVelocity.y, -mGravity);

        MR::startSound(this, "SE_SY_RED_COIN_APPEAR", -1, -1);

        mRedCoin->mLifeTime = 0x7FFFFFFF;
        mRedCoin->appearMove(mTranslation, coinVelocity, 1, 0);
        mRedCoin->mIsSpawnedCoin = true;
        MR::validateHitSensors(mRedCoin);
        
        mHasSpawned = true;
    }

    if (MR::isDead(mRedCoin)) { // Collection function for only spawned coins.
        getRedCoinController(this)->incCountAndUpdateLayouts(mRedCoin);
        makeActorDead();
    }  
}



/* --- RED COIN CONTROLLER --- */

RedCoinController::RedCoinController(const char* pName) : LiveActor(pName) {
    mNumCoins = 0;
    mRedCoinCounter = new RedCoinCounter("RedCoinCounter");
}

void RedCoinController::init(const JMapInfoIter& rIter) {
    MR::initDefaultPos(this, rIter);
    MR::connectToSceneMapObjMovement(this);
    MR::invalidateClipping(this);
    MR::joinToGroupArray(this, rIter, "RedCoin", 32);
    initSound(3, "RedCoin", &mTranslation, 0);
    mRedCoinCounter->initWithoutIter();
}

void RedCoinController::movement() {
    if (mNumCoins == MR::getGroupFromArray(this)->mNumObjs - 1)
        makeActorDead();
}

void RedCoinController::incCountAndUpdateLayouts(RedCoin* pRedCoin) {
    mNumCoins += 1;

    // Coin Counter update
    MR::setTextBoxNumberRecursive(mRedCoinCounter, "Counter", mNumCoins);
    MR::emitEffect(mRedCoinCounter, "RedCoinCounterLight");

    // Coin Counter Player update
    MR::setTextBoxNumberRecursive(pRedCoin->mCoinCounterPlayer, "TxtText", getRedCoinController(this)->mNumCoins);
    pRedCoin->mCoinCounterPlayer->appear();
    MR::startAnim(pRedCoin->mCoinCounterPlayer, "End", 0);

    if (mNumCoins < MR::getGroupFromArray(this)->mNumObjs - 1) {
        MR::startSound(this, "SE_SY_RED_COIN", -1, -1);
        MR::startPaneAnim(mRedCoinCounter, "Counter", "Flash", 0);
    }
    else {
        MR::startSound(this, "SE_SY_RED_COIN_COMPLETE", -1, -1);
        MR::startPaneAnim(mRedCoinCounter, "Counter", "Flash", 0);
    }
}

/* --- LAYOUTS --- */

/* --- RED COIN COUNTER --- */

RedCoinCounter::RedCoinCounter(const char* pName) : LayoutActor(pName, 1) {
}

void RedCoinCounter::init(const JMapInfoIter& rIter) {
    initLayoutManager("RedCoinCounter", 1);
    MR::createAndAddPaneCtrl(this, "Counter", 1);
    MR::connectToSceneLayout(this);
    MR::setTextBoxNumberRecursive(this, "Counter", 0);
    MR::startAnim(this, "Wait", 0);
    initEffectKeeper(1, "RedCoinCounter", 0);
    appear();
}

void RedCoinCounter::control() {
}



/* --- RED COIN COUNTER PLAYER --- */

RedCoinCounterPlayer::RedCoinCounterPlayer(const char* pName, RedCoin* pRedCoin) : LayoutActor(pName, 1) {
    mRedCoin = pRedCoin;
}

void RedCoinCounterPlayer::init(const JMapInfoIter& rIter) {
    initLayoutManager("RedCoinCounterPlayer", 1);
    MR::connectToSceneLayout(this);
    MR::setTextBoxNumberRecursive(this, "TxtText", 0);
    MR::createAndAddPaneCtrl(this, "Text00", 0);
}

void RedCoinCounterPlayer::control() {
    TVec2f* screenPos = new TVec2f();
    MR::calcScreenPosition(screenPos, mRedCoin->mTranslation);
    setTrans(*screenPos);
}



/* --- RED COIN UTIL  --- */

// Iterates through the group actors, until it finds an actor in the group named "RedCoinController".
// If the actor is found, then it is returned.

RedCoinController* getRedCoinController(LiveActor* actor) {
    LiveActorGroup* group = MR::getGroupFromArray(actor);

    for (s32 i = 0; i < group->mNumObjs; i++) {
        if (!strcmp(group->getActor(i)->mName, "RedCoinController"))
            return (RedCoinController*)group->getActor(i);
    }
    return 0;
}