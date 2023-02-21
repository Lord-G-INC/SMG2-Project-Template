#include "pt/MapObj/BlueCoinSystem/BlueCoinManager.h"
#include "pt/MapObj/BlueCoinSystem/BlueCoinLayouts.h"
#include "Game/Screen/GameSceneLayoutHolder.h"

BlueCoinCounter::BlueCoinCounter(const char* pName) : LayoutActor(pName, 0) {}

void BlueCoinCounter::init(const JMapInfoIter& rIter) {
    initLayoutManager("BlueCoinCounterInStage", 2);
    initEffectKeeper(0, 0, 0);

    MR::createAndAddPaneCtrl(this, "Counter", 1);

    OSReport("Blue Coin Range Value: %d\n", BlueCoinUtil::getTotalBlueCoinRangeNumFromBcsv(BlueCoinUtil::getCurrentFileNum(), MR::getCurrentStageName()));

    if (BlueCoinUtil::getTotalBlueCoinNum(BlueCoinUtil::getCurrentFileNum()) > 0)
        MR::setTextBoxNumberRecursive(this, "ShaNumber", BlueCoinUtil::getTotalBlueCoinNum(BlueCoinUtil::getCurrentFileNum()));
    else
        MR::setTextBoxNumberRecursive(this, "ShaNumber", 0);

    MR::registerDemoSimpleCastAll(this);
    MR::connectToSceneLayout(this);

    mPaneRumbler = new CountUpPaneRumbler(this, "Counter");
    mPaneRumbler->mRumbleCalculator->mRumbleStrength = 8.0f;
    mPaneRumbler->reset();
}

kmWrite32(0x80471780, 0x38600050);

void initBlueCoinLayoutInStage(CounterLayoutController* pController) {
    MR::connectToSceneLayout(pController);

    pController->mPTDBlueCoinCounter = new BlueCoinCounter("BCoinCounter");
    pController->mPTDBlueCoinCounter->initWithoutIter();
}

kmCall(0x804657A0, initBlueCoinLayoutInStage);

void appearBlueCoinLayout(CounterLayoutController* pController) {
    pController->mPTDBlueCoinCounter->appear();
    pController->showAllLayout();
}

kmCall(0x80466128, appearBlueCoinLayout);

void disappearBlueCoinLayout(CounterLayoutController* pController) {
    pController->mPTDBlueCoinCounter->kill();
    pController->hideAllLayout();
}

kmCall(0x80466198, disappearBlueCoinLayout);