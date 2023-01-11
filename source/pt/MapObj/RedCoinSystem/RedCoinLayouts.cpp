#include "pt/MapObj/RedCoinSystem/RedCoinLayouts.h"

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

    MR::registerDemoSimpleCastAll(this);
    MR::connectToSceneLayout(this);
    MR::hideLayout(this);

    mPaneRumbler = new CountUpPaneRumbler(this, "Counter");
    mPaneRumbler->mRumbleCalculator->mRumbleStrength = 8.0f;
    mPaneRumbler->reset();
}

void RedCoinCounter::control() {
    mPaneRumbler->update();
}

void RedCoinCounter::calcVisibility() {
    if (MR::isPowerStarGetDemoActive() || MR::isDemoActive() || MR::isPlayerDead() || MR::isTimeKeepDemoActive() || MR::isNormalTalking() || MR::isSystemTalking())
        MR::hideLayout(this);
    else {
        if (mIsValidAppear)
            appearIfHidden();
    }
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

void RedCoinCounter::updateStarIndicator(s32 starID, s32 iconID) {
    wchar_t str;
    MR::addPictureFontCode(&str, MR::hasPowerStarInCurrentStage(starID) ? iconID : 0x52);
    MR::setTextBoxFormatRecursive(this, "TxtCount", &str);
}

/* --- RED COIN COUNTER PLAYER --- */

/*
* Layout created by RedCoin that displays the current
* number of red coins collected on collection of a 
* Red Coin, and the position of the layout is at
* the collected coin.
*/


RedCoinCounterPlayer::RedCoinCounterPlayer(const char* pName) : LayoutActor(pName, 1) {
}

void RedCoinCounterPlayer::init(const JMapInfoIter& rIter) {
    initLayoutManager("RedCoinCounterPlayer", 1);
    MR::connectToSceneLayout(this);
    MR::setTextBoxNumberRecursive(this, "TxtText", 0);
    MR::registerDemoSimpleCastAll(this);
}

void RedCoinCounterPlayer::control() {
    MR::killAtAnimStopped(this, 0);
}

void RedCoinCounterPlayer::calcScreenPos(LiveActor* pActor, bool type) {
    TVec2f screenPos;
    TVec3f newPos;
    TVec3f pos = type ? *((MarioActor*)pActor)->getGravityVec() : pActor->mGravity;
    f32 heightAdd = type ? 250.0f : 150.0f;
    
    JMAVECScaleAdd((Vec*)&pos, (Vec*)&pActor->mTranslation, (Vec*)&newPos, -heightAdd);
    MR::calcScreenPosition(&screenPos, newPos);
    setTrans(screenPos);
}

void RedCoinCounterPlayer::updateCounter(s32 count) {
    MR::setTextBoxNumberRecursive(this, "TxtText", count);
    MR::startAnim(this, "Appear", 0);
    appear();
}