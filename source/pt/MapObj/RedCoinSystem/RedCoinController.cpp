#include "pt/MapObj/RedCoinSystem/RedCoinController.h"
#include "pt/Util/ActorUtil.h"

/* --- RED COIN CONTROLLER --- */

/*
    Manages the collected Red Coins, calculated through an Actor Group.
    Activates an event once all linked Red Coins, usually 8, are collected.

    Obj_args:
    0: [Bool] Do not increment the coin counter by 2
    1: [Int] Power Star Indicator: -1 to disable
    2: [Int] Picture Font Icon ID Entry

    Created by Evanbowl
*/

RedCoinController::RedCoinController(const char* pName) : LiveActor(pName) {
    mNumCoins = 0;
    mElapsed = 0;
    mHasAllRedCoins = false;

    mShouldNotRewardCoins = false;
    mPowerStarCheck = 0;
    mIconID = 0x37;
}

void RedCoinController::init(const JMapInfoIter& rIter) {
    MR::initDefaultPos(this, rIter);
    MR::connectToSceneMapObjMovement(this);
    MR::invalidateClipping(this);
    MR::joinToGroupArray(this, rIter, "RedCoin", 64);
    MR::registerDemoSimpleCastAll(this);

    MR::useStageSwitchWriteA(this, rIter);
    MR::useStageSwitchReadB(this, rIter);

    // Get Obj_args
    MR::getJMapInfoArg0NoInit(rIter, &mShouldNotRewardCoins); // Should the Red Coin increment the coin counter by 2?
    MR::getJMapInfoArg1NoInit(rIter, &mPowerStarCheck); // Power Star to check for to set the collected star indicator
    MR::getJMapInfoArg2NoInit(rIter, &mIconID); // PictureFont.brfnt entry to display

    // Initialize the RedCoinCounter
    mRedCoinCounter = new RedCoinCounter("RedCoinCounter");
    mRedCoinCounter->initWithoutIter();
    mRedCoinCounter->appear();
    mRedCoinCounter->updateStarIndicator(mPowerStarCheck, mIconID);
}

void RedCoinController::movement() {
    mRedCoinCounter->calcVisibility();

    if (mHasAllRedCoins)
        mElapsed++; // There may be a better way to do this

    if (mElapsed == 120)
        MR::startAnim(mRedCoinCounter, "End", 0);

    if (mElapsed == 140) {
        MR::hideLayout(mRedCoinCounter);
        MR::onSwitchA(this);
        makeActorDead();
    }
}

// This function is unused, but there are plans for
// this function to be included if it can be figured out.

void RedCoinController::resetAllRedCoins() {
    LiveActorGroup* group = MR::getGroupFromArray(this);

    for (s32 i = 0; i < group->mNumObjs; i++) {
        if (!strcmp(group->getActor(i)->mName, "RedCoin")) {
            RedCoin* coin = ((RedCoin*)group->getActor(i));
            MR::showModel(coin);
            MR::validateShadowAll(coin);
            MR::validateHitSensors(coin);
            coin->mIsCollected = false;

            if (coin->mIsInAirBubble)
                coin->mAirBubble->makeActorAppeared();
        }
    }

    mNumCoins = 0;
    MR::setTextBoxNumberRecursive(mRedCoinCounter, "Counter", 0);
    mRedCoinCounter->mIsValidAppear = false;
    MR::hideLayout(mRedCoinCounter);
    MR::offSwitchB(this);
}

// Increases both layouts by 1
void RedCoinController::incCountAndUpdateLayouts(LiveActor* pActor) {
    RedCoin* coin = (RedCoin*)pActor;
    mNumCoins++;
    
    mHasAllRedCoins = mNumCoins < MR::getGroupFromArray(this)->mNumObjs - 1 ? 0 : 1;

    mRedCoinCounter->updateCounter(mNumCoins, mHasAllRedCoins);
    coin->mCoinCounterPlayer->updateCounter(mNumCoins);
    
    // Only ever increment coins once.
    if (!coin->mHasRewardedCoins) {
        GameSequenceFunction::getPlayResultInStageHolder()->addCoinNum(mShouldNotRewardCoins ? 0 : 2);
        coin->mHasRewardedCoins = true;
    }
}

/* --- RED COIN UTIL  --- */

namespace RedCoinUtil {

// Iterates through the group actors, until it finds an actor in the group named "RedCoinController".
// If the actor name matches, then the actor is returned.

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

}