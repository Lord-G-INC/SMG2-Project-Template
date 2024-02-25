#if defined USEBLUECOIN && !defined SM64BLUECOIN
#include "pt/MapObj/BlueCoinSystem/BlueCoinUtil.h"
#include "pt/MapObj/BlueCoinSystem/BlueCoinLayouts.h"
#include "Game/Screen/GameSceneLayoutHolder.h"
#include "Game/Screen/PauseMenu.h"


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

void initBlueCoinStageCounters(PauseMenu* pPauseMenu) {
    MR::connectToSceneLayoutOnPause(pPauseMenu);

    MR::setTextBoxFormatRecursive(pPauseMenu, "ShaBlueCoinTotal", counterPictureFonts);
    MR::setTextBoxFormatRecursive(pPauseMenu, "ShaBlueCoinStage", counterPictureFonts);
}

kmCall(0x80486D60, initBlueCoinStageCounters);

void setPauseMenuBlueCoinStageCount(LayoutActor* pPauseMenu) {
    s32 rangeCollected = BlueCoinUtil::getBlueCoinRangeData(0, true);
    s32 rangeTotal = BlueCoinUtil::getBlueCoinRangeData(0, false);

    MR::setTextBoxArgNumberRecursive(pPauseMenu, "ShaBlueCoinTotal", BlueCoinUtil::getTotalBlueCoinNumCurrentFile(false), 0);

    if (rangeCollected > -1) {
        MR::setTextBoxArgNumberRecursive(pPauseMenu, "ShaBlueCoinStage", rangeCollected, 0);
    
        MR::showPaneRecursive(pPauseMenu, "ShaBlueCoinStage");

        if (rangeTotal > -1)
            BlueCoinUtil::getBlueCoinPaneNameFromTable(pPauseMenu, 0);
    }
    else
        MR::hidePaneRecursive(pPauseMenu, "ShaBlueCoinStage");
}

s32 setUpBlueCoinInfo(PauseMenu* pPauseMenu) {
    setPauseMenuBlueCoinStageCount(pPauseMenu);

    pPauseMenu->mDisplayMode = 0;

    s32 rangemin = BlueCoinUtil::getBlueCoinRange(0, 0);

    if (rangemin != -1) {
        s32 totalCoins = (BlueCoinUtil::getBlueCoinRange(0, 1)-rangemin)+1;
        s32 newLineOff = 0;
        s32 collectedCount = 0;
        bool newLineAdded = 0;
        wchar_t* pWStr = new wchar_t[32];
        wchar_t* pCompleteIcon = new wchar_t[2];
        wchar_t* pStarIcon = new wchar_t[2];
        wchar_t* pBButtonIcon = new wchar_t[2];

        if (totalCoins > 30)
            totalCoins = 30;

        MR::addPictureFontCode(&pStarIcon[0], 0x97);
        MR::addPictureFontCode(&pBButtonIcon[0], 0x31);

        MR::setTextBoxFormatRecursive(pPauseMenu, "TxtCoinPage", pStarIcon);
        MR::setTextBoxFormatRecursive(pPauseMenu, "TxtCoinBButton", pBButtonIcon);

        MR::hidePaneRecursive(pPauseMenu, "ShaCoinListWin");
        MR::hidePaneRecursive(pPauseMenu, "TxtCoinComplete");

        MR::showPaneRecursive(pPauseMenu, "TxtCoinPage");
        MR::showPaneRecursive(pPauseMenu, "TxtCoinBButton");

        if (!MR::isStageNoPauseMenuStars()) {
            MR::showPaneRecursive(pPauseMenu, "Stars");
            MR::showPaneRecursive(pPauseMenu, "ScenarioTitle");
        }

        for (s32 i = 0; i < totalCoins + (totalCoins > 15) ? 1 : 0; i++) {
            newLineAdded = 0;

            if (i == totalCoins/2 && totalCoins > 15) {
                MR::addNewLine(&pWStr[i]);
                newLineOff++;
                newLineAdded = true;
            }
            
            if (!newLineAdded) {
                bool isGot = BlueCoinUtil::isBlueCoinGotCurrentFile((i+rangemin)-newLineOff);
                int icon = 0x96;
            
                if (isGot) {
                    icon = 0x8A;
                    collectedCount++;
                }

                MR::addPictureFontCode(&pWStr[i], icon);
            }
        }

        MR::addPictureFontCode(&pCompleteIcon[0], collectedCount == totalCoins ? 0x50 : 0x52);

        MR::setTextBoxFormatRecursive(pPauseMenu, "TxtCoinComplete", pCompleteIcon);
        MR::setTextBoxFormatRecursive(pPauseMenu, "ShaCoinListWin", pWStr);
        }
    else
        pPauseMenu->mDisplayMode = 2;

    return MR::getCoinNum();
} // TxtCoinComplete

kmCall(0x80487090, setUpBlueCoinInfo);

// FILE INFO

void initBlueCoinCounterFileInfo(LayoutActor* pLayout) {
    MR::connectToSceneLayout(pLayout);
    MR::showPaneRecursive(pLayout, "BlueCoinFileInfo");
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