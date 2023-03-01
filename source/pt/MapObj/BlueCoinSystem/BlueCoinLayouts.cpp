#include "pt/MapObj/BlueCoinSystem/BlueCoinManager.h"
#include "pt/MapObj/BlueCoinSystem/BlueCoinLayouts.h"
#include "Game/Screen/GameSceneLayoutHolder.h"

BlueCoinCounter::BlueCoinCounter(const char* pName) : LayoutActor(pName, 0) {
}

void BlueCoinCounter::init(const JMapInfoIter& rIter) {
    initLayoutManager("BlueCoinCounterInStage", 2);
    initEffectKeeper(0, 0, 0);

    MR::createAndAddPaneCtrl(this, "Counter", 1);

    OSReport("Blue Coin Range Value: %d\n", BlueCoinUtil::getTotalBlueCoinRangeNumFromBcsv(MR::getCurrentStageName()));

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

// Add 0x4 to CounterLayoutController for BlueCoinCounter ptr
kmWrite32(0x80471780, 0x38600050);


// BlueCoinCounter hooks

void initBlueCoinLayout(CounterLayoutController* pController) {
    MR::connectToSceneLayout(pController);

    pController->mPTDBlueCoinCounter = new BlueCoinCounter("BlueCoinCounter");
    pController->mPTDBlueCoinCounter->initWithoutIter();
}

kmCall(0x804657A0, initBlueCoinLayout);

void appearBlueCoinLayout(CounterLayoutController* pController) {
    ((BlueCoinCounter*)pController->mPTDBlueCoinCounter)->exeAppear();
    pController->showAllLayout();
}

kmCall(0x80466128, appearBlueCoinLayout);

void appearBlueCoinLayoutWithoutStar(CounterLayoutController* pController) {
    ((BlueCoinCounter*)pController->mPTDBlueCoinCounter)->exeAppear();
    pController->appearAllCounterWithoutStar();
}

kmCall(0x804658B8, appearBlueCoinLayoutWithoutStar);

void disappearBlueCoinLayout(CounterLayoutController* pController) {
    ((BlueCoinCounter*)pController->mPTDBlueCoinCounter)->exeDisappear();
    pController->hideAllLayout();
}

kmCall(0x80466198, disappearBlueCoinLayout);

void killBlueCoinCounter(CounterLayoutController* pController) {
    pController->mPTDBlueCoinCounter->kill();
    pController->killAllCoounter(); // Yes, this function is spelled wrong
}
kmCall(0x8046590C, killBlueCoinCounter);

const wchar_t counterPictureFonts[] = {
0x000E, 0x0003, 0x0055, 0x0002, 0x005A, // Blue Coin Icon
0x000E, 0x0003, 0x0010, 0x0002, 0x0010, // X Icon
0x000E, 0x0006, 0x0001, 0x0008, 0x0000, // Counter
0x0000, 0x0000, 0x0000, 0x0000
};

void initBlueCoinCounters(LayoutActor* actor) {
    MR::connectToSceneLayoutOnPause(actor);

    MR::setTextBoxFormatRecursive(actor, "ShaBlueCoinTotal", counterPictureFonts);
    MR::setTextBoxFormatRecursive(actor, "ShaBlueCoinStage", counterPictureFonts);
}

kmCall(0x80486D60, initBlueCoinCounters);

void setPauseMenuBlueCoinCount(LayoutActor* actor, const char* pStr, s32 l) {
    MR::setTextBoxArgNumberRecursive(actor, pStr, l, 0);
    s32 rangeCount = BlueCoinUtil::getTotalBlueCoinRangeNumFromBcsv(MR::getCurrentStageName());

    MR::setTextBoxArgNumberRecursive(actor, "ShaBlueCoinTotal", BlueCoinUtil::getTotalBlueCoinNumCurrentFile(), 0);

    if (rangeCount > -1) {
        MR::setTextBoxArgNumberRecursive(actor, "ShaBlueCoinStage", rangeCount, 0);
    
        MR::showPane(actor, "StageInfo");
        MR::showPaneRecursive(actor, "ShaBlueCoinStage");
        BlueCoinUtil::calcBlueCoinTexFromBcsv(actor, MR::getCurrentStageName());
    }
    else
        MR::hidePaneRecursive(actor, "ShaBlueCoinStage");
}

kmCall(0x80487188, setPauseMenuBlueCoinCount);

void initGalaxyInfoBlueCoinCount(LayoutActor* actor) {\
    MR::registerDemoSimpleCastAll(actor);
    MR::setTextBoxFormatRecursive(actor, "ShaBlueCoinStage", counterPictureFonts);
}

kmCall(0x804A952C, initGalaxyInfoBlueCoinCount);

// Modifies GalaxyInfo to add the Blue Coin Counter to it.
// Works on vanilla but not the GLE.

void setGalaxyInfoBlueCoinCount(LayoutActor* actor, const char* pGalaxyName, const wchar_t* pWStr) {
    MR::setTextBoxMessageRecursive(actor, "GalaxyName", pWStr);
    s32 rangeNum = BlueCoinUtil::getTotalBlueCoinRangeNumFromBcsv(pGalaxyName);
    MR::hidePaneRecursive(actor, "BlueCoin");

    if (rangeNum > -1) {
        MR::showPane(actor, "BlueCoin");
        MR::showPaneRecursive(actor, "ShaBlueCoinStage");
        MR::setTextBoxArgNumberRecursive(actor, "ShaBlueCoinStage", rangeNum, 0);
        BlueCoinUtil::calcBlueCoinTexFromBcsv(actor, pGalaxyName);
    }
}

kmWrite32(0x804A95E4, 0x809B003C);
kmCall(0x804A95E8, setGalaxyInfoBlueCoinCount);