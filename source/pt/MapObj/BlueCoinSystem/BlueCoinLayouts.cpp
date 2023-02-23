#include "pt/MapObj/BlueCoinSystem/BlueCoinManager.h"
#include "pt/MapObj/BlueCoinSystem/BlueCoinLayouts.h"
#include "Game/Screen/GameSceneLayoutHolder.h"

BlueCoinCounter::BlueCoinCounter(const char* pName) : LayoutActor(pName, 0) {
}

void BlueCoinCounter::init(const JMapInfoIter& rIter) {
    initLayoutManager("BlueCoinCounterInStage", 2);
    initEffectKeeper(0, 0, 0);

    MR::createAndAddPaneCtrl(this, "Counter", 1);

    OSReport("Blue Coin Range Value: %d\n", BlueCoinUtil::getTotalBlueCoinRangeNumFromBcsv(BlueCoinUtil::getCurrentFileNum(), MR::getCurrentStageName()));

    MR::setTextBoxNumberRecursive(this, "ShaNumber", BlueCoinUtil::getTotalBlueCoinNum(BlueCoinUtil::getCurrentFileNum()));

    MR::registerDemoSimpleCastAll(this);
    MR::connectToSceneLayout(this);

    mPaneRumbler = new CountUpPaneRumbler(this, "Counter");
    mPaneRumbler->mRumbleCalculator->mRumbleStrength = 8.0f;
    mPaneRumbler->reset();
}

void BlueCoinCounter::control() {
    mPaneRumbler->update();
}

void BlueCoinCounter::exeAppear() {
    MR::showLayout(this);
    MR::startAnim(this, "Appear", 0);
    MR::startAnim(this, "Wait", 1);
    appear();
}

void BlueCoinCounter::exeDisappear() {
    MR::startAnim(this, "End", 0);
    bool die = true;
}

void BlueCoinCounter::incCounter() {
    if (MR::isHiddenLayout(this)) {
        MR::startAnim(this, "Appear", 0);
        MR::startAnim(this, "Wait", 1);
    }

    MR::showLayout(this);
    MR::setTextBoxNumberRecursive(this, "Counter", BlueCoinUtil::getTotalBlueCoinNum(BlueCoinUtil::getCurrentFileNum()));
    MR::startPaneAnim(this, "Counter", "Flash", 0);
    mPaneRumbler->start();
}

kmWrite32(0x80471780, 0x38600050);

void initBlueCoinLayoutInStage(CounterLayoutController* pController) {
    MR::connectToSceneLayout(pController);

    pController->mPTDBlueCoinCounter = new BlueCoinCounter("BlueCoinCounter");
    
    if (!MR::isEqualStageName("FileSelect")) {
        pController->mPTDBlueCoinCounter->initWithoutIter();
    }
}

kmCall(0x804657A0, initBlueCoinLayoutInStage);

void appearBlueCoinLayout(CounterLayoutController* pController) {
    if (!MR::isEqualStageName("FileSelect")) {
    ((BlueCoinCounter*)pController->mPTDBlueCoinCounter)->exeAppear();
    pController->showAllLayout();
    }
}

kmCall(0x80466128, appearBlueCoinLayout);

void disappearBlueCoinLayout(CounterLayoutController* pController) {
    if (!MR::isEqualStageName("FileSelect")) {
    ((BlueCoinCounter*)pController->mPTDBlueCoinCounter)->exeDisappear();
    pController->hideAllLayout();
    }
}

kmCall(0x80466198, disappearBlueCoinLayout);