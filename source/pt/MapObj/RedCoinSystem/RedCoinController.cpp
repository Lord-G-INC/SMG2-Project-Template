#include "pt/MapObj/RedCoinSystem/RedCoinController.h"

/* --- RED COIN CONTROLLER --- */

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
    MR::joinToGroupArray(this, rIter, "RedCoin", 32);

    MR::useStageSwitchReadAppear(this, rIter);
    MR::useStageSwitchReadA(this, rIter);
    MR::useStageSwitchWriteB(this, rIter);

    MR::getJMapInfoArg0NoInit(rIter, &mShouldNotRewardCoins); // Should the Red Coin increment the coin counter by 2?
    MR::getJMapInfoArg1NoInit(rIter, &mPowerStarCheck); // Power Star to check for to set the collected star indicator
    MR::getJMapInfoArg2NoInit(rIter, &mIconID); // PictureFont.brfnt entry to display

    initSound(1, "RedCoin", &mTranslation, 0);
    
    mRedCoinCounter = new RedCoinCounter("RedCoinCounter");
    mRedCoinCounter->initWithoutIter();
    mRedCoinCounter->appear();
    mRedCoinCounter->updateStarIndicator(mPowerStarCheck, mIconID);
}

void RedCoinController::movement() {
    calcCounterVisibilty();

    if (mHasAllRedCoins) {
        mElapsed++;

        //if (MR::isValidSwitchB(this))
        //    if (!MR::isOnSwitchB(this)) {
        //        MR::onSwitchB(this);
        //    }
    }
        if (MR::isValidSwitchB(this))
            if (MR::isOnSwitchB(this) && mNumCoins > 0) {
                resetAllRedCoins();
            }

        if (mElapsed == 120)
            MR::startAnim(mRedCoinCounter, "End", 0);

        if (mElapsed == 140) {
            MR::onSwitchA(this);
            makeActorDead();
        }
}

void RedCoinController::resetAllRedCoins() {
    LiveActorGroup* group = MR::getGroupFromArray(this);

    for (s32 i = 0; i < group->mNumObjs; i++) {
        if (!strcmp(group->getActor(i)->mName, "RedCoin")) {
            group->getActor(i)->makeActorAppeared();
            MR::showModel(group->getActor(i));
            MR::validateShadowAll(group->getActor(i));
            MR::validateHitSensors(group->getActor(i));
            ((RedCoin*)group->getActor(i))->mIsCollected = false;
        }
    }

    mNumCoins = 0;
    MR::hideLayout(mRedCoinCounter);
    MR::setTextBoxNumberRecursive(mRedCoinCounter, "Counter", 0);
    MR::offSwitchB(this);
}

void RedCoinController::incCountAndUpdateLayouts(LiveActor* pActor) {
    mNumCoins += 1;
    
    mHasAllRedCoins = mNumCoins < MR::getGroupFromArray(this)->mNumObjs - 1 ? 0 : 1;

    mRedCoinCounter->updateCounter(mNumCoins, mHasAllRedCoins);
    ((RedCoin*)pActor)->mCoinCounterPlayer->updateCounter(mNumCoins);
    
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
}

/* --- LAYOUTS --- */

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