#if defined (ALL) || defined (SMSS)
#include "pt/MapObj/BlueCoinSystem/BlueCoinUtil.h"
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
    //if (MR::isHiddenLayout(this)) {
    //    MR::startAnim(this, "Appear", 0);
    //    MR::startAnim(this, "Wait", 1);
    //}

    if (!MR::isActiveDefaultGameLayout())
        MR::activateDefaultGameLayout();

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


/* 
/    BLUE COIN COUNTERS
/
/    Pause Menu Stage Counter
/    Pause Menu Total Counter
/    Galaxy Banner Stage Counter
/    File Select Info Total Counter
*/

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
        BlueCoinUtil::showAmountPaneFromBcsv(actor, MR::getCurrentStageName());
    }
    else
        MR::hidePaneRecursive(actor, "ShaBlueCoinStage");
}

kmCall(0x80487188, setPauseMenuBlueCoinCount);

#ifdef ALL
void initGalaxyInfoBlueCoinCount(LayoutActor* actor) {
    MR::registerDemoSimpleCastAll(actor);
    MR::setTextBoxFormatRecursive(actor, "ShaBlueCoinGalaxyInfo", counterPictureFonts);
}

kmCall(0x804A952C, initGalaxyInfoBlueCoinCount);
#endif

void setGalaxyInfoBlueCoinCount(LayoutActor* actor, const char* pGalaxyName, const wchar_t* pWStr) {

    #ifdef SMSS
    MR::setTextBoxFormatRecursive(actor, "ShaBlueCoinGalaxyInfo", counterPictureFonts);
    #endif

    MR::hidePaneRecursive(actor, "BlueCoin");
    s32 rangeNum = BlueCoinUtil::getTotalBlueCoinRangeNumFromBcsv(pGalaxyName);

    if (rangeNum > -1) {
        MR::showPane(actor, "BlueCoin");
        MR::showPaneRecursive(actor, "ShaBlueCoinGalaxyInfo");
        MR::setTextBoxArgNumberRecursive(actor, "ShaBlueCoinGalaxyInfo", rangeNum, 0);
        BlueCoinUtil::showAmountPaneFromBcsv(actor, pGalaxyName);
    }

    MR::setTextBoxMessageRecursive(actor, "GalaxyName", pWStr);
}

#ifdef ALL
kmWrite32(0x804A95E4, 0x809B003C);
kmCall(0x804A95E8, setGalaxyInfoBlueCoinCount);
#endif

#ifdef SMSS
kmWrite32(0x804E0BE4, 0x809B003C);
kmCall(0x804E0BE8, setGalaxyInfoBlueCoinCount);
#endif

void initBlueCoinCounterFileInfo(LayoutActor* pLayout) {
    MR::connectToSceneLayout(pLayout);
    MR::setTextBoxFormatRecursive(pLayout, "ShaBlueCoinFileInfo", counterPictureFonts);
}

kmCall(0x8046D908, initBlueCoinCounterFileInfo);

void setBlueCoinCounterFileInfo(LayoutActor* pLayout, const Nerve* pNerve) {
    s32 fileID = 1;
    asm("lwz %0, 0x2C(r31)" : "=r" (fileID));

    MR::setTextBoxArgNumberRecursive(pLayout, "ShaBlueCoinFileInfo", BlueCoinUtil::getTotalBlueCoinNum(fileID - 1), 0);
    pLayout->setNerve(pNerve);
}

kmCall(0x8046D9BC, setBlueCoinCounterFileInfo);
#endif