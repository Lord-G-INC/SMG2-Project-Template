#include "pt/MapObj/RedCoinSystem/RedCoinController.h"
#include "pt/MapObj/RedCoinSystem/RedCoinLayouts.h"
#include "pt/MapObj/BlueCoinSystem/BlueCoinUtil.h"
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
    mRewardCoins = false;
    mIsValidCounterAppear = false;
    mRedCoinCounter = 0;
    mRedCoinPlayerCounter = 0;
}

void RedCoinController::init(const JMapInfoIter& rIter) {
    MR::initDefaultPos(this, rIter);
    MR::connectToSceneMapObjMovement(this);
    MR::invalidateClipping(this);
    MR::registerDemoSimpleCastAll(this);
    MR::useStageSwitchWriteA(this, rIter);
    MR::useStageSwitchReadB(this, rIter);
    MR::joinToGroupArray(this, rIter, "RedCoin", 24);

    // Get Obj_args
    s32 powerStarCheck = 0;
    s32 iconID = 0x37;
    s32 layoutMode = -1;
    MR::getJMapInfoArg0NoInit(rIter, &mRewardCoins); // Should the Red Coin increment the coin counter by 2?
    MR::getJMapInfoArg1NoInit(rIter, &powerStarCheck); // Power Star to check for to set the collected star indicator
    MR::getJMapInfoArg2NoInit(rIter, &iconID); // PictureFont.brfnt entry to display
    MR::getJMapInfoArg3NoInit(rIter, &layoutMode);

    // Initialize the RedCoinCounter
    mRedCoinCounter = new RedCoinCounter("RedCoinCounter");
    mRedCoinCounter->initWithoutIter();
    mRedCoinCounter->setStarIcon(powerStarCheck, iconID);

    // Initialize RedCoinCounterPlayer
    mRedCoinPlayerCounter = new RedCoinCounterPlayer("RedCoinCounterPlayer");
    mRedCoinPlayerCounter->initWithoutIter();
    
    if (layoutMode == 0) {
        mRedCoinCounter->mLayoutMode = 0;
        mRedCoinCounter->appear();
    }
}

void RedCoinController::movement() {    
    calcCounterVisibility();

    if (mHasAllRedCoins)
        mElapsed++; // There may be a better way to do this

    if (mElapsed == 140) {
        if (MR::isValidSwitchA(this)) {
            MR::onSwitchA(this);
        }
        
        MR::getGroupFromArray(this)->killAll();
    }
}

// Increases both layouts by 1
void RedCoinController::startCountUp(LiveActor* pRedCoin) {
    mNumCoins++;
    
    mHasAllRedCoins = mNumCoins < MR::getGroupFromArray(this)->mNumObjs - 1 ? 0 : 1;

    mRedCoinCounter->startCountUp(mNumCoins, mHasAllRedCoins);

    mRedCoinPlayerCounter->mLastRedCoin = pRedCoin;
    mRedCoinPlayerCounter->mNumCoins = mNumCoins;
    mRedCoinPlayerCounter->appear();
}

void RedCoinController::calcCounterVisibility() {
    bool blueCoin = false;

    #if defined USEBLUECOIN && !defined SM64BLUECOIN
        blueCoin = BlueCoinUtil::isBlueCoinTextBoxAppeared();

        if (blueCoin) {
            requestResume();
            mRedCoinCounter->requestResume();
        }
    #endif

    if (MR::isPowerStarGetDemoActive() || MR::isDemoActive() || MR::isPlayerDead() || MR::isTimeKeepDemoActive() || MR::isNormalTalking() || MR::isSystemTalking() || blueCoin)
        MR::hideLayout(mRedCoinCounter);
    else
        MR::showLayout(mRedCoinCounter);
}

// This function is unused, but there are plans for
// this function to be included if it can be figured out.

//void RedCoinController::resetAllRedCoins() {
//    LiveActorGroup* group = MR::getGroupFromArray(this);
//
//    for (s32 i = 0; i < group->mNumObjs; i++) {
//        if (!strcmp(group->getActor(i)->mName, "RedCoin")) {
//            RedCoin* coin = ((RedCoin*)group->getActor(i));
//            MR::hideModel(coin);
//            MR::invalidateShadowAll(coin);
//            MR::invalidateHitSensors(coin);
//
//            coin->mIsCollected = false;
//
//            if (coin->mIsInAirBubble)
//                coin->mAirBubble->makeActorAppeared();
//        }
//    }
//
//    mNumCoins = 0;
//    mIsValidCounterAppear = false;
//    MR::offSwitchB(this);
//    MR::stopAnim(mRedCoinPlayerCounter, 0);
//    MR::startSystemSE("SE_SY_TIMER_A_0", -1, -1);
//    MR::hideLayout(mRedCoinCounter);
//    MR::setTextBoxNumberRecursive(mRedCoinCounter, "Counter", 0);
//}