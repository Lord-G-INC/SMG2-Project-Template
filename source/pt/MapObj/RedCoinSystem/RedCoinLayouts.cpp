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

void RedCoinCounter::updateStarIndicator(s32 starID, s32 iconID) {
    wchar_t* str = L"";
    MR::addPictureFontCode(str, MR::hasPowerStarInCurrentStage(starID) ? iconID : 0x52);
    MR::setTextBoxFormatRecursive(this, "TxtCount", str);
}

/* --- RED COIN COUNTER PLAYER --- */

/*
* Layout created by RedCoin that displays the current
* number of red coins collected on collection of a 
* Red Coin, and the position of the layout is at
* the collected coin.
*/

RedCoinCounterPlayer::RedCoinCounterPlayer(const char* pName) : LayoutActor(pName, 1) {
    bool mLytPos;
}

void RedCoinCounterPlayer::init(const JMapInfoIter& rIter) {
    initLayoutManager("RedCoinCounterPlayer", 1);
    MR::connectToSceneLayout(this);
    MR::setTextBoxNumberRecursive(this, "TxtText", 0);
}

void RedCoinCounterPlayer::calcScreenPos(LiveActor* pActor) {
    TVec2f screenPos;
    TVec3f pos = mLytPos ? *MR::getPlayerPos() : pActor->mTranslation;
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
