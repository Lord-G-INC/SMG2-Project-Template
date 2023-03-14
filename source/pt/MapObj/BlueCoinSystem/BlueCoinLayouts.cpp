//#if defined (ALL) || defined (SMSS)
#include "pt/MapObj/BlueCoinSystem/BlueCoinUtil.h"
#include "pt/MapObj/BlueCoinSystem/BlueCoinLayouts.h"
#include "Game/Screen/GameSceneLayoutHolder.h"

BlueCoinCounter::BlueCoinCounter(const char* pName) : LayoutActor(pName, 0) {}

void BlueCoinCounter::init(const JMapInfoIter& rIter) {
    initLayoutManager("BlueCoinCounterInStage", 2);
    initEffectKeeper(0, 0, 0);

    MR::createAndAddPaneCtrl(this, "Counter", 1);

    MR::setTextBoxNumberRecursive(this, "ShaNumber", BlueCoinUtil::getTotalBlueCoinNum(BlueCoinUtil::getCurrentFileNum()));

    MR::registerDemoSimpleCastAll(this);

    mAppearer = new CounterLayoutAppearer(this, TVec2f(-50.0f, 0.0f));

    mPaneRumbler = new CountUpPaneRumbler(this, "Counter");
    mPaneRumbler->mRumbleCalculator->mRumbleStrength = 8.0f;
    mPaneRumbler->reset();

    MR::connectToSceneLayout(this);
    appear();
    initNerve(&NrvBlueCoinCounter::NrvHide::sInstance);
    MR::hideLayout(this);
}

void BlueCoinCounter::control() {
    mAppearer->updateNerve();
    mPaneRumbler->update();
}

void BlueCoinCounter::exeAppear() {
    if (MR::isFirstStep(this)) {
        MR::showLayout(this);

        if (!isNerve(&NrvBlueCoinCounter::NrvUpdateCounter::sInstance)) {
            mAppearer->appear(TVec2f(0.0f, 0.0f));
            MR::startAnim(this, "Wait", 1);
        }
    }
}

void BlueCoinCounter::exeDisappear() {
    if (MR::isFirstStep(this))
        mAppearer->disappear();

    if (MR::isStep(this, 10))
        MR::hideLayout(this);
}

void BlueCoinCounter::incCounter() {
    MR::setTextBoxNumberRecursive(this, "Counter", BlueCoinUtil::getTotalBlueCoinNumCurrentFile());
    MR::startPaneAnim(this, "Counter", "Flash", 0);
    mPaneRumbler->start();

    if (isNerve(&NrvBlueCoinCounter::NrvHide::sInstance))
        setNerve(&NrvBlueCoinCounter::NrvUpdateCounter::sInstance);
}

void BlueCoinCounter::updateCounter() {
    if (MR::isFirstStep(this)) {
        MR::showLayout(this);
        mAppearer->appear(TVec2f(0.0f, 0.0f));
    }

    if (MR::isStep(this, 120))
        setNerve(&NrvBlueCoinCounter::NrvHide::sInstance);
}

// Add 0x4 to CounterLayoutController for BlueCoinCounter ptr
kmWrite32(0x80471780, 0x38600050);

// Blue Coin Counter nerves

namespace NrvBlueCoinCounter {
	void NrvShow::execute(Spine* pSpine) const {
		BlueCoinCounter* pActor = (BlueCoinCounter*)pSpine->mExecutor;
        pActor->exeAppear();
	}

	void NrvHide::execute(Spine* pSpine) const {
		BlueCoinCounter* pActor = (BlueCoinCounter*)pSpine->mExecutor;
        pActor->exeDisappear();
	}

	void NrvUpdateCounter::execute(Spine* pSpine) const {
		BlueCoinCounter* pActor = (BlueCoinCounter*)pSpine->mExecutor;
        pActor->updateCounter();
	}

	NrvShow(NrvShow::sInstance);
    NrvHide(NrvHide::sInstance);
    NrvUpdateCounter(NrvUpdateCounter::sInstance);
}

// BlueCoinCounter hooks

void initBlueCoinLayout(CounterLayoutController* pController) {
    MR::connectToSceneLayout(pController);

    pController->mPTDBlueCoinCounter = new BlueCoinCounter("BlueCoinCounter");
    pController->mPTDBlueCoinCounter->initWithoutIter();
    MR::hideLayout(pController->mPTDBlueCoinCounter);
}

kmCall(0x804657A0, initBlueCoinLayout);

void appearBlueCoinLayout(CounterLayoutController* pController) {
    pController->mPTDBlueCoinCounter->setNerve(&NrvBlueCoinCounter::NrvShow::sInstance);
    pController->showAllLayout();
}

kmCall(0x880466128, appearBlueCoinLayout);

void disappearBlueCoinLayout(CounterLayoutController* pController) {
    pController->mPTDBlueCoinCounter->setNerve(&NrvBlueCoinCounter::NrvHide::sInstance);
    pController->hideAllLayout();
}

kmCall(0x80466198, disappearBlueCoinLayout);

void killBlueCoinCounter(CounterLayoutController* pController) {
    MR::hideLayout(pController->mPTDBlueCoinCounter);
    pController->killAllCoounter();
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


// Disabled since GLE does not want to play nice

//#ifdef ALL
//void initGalaxyInfoBlueCoinCount(LayoutActor* actor) {
//    MR::registerDemoSimpleCastAll(actor);
//    MR::setTextBoxFormatRecursive(actor, "ShaBlueCoinGalaxyInfo", counterPictureFonts);
//}
//
//kmCall(0x804A952C, initGalaxyInfoBlueCoinCount);
//#endif
//
//void setGalaxyInfoBlueCoinCount(LayoutActor* actor, const char* pGalaxyName, const wchar_t* pWStr) {
//
//    #ifdef SMSS
//    MR::setTextBoxFormatRecursive(actor, "ShaBlueCoinGalaxyInfo", counterPictureFonts);
//    #endif
//
//    MR::hidePaneRecursive(actor, "BlueCoin");
//    s32 rangeNum = BlueCoinUtil::getTotalBlueCoinRangeNumFromBcsv(pGalaxyName);
//
//    if (rangeNum > -1) {
//        MR::showPane(actor, "BlueCoin");
//        MR::showPaneRecursive(actor, "ShaBlueCoinGalaxyInfo");
//        MR::setTextBoxArgNumberRecursive(actor, "ShaBlueCoinGalaxyInfo", rangeNum, 0);
//        BlueCoinUtil::showAmountPaneFromBcsv(actor, pGalaxyName);
//    }
//
//    MR::setTextBoxMessageRecursive(actor, "GalaxyName", pWStr);
//}
//
//#ifdef ALL
//kmWrite32(0x804A95E4, 0x809B003C);
//kmCall(0x804A95E8, setGalaxyInfoBlueCoinCount);
//#endif
//
//#ifdef SMSS
//kmWrite32(0x804E0BE4, 0x809B003C);
//kmCall(0x804E0BE8, setGalaxyInfoBlueCoinCount);
//#endif

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
//#endif