#if defined USEBLUECOIN && !defined SM64BLUECOIN
#pragma once
#include "syati.h"
#include "pt/MapObj/BlueCoinSystem/BlueCoinUtil.h"
#include "Game/Screen/ButtonPaneController.h"
#include "Game/Screen/SysInfoWindow.h"
#include "Game/Screen/CountUpPaneRumbler.h"
#include "Game/Screen/BackButton.h"
#include "Game/Map/PauseBlur.h"

class BlueCoinBoard : public LayoutActor {
public:
    BlueCoinBoard(const char* pName);
    virtual void init(const JMapInfoIter& rIter);
    virtual void control();
    virtual void appear();
    const char* getLabelName(const char* pName, s32 num);
    void exeAppear();
    void exeSelecting();
    void exeDisappear();
    void exeSelected();
    void exeConfirmUnlock();
    void exeCountDownBlueCoin();
    void exeNotEnoughBlueCoins();
    void exeChangeButtonText();
    void exeConfirmPlayStage();

    ButtonPaneController* mButtons[8];
    TVec2f mButtonFollowPositions[8];
    TVec2f mBlueCoinCounterFollowPos;
    SysInfoWindow* mSysInfoWindowSelect;
    SysInfoWindow* mSysInfoWindowBox;
    CountUpPaneRumbler* mBlueCoinPaneRumbler;
    BackButtonCancelB* mBackButton;
    JMapInfo* mTable;
    char mBoxButtonName[8][12];
    char mButtonName[8][13];
    char mButtonTxtName[8][13];
    char mButtonFollow[8][9];
    char mCopyPosName[8][12];
    s32 mSelectedButton;
    s32 mBlueCoinNumToDisplay;
    bool mHasSpentBlueCoins;
};

namespace NrvBlueCoinBoard {
    NERVE(NrvAppear);
    NERVE(NrvSelecting);
    NERVE(NrvDisappear);
    NERVE(NrvHide);
    NERVE(NrvSelected);
    NERVE(NrvConfirmUnlock);
    NERVE(NrvCountDownBlueCoin);
    NERVE(NrvNotEnoughBlueCoins);
    NERVE(NrvChangeButtonText);
    NERVE(NrvConfirmPlayStage);
}

class BlueCoinSign : public NPCActor {
public:
    BlueCoinSign(const char* pName);
    virtual void init(const JMapInfoIter& rIter);
    bool eventFunc(u32 eventParam);
    void exeWait();
    void exeOpen();

    BlueCoinBoard* pBoard;
};

namespace NrvBlueCoinSign {
    NERVE(NrvWait);
    NERVE(NrvOpen);
};
#endif