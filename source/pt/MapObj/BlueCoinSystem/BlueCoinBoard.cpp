#if defined USEBLUECOIN && !defined SM64BLUECOIN && defined SMSS
#include "pt/MapObj/BlueCoinSystem/BlueCoinBoard.h"
#include "pt/Util.h"

/*
    Super Mario Starshine: Blue Coin Board

    This layout handles the purchasing and entering of the Blue Coin bonus stages.

    Blue Coin Flags were only ever implemented since I planned very far ahead.
    This is the only reason why they ever existed.

    Made for Starshine only. Do not ask me for this in your mods.

    First Concepted in 5/2023
    Started 9/24/2023
    Finished 10/1/23
    Revealed 10/2/23

    Made by Evanbowl
    
    I thank SPG64, Lord Giganticus, and Xandog for very helpful feedback.
*/

void* gBoardDataTable = pt::loadArcAndFile("/SystemData/BlueCoinBoardDataTable.arc", "/BlueCoinBoardDataTable.bcsv");

BlueCoinSign::BlueCoinSign(const char* pName) : NPCActor(pName) {
    pBoard = 0;
}

void BlueCoinSign::init(const JMapInfoIter& rIter) {
    NPCActorCaps caps = NPCActorCaps("BlueCoinSign");
    caps.setDefault();
    caps.mInitLightCtrl = 0;
    caps.mInitYoshiLockOnTarget = 0;
    caps.mWaitNerve = &NrvBlueCoinSign::NrvWait::sInstance;

    initialize(rIter, caps, 0, 0, 0);

    mTalkCtrl->mShowFrame = false;

    MR::registerEventFunc(mTalkCtrl, TalkMessageFunc(this, &eventFunc));

    pBoard = new BlueCoinBoard("BlueCoinBoard");
    pBoard->initWithoutIter();
    makeActorAppeared();
}

bool BlueCoinSign::eventFunc(u32 eventParam) {
    if (isNerve(&NrvBlueCoinSign::NrvOpen::sInstance)) {
        if (MR::isDead(pBoard)) {
            if (pBoard->mHasSpentBlueCoins)
                BlueCoinUtil::startCounterCountUp();

            popNerve();
            return true;
        }
        else
            return false;
    }
    else {  
        pushNerve(&NrvBlueCoinSign::NrvOpen::sInstance);
        return false;
    }

    return false;
}

void BlueCoinSign::exeWait() {
    MR::tryTalkNearPlayer(mTalkCtrl);
}

void BlueCoinSign::exeOpen() {
    if (MR::isFirstStep(this)) {
        MR::startSystemSE("SE_SY_STAR_RESULT_PANEL_OPEN", -1, -1);
        pBoard->appear();
    }
}

namespace NrvBlueCoinSign {
    void NrvWait::execute(Spine* pSpine) const {
        ((BlueCoinSign*)pSpine->mExecutor)->exeWait();
    }

    void NrvOpen::execute(Spine* pSpine) const {
        ((BlueCoinSign*)pSpine->mExecutor)->exeOpen();
    }

    NrvWait(NrvWait::sInstance);
    NrvOpen(NrvOpen::sInstance);
}


BlueCoinBoard::BlueCoinBoard(const char* pName) : LayoutActor(pName, 0) {
    for (s32 i = 0; i < 8; i++) {
        mButtons[i] = 0;
        mButtonFollowPositions[i] = TVec2f(0.0f, 0.0f);
    }

    mSelectedButton = -1;
    mSysInfoWindowSelect = 0;
    mBlueCoinPaneRumbler = 0;
    mBlueCoinNumToDisplay = 0;
    mBackButton = 0;
    mHasSpentBlueCoins = 0;
    mTable = 0;
    mTotalBlueCoinPrices = 0;
}

void BlueCoinBoard::init(const JMapInfoIter& rIter) {
    initLayoutManager("BlueCoinBoard", 2);
    MR::registerDemoSimpleCastAll(this);
    initNerve(&NrvBlueCoinBoard::NrvHide::sInstance);
    MR::connectToScene(this, 0xE, 0xD, -1, 0x48);

    mBackButton = new BackButtonCancelB("BackButtonForBlueCoin", 0);
    MR::connectToScene(mBackButton, 0xE, 0xD, -1, 0x48);
    mBackButton->initWithoutIter();

    mSysInfoWindowSelect = MR::createSysInfoWindow();
    MR::connectToScene(mSysInfoWindowSelect, 0xE, 0xD, -1, 0x48);

    mSysInfoWindowBox = MR::createSysInfoWindowMiniExecuteWithChildren();
    MR::connectToScene(mSysInfoWindowBox, 0xE, 0xD, -1, 0x48);

    mBlueCoinPaneRumbler = new CountUpPaneRumbler(this, "CounterBlueCoin");
    mBlueCoinPaneRumbler->mRumbleCalculator->mRumbleStrength = 8.0f;
    mBlueCoinPaneRumbler->reset();

    mTable = new JMapInfo();
    mTable->attach(gBoardDataTable);

    checkBoardProgress();

    MR::createAndAddPaneCtrl(this, "Misc", 1);
    MR::createAndAddPaneCtrl(this, "StarCounter", 1);
    MR::createAndAddPaneCtrl(this, "BlueCoinCounter", 1);
    
    MR::setFollowPos(&mBlueCoinCounterFollowPos, this, "BlueCoinCounter");

    for (s32 i = 0; i < 8; i++) {
        sprintf(mBoxButtonName[i], "BoxButton%d", i);
        sprintf(mButtonName[i], "CoinButton%d", i);
        sprintf(mButtonTxtName[i], "Button%dText", i);
        sprintf(mButtonFollow[i], "Button%d", i);
        sprintf(mCopyPosName[i], "Button%dPos", i);

        MR::createAndAddPaneCtrl(this, mButtonFollow[i], 1);

        mButtons[i] = new ButtonPaneController(this, mButtonName[i], mBoxButtonName[i], 0, 1);
        mButtons[i]->mFadeAfterSelect = false;

        MR::setFollowPos(&mButtonFollowPositions[i], this, mButtonFollow[i]);
        MR::setFollowTypeReplace(this, mButtonFollow[i]);

        s32 priceFromTable = 0;
        MR::getCsvDataS32(&priceFromTable, mTable, "BlueCoinPrice", i);
        mTotalBlueCoinPrices += priceFromTable;
    }
}

void BlueCoinBoard::appear() {
    setNerve(&NrvBlueCoinBoard::NrvAppear::sInstance);
    LayoutActor::appear();
}

void BlueCoinBoard::exeAppear() {
    if (MR::isFirstStep(this)) {
        s32 numStars = 0;
        const char* nameFromTable;
        s32 scenarioNoFromTable;

        for (s32 i = 0; i < 8; i++) {

            MR::getCsvDataStr(&nameFromTable, mTable, "StageName", i);
            MR::getCsvDataS32(&scenarioNoFromTable, mTable, "ScenarioNo", i);

            if (MR::makeGalaxyStatusAccessor(nameFromTable).hasPowerStar(scenarioNoFromTable))
                numStars++;

            MR::setTextBoxGameMessageRecursive(this, mButtonTxtName[i], getLabelName("BoardButton", i));

            if (BlueCoinUtil::isOnBlueCoinFlagCurrentFile(i))
                MR::setTextBoxArgStringRecursive(this, mButtonTxtName[i], MR::getGalaxyNameOnCurrentLanguage(nameFromTable), 0);
            else {
                s32 priceFromTable = 0;
                MR::getCsvDataS32(&priceFromTable, mTable, "BlueCoinPrice", i);

                MR::setTextBoxArgNumberRecursive(this, mButtonTxtName[i], i+1, 0);
                pt::setTextBoxArgStringNumberFontRecursive(this, mButtonTxtName[i], priceFromTable, 1);
            }

            mButtons[i]->appear();
        }

        mHasSpentBlueCoins = false;

        MR::requestMovementOn(mBackButton);
        MR::requestMovementOn(mSysInfoWindowSelect);
        MR::requestMovementOn(mSysInfoWindowBox);

        if (MR::isExistSceneObj(SCENE_OBJ_PAUSE_BLUR)) {
            MR::requestMovementOn(MR::getSceneObjHolder()->getObj(SCENE_OBJ_PAUSE_BLUR));
            ((PauseBlur*)MR::getSceneObjHolder()->getObj(SCENE_OBJ_PAUSE_BLUR))->_30+=1;
        }

        MR::startAnim(this, "Appear", 1);
        MR::startStarPointerModeChooseYesNo(this);

        mBlueCoinNumToDisplay = BlueCoinUtil::getTotalBlueCoinNumCurrentFile(true);

        MR::setTextBoxNumberRecursive(this, "CounterBlueCoin", mBlueCoinNumToDisplay);
        MR::setTextBoxNumberRecursive(this, "CounterStar", numStars);

        MR::setTextBoxGameMessageRecursive(this, "TextWinBase", "WinBase_NoSelection");
        MR::setTextBoxGameMessageRecursive(this, "TextTitle", "Board_Title");

        if (BlueCoinUtil::getSpentBlueCoinNumCurrentFile() == mTotalBlueCoinPrices) {
            MR::showPaneRecursive(this, "TextComplete");
            MR::hidePaneRecursive(this, "BlueCoinCounter");

            if (BlueCoinUtil::isBlueCoinBoardCompletedCurrentFile())
                MR::setTextBoxGameMessageRecursive(this, "TextComplete", "WinBase_Complete");
            else
                MR::setTextBoxGameMessageRecursive(this, "TextComplete", "WinBase_AllSpent");
        }
        
        MR::copyPaneTrans(&mBlueCoinCounterFollowPos, this, BlueCoinUtil::getTotalBlueCoinNumCurrentFile(true) > 99 ? "BlueCoinPos100" : "BlueCoinPos10");

        mBackButton->appear();

        setNerve(&NrvBlueCoinBoard::NrvSelecting::sInstance);
    }
}

void BlueCoinBoard::control() {
    for (s32 i = 0; i < 8; i++) {
        MR::copyPaneTrans(&mButtonFollowPositions[i], this, mCopyPosName[i]);
        mButtons[i]->update();
    }

    mBlueCoinPaneRumbler->update();
}

void BlueCoinBoard::exeSelecting() {
    if (MR::isFirstStep(this))
        MR::requestMovementOn(mBackButton);

    mSelectedButton = -1;
    s32 pointedButton = -1;

    for (s32 i = 0; i < 8; i++) {
        if (mButtons[i]->trySelect())
            mSelectedButton = i;

        if (mButtons[i]->isPointing())
            pointedButton = i;

        if (mButtons[i]->isPointingTrigger())
            MR::startSystemSE("SE_SY_SELECT_PAUSE_ITEM", -1, -1);
    }

    if (mSelectedButton > -1) {
        pointedButton = mSelectedButton;
        setNerve(&NrvBlueCoinBoard::NrvSelected::sInstance);
    }

    if (mBackButton->_30)
        setNerve(&NrvBlueCoinBoard::NrvDisappear::sInstance);

    if (!MR::isFirstStep(this)) {
        const char* nameFromTable;
        MR::getCsvDataStr(&nameFromTable, mTable, "StageName", pointedButton);
    
        if (pointedButton > -1) {
            MR::setTextBoxGameMessageRecursive(this, "TextWinBase", getLabelName("WinBase", pointedButton));
    
            if (BlueCoinUtil::isOnBlueCoinFlagCurrentFile(pointedButton)) {
                MR::setTextBoxArgStringRecursive(this, "TextWinBase", MR::getGalaxyNameOnCurrentLanguage(nameFromTable), 0);
            }
            else {
                s32 priceFromTable = 0;
                MR::getCsvDataS32(&priceFromTable, mTable, "BlueCoinPrice", pointedButton);
                MR::setTextBoxArgNumberRecursive(this, "TextWinBase", pointedButton+1, 0);
                pt::setTextBoxArgStringNumberFontRecursive(this, "TextWinBase", priceFromTable, 1);
            }
        }
        else
            MR::setTextBoxGameMessageRecursive(this, "TextWinBase", "WinBase_NoSelection");
    }
}

void BlueCoinBoard::exeDisappear() { 
    if (MR::isStep(this, 30)) {
        for (s32 i = 0; i < 8; i++) {
            mButtons[i]->disappear();
        }

        if (MR::isExistSceneObj(SCENE_OBJ_PAUSE_BLUR))
            ((PauseBlur*)MR::getSceneObjHolder()->getObj(SCENE_OBJ_PAUSE_BLUR))->_30-=1;
        
        MR::startAnim(this, "End", 0);
    }

    if (MR::isStep(this, 50)) {
        MR::endStarPointerMode(this);
        kill();
    }
}

void BlueCoinBoard::exeSelected() {
    if (MR::isFirstStep(this)) {
        MR::requestMovementOff(mBackButton);
        MR::startSystemSE("SE_SY_PAUSE_OFF", -1, -1);
    }

    if (MR::isStep(this, 25)) {
        mButtons[mSelectedButton]->forceToWait();

        if (BlueCoinUtil::isOnBlueCoinFlagCurrentFile(mSelectedButton))
            setNerve(&NrvBlueCoinBoard::NrvConfirmPlayStage::sInstance);
        else
            setNerve(&NrvBlueCoinBoard::NrvConfirmUnlock::sInstance);
    }
}

void BlueCoinBoard::exeConfirmUnlock() {
    s32 priceFromTable;
    MR::getCsvDataS32(&priceFromTable, mTable, "BlueCoinPrice", mSelectedButton);

    if (MR::isFirstStep(this)) {
        MR::requestMovementOn(mSysInfoWindowSelect);
        mSysInfoWindowSelect->appear("BoardInfoWindow_ConfirmUnlockStage", SysInfoWindow::SysInfoType_2, SysInfoWindow::SysInfoTextPos_0, SysInfoWindow::SysInfoMessageType_1);
        MR::setTextBoxArgNumberRecursive(mSysInfoWindowSelect, mSysInfoWindowSelect->_3C, mSelectedButton+1, 0);
        MR::setTextBoxArgNumberRecursive(mSysInfoWindowSelect, mSysInfoWindowSelect->_3C, priceFromTable, 1);
    }

    if (MR::isDead(mSysInfoWindowSelect)) {
        if (mSysInfoWindowSelect->isSelectedYes() && !BlueCoinUtil::isBlueCoinBoardCompletedCurrentFile()) {
            
            if (BlueCoinUtil::getTotalBlueCoinNumCurrentFile(true) >= priceFromTable)
                setNerve(&NrvBlueCoinBoard::NrvCountDownBlueCoin::sInstance);
            else
                setNerve(&NrvBlueCoinBoard::NrvNotEnoughBlueCoins::sInstance);
        }
        else
            setNerve(&NrvBlueCoinBoard::NrvSelecting::sInstance);
    }
}

void BlueCoinBoard::exeCountDownBlueCoin() {
    s32 priceFromTable = 0;
    MR::getCsvDataS32(&priceFromTable, mTable, "BlueCoinPrice", mSelectedButton);

    if (mBlueCoinNumToDisplay > (BlueCoinUtil::getTotalBlueCoinNumCurrentFile(true)-priceFromTable)) {
        if (getNerveStep() % 2 == 0)
            MR::startSystemSE("SE_SY_PURPLE_COIN", -1, -1);

        mBlueCoinNumToDisplay--;
    }
    else {
        MR::startPaneAnim(this, "CounterBlueCoin", "Flash", 0);
        mBlueCoinPaneRumbler->start();
        mHasSpentBlueCoins = true;
        BlueCoinUtil::spendBlueCoinCurrentFile(priceFromTable);
        BlueCoinUtil::setOnBlueCoinFlagCurrentFile(mSelectedButton);
        setNerve(&NrvBlueCoinBoard::NrvChangeButtonText::sInstance);
    }

    if (mBlueCoinNumToDisplay == 99)
        MR::copyPaneTrans(&mBlueCoinCounterFollowPos, this, "BlueCoinPos10");

    MR::setTextBoxNumberRecursive(this, "CounterBlueCoin", mBlueCoinNumToDisplay);  
}

void BlueCoinBoard::exeNotEnoughBlueCoins() {
    if (MR::isFirstStep(this))
        mSysInfoWindowBox->appear("BoardInfoWindow_NotEnoughBlueCoins", SysInfoWindow::SysInfoType_0, SysInfoWindow::SysInfoTextPos_0, SysInfoWindow::SysInfoMessageType_1);

    if (MR::isDead(mSysInfoWindowBox))
        setNerve(&NrvBlueCoinBoard::NrvSelecting::sInstance);
}

void BlueCoinBoard::exeChangeButtonText() {
    const char* nameFromTable;
    MR::getCsvDataStr(&nameFromTable, mTable, "StageName", mSelectedButton);

    if (MR::isStep(this, 20)) {
        MR::setTextBoxGameMessageRecursive(this, mButtonTxtName[mSelectedButton], "BoardButton_UnlockedUncleared");
        MR::setTextBoxArgStringRecursive(this, mButtonTxtName[mSelectedButton], MR::getGalaxyNameOnCurrentLanguage(nameFromTable), 0);

        MR::setTextBoxGameMessageRecursive(this, "TextWinBase", "WinBase_UnlockedUncleared");
        MR::setTextBoxArgStringRecursive(this, "TextWinBase", MR::getGalaxyNameOnCurrentLanguage(nameFromTable), 0);

        if (BlueCoinUtil::getSpentBlueCoinNumCurrentFile() == mTotalBlueCoinPrices) {
            MR::showPaneRecursive(this, "TextComplete");
            MR::hidePaneRecursive(this, "BlueCoinCounter");
            MR::setTextBoxGameMessageRecursive(this, "TextComplete", "WinBase_AllSpent");
        }
    }

    if (MR::isStep(this, 30))
        setNerve(&NrvBlueCoinBoard::NrvConfirmPlayStage::sInstance);
}

void BlueCoinBoard::exeConfirmPlayStage() {
    const char* nameFromTable;
    s32 scenarioNoFromTable;
    MR::getCsvDataStr(&nameFromTable, mTable, "StageName", mSelectedButton);
    MR::getCsvDataS32(&scenarioNoFromTable, mTable, "ScenarioNo", mSelectedButton);

    if (MR::isFirstStep(this)) {
        mSysInfoWindowSelect->appear("BoardInfoWindow_ConfirmPlayStage", SysInfoWindow::SysInfoType_2, SysInfoWindow::SysInfoTextPos_0, SysInfoWindow::SysInfoMessageType_1);
        MR::setTextBoxArgStringRecursive(mSysInfoWindowSelect, mSysInfoWindowSelect->_3C, MR::getGalaxyNameOnCurrentLanguage(nameFromTable), 0);
    }

    if (MR::isDead(mSysInfoWindowSelect)) {
        if (mSysInfoWindowSelect->isSelectedYes()) {
            MR::startSystemWipeCircleWithCaptureScreen(90);
            MR::stopStageBGM(60);
            GameSequenceFunction::requestChangeScenarioSelect(nameFromTable);
            GameSequenceFunction::requestChangeStage(nameFromTable, scenarioNoFromTable, scenarioNoFromTable, JMapIdInfo(0, 0));
        }
        else
            setNerve(&NrvBlueCoinBoard::NrvSelecting::sInstance);
    }
}

const char* BlueCoinBoard::getLabelName(const char* pName, s32 num) {
    const char* nameFromTable;
    s32 scenarioNoFromTable;
    MR::getCsvDataStr(&nameFromTable, mTable, "StageName", num);
    MR::getCsvDataS32(&scenarioNoFromTable, mTable, "ScenarioNo", num);

    const char* galaxyState = "Locked";

    if (BlueCoinUtil::isOnBlueCoinFlagCurrentFile(num)) {
        galaxyState = "UnlockedUncleared";

        if (MR::makeGalaxyStatusAccessor(nameFromTable).hasPowerStar(scenarioNoFromTable))
            galaxyState = "UnlockedCleared";
    }

    char* str = new char[32];
    snprintf(str, 32, "%s_%s", pName, galaxyState);

    return str;
}

void BlueCoinBoard::checkBoardProgress() {
    s32 completedStages = 0;

    if (!BlueCoinUtil::isBlueCoinBoardCompletedCurrentFile()) {
        for (s32 i = 0; i < 8; i++) {
            if (BlueCoinUtil::isOnBlueCoinFlagCurrentFile(i)) {
                const char* nameFromTable;
                s32 scenarioNoFromTable;
                MR::getCsvDataStr(&nameFromTable, mTable, "StageName", i);
                MR::getCsvDataS32(&scenarioNoFromTable, mTable, "ScenarioNo", i);

                if (MR::makeGalaxyStatusAccessor(nameFromTable).hasPowerStar(scenarioNoFromTable))
                    completedStages++;
            }
        }

        if (completedStages == 8)
            BlueCoinUtil::setBlueCoinBoardCompletedCurrentFile();
    }

}

namespace NrvBlueCoinBoard {
    void NrvAppear::execute(Spine* pSpine) const {
        ((BlueCoinBoard*)pSpine->mExecutor)->exeAppear();
    }

    void NrvSelecting::execute(Spine* pSpine) const {
        ((BlueCoinBoard*)pSpine->mExecutor)->exeSelecting();
    }

    void NrvDisappear::execute(Spine* pSpine) const {
        ((BlueCoinBoard*)pSpine->mExecutor)->exeDisappear();
    }

    void NrvHide::execute(Spine* pSpine) const {}

    void NrvSelected::execute(Spine* pSpine) const {
        ((BlueCoinBoard*)pSpine->mExecutor)->exeSelected();
    }

    void NrvConfirmUnlock::execute(Spine* pSpine) const {
        ((BlueCoinBoard*)pSpine->mExecutor)->exeConfirmUnlock();
    }

    void NrvCountDownBlueCoin::execute(Spine* pSpine) const {
        ((BlueCoinBoard*)pSpine->mExecutor)->exeCountDownBlueCoin();
    }

    void NrvNotEnoughBlueCoins::execute(Spine* pSpine) const {
        ((BlueCoinBoard*)pSpine->mExecutor)->exeNotEnoughBlueCoins();
    }

    void NrvChangeButtonText::execute(Spine* pSpine) const {
        ((BlueCoinBoard*)pSpine->mExecutor)->exeChangeButtonText();
    }

    void NrvConfirmPlayStage::execute(Spine* pSpine) const {
        ((BlueCoinBoard*)pSpine->mExecutor)->exeConfirmPlayStage();
    }

    NrvAppear(NrvAppear::sInstance);
    NrvSelecting(NrvSelecting::sInstance);
    NrvDisappear(NrvDisappear::sInstance);
    NrvHide(NrvHide::sInstance);
    NrvSelected(NrvSelected::sInstance);
    NrvConfirmUnlock(NrvConfirmUnlock::sInstance);
    NrvCountDownBlueCoin(NrvCountDownBlueCoin::sInstance);
    NrvNotEnoughBlueCoins(NrvNotEnoughBlueCoins::sInstance);
    NrvChangeButtonText(NrvChangeButtonText::sInstance);
    NrvConfirmPlayStage(NrvConfirmPlayStage::sInstance);
}
#endif