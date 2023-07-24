#include "pt/MapObj/RedCoinSystem/RedCoinController.h"
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

    mLastRedCoin = this;

    mRewardCoins = false;
    mPowerStarCheck = 0;
    mIconID = 0x37;
    mIsValidCounterAppear = false;
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
    MR::getJMapInfoArg0NoInit(rIter, &mRewardCoins); // Should the Red Coin increment the coin counter by 2?
    MR::getJMapInfoArg1NoInit(rIter, &mPowerStarCheck); // Power Star to check for to set the collected star indicator
    MR::getJMapInfoArg2NoInit(rIter, &mIconID); // PictureFont.brfnt entry to display

    // Initialize the RedCoinCounter
    mRedCoinCounter = MR::createSimpleLayout("counter", "RedCoinCounter", 2);
    mRedCoinCounter->initEffectKeeper(0, 0, 0);
    mRedCoinCounter->appear();
    MR::setTextBoxNumberRecursive(mRedCoinCounter, "Counter", 0);
    MR::hideLayout(mRedCoinCounter);

    // Initialize RedCoinCounterPlayer
    mRedCoinCounterPlayer = MR::createSimpleLayout("counternumber", "RedCoinCounterPlayer", 1);
    MR::setTextBoxNumberRecursive(mRedCoinCounterPlayer, "TxtText", 0);
    MR::registerDemoSimpleCastAll(mRedCoinCounterPlayer);

    wchar_t str;
    MR::addPictureFontCode(&str, MR::hasPowerStarInCurrentStage(mPowerStarCheck) ? mIconID : 0x52);
    MR::setTextBoxFormatRecursive(mRedCoinCounter, "TxtStar", &str);

    mRumbler = new CountUpPaneRumbler(mRedCoinCounter, "Counter");
    mRumbler->mRumbleCalculator->mRumbleStrength = 8.0f;
    mRumbler->reset();
}

void RedCoinController::movement() {
    if (!MR::isHiddenLayout(mRedCoinCounterPlayer))
        calcRedCoinCounterPlayerPos();

    mRumbler->update();

    calcCounterVisibility();

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

// This function is unused, but there are plans for
// this function to be included if it can be figured out.

void RedCoinController::resetAllRedCoins() {
    LiveActorGroup* group = MR::getGroupFromArray(this);

    for (s32 i = 0; i < group->mNumObjs; i++) {
        if (!strcmp(group->getActor(i)->mName, "RedCoin")) {
            RedCoin* coin = ((RedCoin*)group->getActor(i));
            MR::hideModel(coin);
            MR::invalidateShadowAll(coin);
            MR::invalidateHitSensors(coin);

            coin->mIsCollected = false;

            if (coin->mIsInAirBubble)
                coin->mAirBubble->makeActorAppeared();
        }
    }

    mNumCoins = 0;
    mIsValidCounterAppear = false;
    MR::offSwitchB(this);
    MR::stopAnim(mRedCoinCounterPlayer, 0);
    MR::startSystemSE("SE_SY_TIMER_A_0", -1, -1);
    MR::hideLayout(mRedCoinCounter);
    MR::setTextBoxNumberRecursive(mRedCoinCounter, "Counter", 0);
}

// Increases both layouts by 1
void RedCoinController::startCountUp(LiveActor* pRedCoin) {
    mNumCoins++;
    
    mLastRedCoin = pRedCoin;
    
    mHasAllRedCoins = mNumCoins < MR::getGroupFromArray(this)->mNumObjs - 1 ? 0 : 1;

    updateCounter();

    calcRedCoinCounterPlayerPos();
    MR::setTextBoxNumberRecursive(mRedCoinCounterPlayer, "TxtText", mNumCoins);
    MR::startAnim(mRedCoinCounterPlayer, "Appear", 0);
    MR::showLayout(mRedCoinCounterPlayer);
    mRedCoinCounterPlayer->appear();
}

void RedCoinController::calcRedCoinCounterPlayerPos() {
    TVec3f pos;
    TVec3f pos2;
    f32 heightAdd;

    if (((RedCoin*)mLastRedCoin)->mRedCoinCounterPlayerPos) {
        pos = *MarioAccess::getPlayerActor()->getGravityVec();
        pos2 = *MR::getPlayerPos();
        heightAdd = 200.0f;
    }
    else {
        pos = mLastRedCoin->mGravity;
        pos2 = mLastRedCoin->mTranslation;
        heightAdd = 150.0f;
    }

    TVec2f screenPos;
    TVec3f newPos;

    JMAVECScaleAdd((Vec*)&pos, (Vec*)&pos2, (Vec*)&newPos, -heightAdd);
    
    MR::calcScreenPosition(&screenPos, newPos);
    mRedCoinCounterPlayer->setTrans(screenPos);
}

void RedCoinController::updateCounter() {
    if (mNumCoins == 1) {
        mIsValidCounterAppear = true;
        appearCounterIfHidden();
    }
    
    MR::setTextBoxNumberRecursive(mRedCoinCounter, "Counter", mNumCoins);
    MR::startPaneAnim(mRedCoinCounter, "Counter", mHasAllRedCoins ? "FlashLoop" : "Flash", 0);
    MR::emitEffect(mRedCoinCounter, "RedCoinCounterLight");
    mRumbler->start();
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