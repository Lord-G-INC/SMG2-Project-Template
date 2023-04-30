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
    mIsValidCounterAppear = false;
    mIsTimerMode = false;

    mTimerModeMinutes = 0;
    mTimerModeSeconds = 30;

    mTimerMinutes = 0;
    mTimerSeconds = 0;
    mTimerMilliseconds = 0;

    mIsUp = false;
}

void RedCoinController::init(const JMapInfoIter& rIter) {
    MR::initDefaultPos(this, rIter);
    MR::connectToSceneMapObjMovement(this);
    MR::invalidateClipping(this);
    MR::registerDemoSimpleCastAll(this);
    MR::useStageSwitchWriteA(this, rIter);
    MR::useStageSwitchReadB(this, rIter);
    MR::joinToGroupArray(this, rIter, "RedCoin", 64);

    // Get Obj_args
    MR::getJMapInfoArg0NoInit(rIter, &mShouldNotRewardCoins); // Should the Red Coin increment the coin counter by 2?
    MR::getJMapInfoArg1NoInit(rIter, &mPowerStarCheck); // Power Star to check for to set the collected star indicator
    MR::getJMapInfoArg2NoInit(rIter, &mIconID); // PictureFont.brfnt entry to display
    MR::getJMapInfoArg3NoInit(rIter, &mIsTimerMode);
    MR::getJMapInfoArg4NoInit(rIter, &mTimerModeMinutes);
    MR::getJMapInfoArg5NoInit(rIter, &mTimerModeSeconds);

    mTimerMinutes = mTimerModeMinutes;
    mTimerSeconds = mTimerModeSeconds;

    // Initialize the RedCoinCounter
    mRedCoinCounter = MR::createSimpleLayout("counter", "RedCoinCounter", 2);
    mRedCoinCounter->initEffectKeeper(0, 0, 0);
    mRedCoinCounter->appear();
    MR::setTextBoxNumberRecursive(mRedCoinCounter, "Counter", 0);
    MR::hideLayout(mRedCoinCounter);

    wchar_t str;
    MR::addPictureFontCode(&str, MR::hasPowerStarInCurrentStage(mPowerStarCheck) ? mIconID : 0x52);
    MR::setTextBoxFormatRecursive(mRedCoinCounter, "TxtStar", &str);

    mRumbler = new CountUpPaneRumbler(mRedCoinCounter, "Counter");
    mRumbler->mRumbleCalculator->mRumbleStrength = 8.0f;
    mRumbler->reset();

    if (mIsTimerMode)
        MR::showPaneRecursive(mRedCoinCounter, "ShaTime");
}

void RedCoinController::movement() {
    mRumbler->update();

    calcCounterVisibility();

    if (mIsTimerMode && MR::isOnSwitchB(this) && !mHasAllRedCoins) {
        appearCounterIfHidden();
        calcCounterTimer();
    }

    if (mHasAllRedCoins)
        mElapsed++; // There may be a better way to do this

    if (mElapsed == 120)
        MR::startAnim(mRedCoinCounter, "End", 0);

    if (mElapsed == 140) {
        MR::hideLayout(mRedCoinCounter);
        MR::onSwitchA(this);
        MR::getGroupFromArray(this)->killAll();
    }
}

void RedCoinController::calcCounterTimer() {
    if (!mIsUp) {
        if (mTimerMilliseconds < 60)
            mTimerMilliseconds--;

        if (mTimerMilliseconds < 0) {
            mTimerMilliseconds = 59;
    
            if (mTimerSeconds >= 0)
                mTimerSeconds--;
        }
    
        if (mTimerSeconds < 0) {
            mTimerSeconds = 59;
    
            if (mTimerMinutes > 0)
                mTimerMinutes--;
        }
    }
    else {
        resetAllRedCoins();
        mTimerMilliseconds = -1;
    }


    if (mTimerMinutes == 0 && mTimerSeconds == 0 && mTimerMilliseconds == 0)
        mIsUp = true;

    MR::setTextBoxFormatRecursive(mRedCoinCounter, "ShaTime", L"%02d:%02d", mTimerMinutes, mTimerSeconds);
}

// This function is unused, but there are plans for
// this function to be included if it can be figured out.

void RedCoinController::resetAllRedCoins() {
    MR::offSwitchB(this);
    LiveActorGroup* group = MR::getGroupFromArray(this);

    for (s32 i = 0; i < group->mNumObjs; i++) {
        if (!strcmp(group->getActor(i)->mName, "RedCoin")) {
            RedCoin* coin = ((RedCoin*)group->getActor(i));
            MR::hideModel(coin);
            MR::invalidateShadowAll(coin);
            MR::invalidateHitSensors(coin);
            MR::stopAnim(coin->mCoinCounterPlayer, 0);
            coin->mIsCollected = false;

            if (coin->mIsInAirBubble)
                coin->mAirBubble->makeActorAppeared();
        }
    }


    mTimerMinutes = mTimerModeMinutes;
    mTimerSeconds = mTimerModeSeconds;
    mTimerMilliseconds = 0;
    mNumCoins = 0;
    mIsUp = false;
    mIsValidCounterAppear = false;
    MR::startSystemSE("SE_SY_TIMER_A_0", -1, -1);
    MR::hideLayout(mRedCoinCounter);
    MR::setTextBoxNumberRecursive(mRedCoinCounter, "Counter", 0);
}

// Increases both layouts by 1
void RedCoinController::incCountAndUpdateLayouts() {
    mNumCoins++;
    
    mHasAllRedCoins = mNumCoins < MR::getGroupFromArray(this)->mNumObjs - 1 ? 0 : 1;

    updateCounter();
}

void RedCoinController::updateCounter() {
    if (mNumCoins == 1 && !mIsTimerMode) {
        mIsValidCounterAppear = true;
        appearCounterIfHidden();
    }
    
    MR::setTextBoxNumberRecursive(mRedCoinCounter, "Counter", mNumCoins);
    MR::startPaneAnim(mRedCoinCounter, "Counter", mHasAllRedCoins ? "FlashLoop" : "Flash", 0);
    MR::emitEffect(mRedCoinCounter, "RedCoinCounterLight");
    mRumbler->start();
}

void RedCoinController::calcCounterVisibility() {
    if (MR::isPowerStarGetDemoActive() || MR::isDemoActive() || MR::isPlayerDead() || MR::isTimeKeepDemoActive() || MR::isNormalTalking() || MR::isSystemTalking())
        MR::hideLayout(mRedCoinCounter);
    else {
        if (mIsValidCounterAppear)
            appearCounterIfHidden();
    }   
}

void RedCoinController::appearCounterIfHidden() {
    if (MR::isHiddenLayout(mRedCoinCounter)) {
        MR::showLayout(mRedCoinCounter);
        MR::startAnim(mRedCoinCounter, "Appear", 0);
        MR::startAnim(mRedCoinCounter, "Wait", 1);
    }
}