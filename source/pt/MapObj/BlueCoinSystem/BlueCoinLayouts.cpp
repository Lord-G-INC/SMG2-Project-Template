#ifdef SMSS
#include "pt/MapObj/BlueCoinSystem/BlueCoinUtil.h"
#include "pt/MapObj/BlueCoinSystem/BlueCoinLayouts.h"
#include "Game/Screen/GameSceneLayoutHolder.h"

// HUD

BlueCoinCounter::BlueCoinCounter(const char* pName) : LayoutActor(pName, 0) {
    mWaitTime = -1;
}

void BlueCoinCounter::init(const JMapInfoIter& rIter) {
    initLayoutManager("BlueCoinCounterInStage", 2);
    initEffectKeeper(0, 0, 0);
    MR::registerDemoSimpleCastAll(this);
    MR::connectToSceneLayout(this);
    MR::createAndAddPaneCtrl(this, "Counter", 1);
    MR::setTextBoxNumberRecursive(this, "ShaNumber", BlueCoinUtil::getTotalBlueCoinNumCurrentFile());

    mAppearer = new CounterLayoutAppearer(this, TVec2f(-50.0f, 0.0f));

    mPaneRumbler = new CountUpPaneRumbler(this, "Counter");
    mPaneRumbler->mRumbleCalculator->mRumbleStrength = 8.0f;
    mPaneRumbler->reset();

    appear();
    initNerve(&NrvBlueCoinCounter::NrvDisappear::sInstance);
    MR::hideLayout(this);
}

void BlueCoinCounter::control() {
    if (mWaitTime > 0)
        mWaitTime--;

    if (mWaitTime == 0)
        setNerve(&NrvBlueCoinCounter::NrvDisappear::sInstance);

    mAppearer->updateNerve();
    mPaneRumbler->update();
}

void BlueCoinCounter::exeAppear() {
    if (MR::isFirstStep(this) || MR::isStep(this, 1) && mAppearer->isDisappeared()) {
        if (!mAppearer->isAppeared()) {
            MR::showLayout(this);
            mAppearer->appear(TVec2f(0.0f, 0.0f));
            MR::startAnim(this, "Wait", 1);
        }

        if (mWaitTime == 0)
            mWaitTime = -1;
    }
}
  
void BlueCoinCounter::exeDisappear() {
    if (MR::isFirstStep(this)) {
        mAppearer->disappear();
        mWaitTime = -1;
    }

    if (MR::isStep(this, 10))
        MR::hideLayout(this);
}

void BlueCoinCounter::startCountUp() {  
    if (mAppearer->isDisappeared()) {
        setNerve(&NrvBlueCoinCounter::NrvAppearAndUpdate::sInstance);
        mWaitTime = 120;
    }
    else {
        updateCounter();

        if (mWaitTime > 0)
            mWaitTime = 120;
    }
}

void BlueCoinCounter::exeAppearAndUpdate() {
    if (MR::isFirstStep(this))
        exeAppear();

    if (MR::isStep(this, 15))
        updateCounter();
}

void BlueCoinCounter::updateCounter() {
    MR::setTextBoxNumberRecursive(this, "Counter", BlueCoinUtil::getTotalBlueCoinNumCurrentFile());
    MR::startPaneAnim(this, "Counter", "Flash", 0);
    mPaneRumbler->start();
}

namespace NrvBlueCoinCounter {
	void NrvAppear::execute(Spine* pSpine) const {
        ((BlueCoinCounter*)pSpine->mExecutor)->exeAppear();
	}

	void NrvDisappear::execute(Spine* pSpine) const {
        ((BlueCoinCounter*)pSpine->mExecutor)->exeDisappear();
	}

	void NrvAppearAndUpdate::execute(Spine* pSpine) const {
        ((BlueCoinCounter*)pSpine->mExecutor)->exeAppearAndUpdate();
	}

	NrvAppear(NrvAppear::sInstance);
    NrvDisappear(NrvDisappear::sInstance);
    NrvAppearAndUpdate(NrvAppearAndUpdate::sInstance);
}

// Add 0x4 to CounterLayoutController for BlueCoinCounter ptr
kmWrite32(0x80471780, 0x38600050);

void initBlueCoinLayout(CounterLayoutController* pController) {
    MR::connectToSceneLayout(pController);
    
    if (!MR::isStageFileSelect()) {
        pController->mPTDBlueCoinCounter = new BlueCoinCounter("BlueCoinCounter");
        pController->mPTDBlueCoinCounter->initWithoutIter();
    }
}

kmCall(0x804657A0, initBlueCoinLayout);

void appearBlueCoinLayout(CounterLayoutController* pController) {
    if (!MR::isStageFileSelect()) {
        ((BlueCoinCounter*)pController->mPTDBlueCoinCounter)->mWaitTime = -1;
        pController->mPTDBlueCoinCounter->setNerve(&NrvBlueCoinCounter::NrvAppear::sInstance);
    } 

        pController->showAllLayout();
}

kmCall(0x80466128, appearBlueCoinLayout);

void disappearBlueCoinLayout(CounterLayoutController* pController) {
    if (!MR::isStageFileSelect())
        pController->mPTDBlueCoinCounter->setNerve(&NrvBlueCoinCounter::NrvDisappear::sInstance);
        
    pController->hideAllLayout();
}

kmCall(0x80466198, disappearBlueCoinLayout);

void killBlueCoinCounter(CounterLayoutController* pController) {
    if (!MR::isStageFileSelect()) {
        MR::hideLayout(pController->mPTDBlueCoinCounter);
        pController->mPTDBlueCoinCounter->setNerve(&NrvBlueCoinCounter::NrvDisappear::sInstance);
    }

    pController->killAllCoounter();
}

kmCall(0x8046590C, killBlueCoinCounter);

// PAUSE MENU

void initBlueCoinCounters(LayoutActor* actor) {
    MR::connectToSceneLayoutOnPause(actor);

    MR::setTextBoxFormatRecursive(actor, "ShaBlueCoinTotal", counterPictureFonts);
    MR::setTextBoxFormatRecursive(actor, "ShaBlueCoinStage", counterPictureFonts);
}

kmCall(0x80486D60, initBlueCoinCounters);

void setPauseMenuBlueCoinCount(LayoutActor* actor, const char* pStr, s32 l) {
    MR::setTextBoxArgNumberRecursive(actor, pStr, l, 0);
    
    s32 rangeCollected = BlueCoinUtil::getBlueCoinRangeData(0, true);
    s32 rangeTotal = BlueCoinUtil::getBlueCoinRangeData(0, false);

    MR::setTextBoxArgNumberRecursive(actor, "ShaBlueCoinTotal", BlueCoinUtil::getTotalBlueCoinNumCurrentFile(), 0);

    if (rangeCollected > -1) {
        MR::setTextBoxArgNumberRecursive(actor, "ShaBlueCoinStage", rangeCollected, 0);
    
        MR::showPane(actor, "StageInfo");
        MR::showPaneRecursive(actor, "ShaBlueCoinStage");

        if (rangeTotal > -1) {
            char paneName[0xA];
            snprintf(paneName, 0xA, "ShaCoin%d", rangeTotal);
            MR::showPaneRecursive(actor, paneName);
        }
    }
    else
        MR::hidePaneRecursive(actor, "ShaBlueCoinStage");
}

kmCall(0x80487188, setPauseMenuBlueCoinCount);

// FILE INFO

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