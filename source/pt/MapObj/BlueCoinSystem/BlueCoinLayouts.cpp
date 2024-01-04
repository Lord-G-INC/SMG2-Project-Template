#if defined USEBLUECOIN && !defined SM64BLUECOIN
#include "pt/MapObj/BlueCoinSystem/BlueCoinUtil.h"
#include "pt/MapObj/BlueCoinSystem/BlueCoinLayouts.h"
#include "Game/Screen/GameSceneLayoutHolder.h"
#include "nw4r/ut/TextWriterBase.h"

// HUD

BlueCoinCounter::BlueCoinCounter(const char* pName) : LayoutActor(pName, 0) {
    mWaitTime = -1;
    mSysInfoWindow = 0;
}

void BlueCoinCounter::init(const JMapInfoIter& rIter) {
    initLayoutManager("BlueCoinCounterInStage", 2);
    initEffectKeeper(0, 0, 0);
    MR::registerDemoSimpleCastAll(this);
    MR::connectToSceneLayout(this);
    MR::createAndAddPaneCtrl(this, "Counter", 1);
    MR::setTextBoxNumberRecursive(this, "ShaNumber", BlueCoinUtil::getTotalBlueCoinNumCurrentFile(true));

    mAppearer = new CounterLayoutAppearer(this, TVec2f(-50.0f, 0.0f));

    mPaneRumbler = new CountUpPaneRumbler(this, "Counter");
    mPaneRumbler->mRumbleCalculator->mRumbleStrength = 8.0f;
    mPaneRumbler->reset();

    appear();
    initNerve(&NrvBlueCoinCounter::NrvDisappear::sInstance);
    MR::hideLayout(this);

    if (!BlueCoinUtil::hasSeenBlueCoinTextBoxCurrentFile()) {
        mSysInfoWindow = MR::createSysInfoWindowMiniExecuteWithChildren();
        MR::connectToSceneLayout(mSysInfoWindow);
        MR::registerDemoSimpleCastAll(mSysInfoWindow);
    }
}

void BlueCoinCounter::control() {
    if (mWaitTime > 0)
        mWaitTime--;

    if (mWaitTime == 0 && !isNerve(&NrvBlueCoinCounter::NrvShowTextBox::sInstance))
        setNerve(&NrvBlueCoinCounter::NrvDisappear::sInstance);

    mAppearer->updateNerve();
    mPaneRumbler->update();

    const wchar_t* pStr = L"123";
    nw4r::ut::TextWriterBase<wchar_t> writer = nw4r::ut::TextWriterBase<wchar_t>();
    writer.SetFont(*MR::getFontOnCurrentLanguage());
    writer.SetFontSize(32.0f);
    writer.SetupGX();
    OSReport("X: %f, Y: %f, len: %f\n", writer.GetCursorX(), writer.GetCursorY(), writer.Printf(pStr));
    writer.~TextWriterBase();
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
    if (BlueCoinUtil::hasSeenBlueCoinTextBoxCurrentFile()) {
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
    else
        setNerve(&NrvBlueCoinCounter::NrvShowTextBox::sInstance);
}   

void BlueCoinCounter::exeAppearAndUpdate() {
    if (MR::isFirstStep(this))
        exeAppear();

    if (MR::isStep(this, 15))
        updateCounter();
}

void BlueCoinCounter::updateCounter() {
    MR::setTextBoxNumberRecursive(this, "Counter", BlueCoinUtil::getTotalBlueCoinNumCurrentFile(true));
    MR::startPaneAnim(this, "Counter", "Flash", 0);
    mPaneRumbler->start();
}

void BlueCoinCounter::exeShowTextBox() {
    if (MR::isStep(this, 3)) {
        mAppearer->disappear();
        mWaitTime = -1;
        mSysInfoWindow->appear("BlueCoinCounter_OnFirstBlueCoin", SysInfoWindow::SysInfoType_0, SysInfoWindow::SysInfoTextPos_0, SysInfoWindow::SysInfoMessageType_1);
        MR::deactivateDefaultGameLayout();
        MR::hideLayout(this);
        MR::suspendAllSceneNameObj();
        mSysInfoWindow->requestResume();
        mSysInfoWindow->mIconAButton->requestResume();
        requestResume();
    }

    if (mSysInfoWindow->isDisappear()) {
        MR::resumeAllSceneNameObj();
        mWaitTime = 120;
        MR::activateDefaultGameLayout();
        BlueCoinUtil::setSeenBlueCoinTextBoxCurrentFile();
        setNerve(&NrvBlueCoinCounter::NrvAppearAndUpdate::sInstance);
    }
}

bool fixBlueCoinWindowCrash() {
    if (!MR::isStageFileSelect() && !MR::isEqualStageName("PeachCastleGalaxy") && !MR::isStageStoryBook())
        if (!BlueCoinUtil::hasSeenBlueCoinTextBoxCurrentFile())
            return MR::isPlayerDead() || BlueCoinUtil::isBlueCoinTextBoxAppeared();

    return MR::isPlayerDead();
}

kmCall(0x80451C40, fixBlueCoinWindowCrash);

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

	void NrvShowTextBox::execute(Spine* pSpine) const {
        ((BlueCoinCounter*)pSpine->mExecutor)->exeShowTextBox();
	}

	NrvAppear(NrvAppear::sInstance);
    NrvDisappear(NrvDisappear::sInstance);
    NrvAppearAndUpdate(NrvAppearAndUpdate::sInstance);
    NrvShowTextBox(NrvShowTextBox::sInstance);
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
        if (!BlueCoinUtil::isBlueCoinTextBoxAppeared()) {
            ((BlueCoinCounter*)pController->mPTDBlueCoinCounter)->mWaitTime = -1;
            pController->mPTDBlueCoinCounter->setNerve(&NrvBlueCoinCounter::NrvAppear::sInstance);
        }
    } 

        pController->showAllLayout();
}

kmCall(0x80466128, appearBlueCoinLayout);

void disappearBlueCoinLayout(CounterLayoutController* pController) {
    if (!MR::isStageFileSelect()) {
        if (!BlueCoinUtil::isBlueCoinTextBoxAppeared()) {
            pController->mPTDBlueCoinCounter->setNerve(&NrvBlueCoinCounter::NrvDisappear::sInstance);
        }
    }
        
    pController->hideAllLayout();
}

kmCall(0x80466198, disappearBlueCoinLayout);

void killBlueCoinCounter(CounterLayoutController* pController) {
    if (!MR::isStageFileSelect()) {
        if (!BlueCoinUtil::isBlueCoinTextBoxAppeared()) {
            MR::hideLayout(pController->mPTDBlueCoinCounter);
            pController->mPTDBlueCoinCounter->setNerve(&NrvBlueCoinCounter::NrvDisappear::sInstance);
        }
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

    MR::setTextBoxArgNumberRecursive(actor, "ShaBlueCoinTotal", BlueCoinUtil::getTotalBlueCoinNumCurrentFile(false), 0);

    if (rangeCollected > -1) {
        MR::setTextBoxArgNumberRecursive(actor, "ShaBlueCoinStage", rangeCollected, 0);
    
        MR::showPaneRecursive(actor, "ShaBlueCoinStage");

        if (rangeTotal > -1)
            BlueCoinUtil::getBlueCoinPaneNameFromTable(actor, 0);
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

    MR::setTextBoxArgNumberRecursive(pLayout, "ShaBlueCoinFileInfo", BlueCoinUtil::getTotalBlueCoinNum(fileID - 1, false), 0);
    pLayout->setNerve(pNerve);
}

kmCall(0x8046D9BC, setBlueCoinCounterFileInfo);

void initGalaxyInfoBlueCoinCount(LayoutActor* actor) {
    MR::setTextBoxFormatRecursive(actor, "ShaBlueCoinGalax", counterPictureFonts2);
}

//#ifdef NOGLE
//    kmCall(0x804A952C, initGalaxyInfoBlueCoinCount);
//#endif

void setGalaxyInfoBlueCoinCount(LayoutActor* actor, const char* pGalaxyName, const wchar_t* pWStr) {
    MR::setTextBoxMessageRecursive(actor, "StarIcon", pWStr);

    s32 rangeNum = BlueCoinUtil::getBlueCoinRangeData(pGalaxyName, true);
    MR::hidePaneRecursive(actor, "BlueCoin");

    if (rangeNum != -1) {
        MR::showPane(actor, "BlueCoin");
        MR::showPaneRecursive(actor, "ShaBlueCoinGalax");
        BlueCoinUtil::getBlueCoinPaneNameFromTable(actor, pGalaxyName);
        MR::setTextBoxFormatRecursive(actor, "ShaBlueCoinGalax", L"%ls%d", counterPictureFonts2, rangeNum);
    }
}

//#ifdef NOGLE
//kmWrite32(0x804A95E4, 0x809B003C);
//kmCall(0x804A95E8, setGalaxyInfoBlueCoinCount);
//#endif
#endif