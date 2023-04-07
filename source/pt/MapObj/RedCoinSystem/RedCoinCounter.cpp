#include "pt/MapObj/RedCoinSystem/RedCoinCounter.h"

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