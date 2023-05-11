#ifdef WIP
#pragma once

#include "syati.h"
#include "Game/Screen/PauseMenu.h"

kmWrite32(0x804712C0, 0x3860006C); // li r3, 0x6C

// init hook

void InitNewButton(PauseMenu* pPauseMenu, const char* pStr) {
    MR::createAndAddPaneCtrl(pPauseMenu, pStr, 1);
    MR::createAndAddPaneCtrl(pPauseMenu, "NewButton", 1);
    MR::setTextBoxGameMessageRecursive(pPauseMenu, "Text4", "PauseMenu_ButtonRestartStage");
    pPauseMenu->mIsUsedNewButton = false;
    pPauseMenu->mButtonNewFollowPos = TVec2f(0.0f, 0.0f);

    pPauseMenu->mButtonNew = new ButtonPaneController(pPauseMenu, "NBackNew", "BoxButton4", 0, 1);
    pPauseMenu->mButtonNew->_26 = false;
    MR::setFollowPos(&pPauseMenu->mButtonNewFollowPos, pPauseMenu, "NewButton");
    MR::setFollowTypeReplace(pPauseMenu, "NewButton");
}

kmCall(0x80486D58, InitNewButton);

void ButtonControl(TVec2f* pPos, PauseMenu* pPauseMenu, const char* pStr) {
    MR::copyPaneTrans(&pPauseMenu->mButtonTopFollowPos, pPauseMenu, pStr);
    MR::copyPaneTrans(&pPauseMenu->mButtonNewFollowPos, pPauseMenu, "NewButtonPos");

    pPauseMenu->mButtonNew->update();
}

kmCall(0x8048727C, ButtonControl);

void ForceToWaitNewButton(ButtonPaneController* pButtonTop) {
    PauseMenu* pPauseMenu = (PauseMenu*)pButtonTop->mHost;

    pButtonTop->forceToWait();
    pPauseMenu->mButtonNew->forceToWait();
}

kmCall(0x80487508, ForceToWaitNewButton);

void AppearNewButton(ButtonPaneController* pButtonTop) {
    PauseMenu* pPauseMenu = (PauseMenu*)pButtonTop->mHost;
    pButtonTop->appear();
    pPauseMenu->mButtonNew->appear();
}

kmCall(0x80487564, AppearNewButton);

bool ButtonTest(ButtonPaneController* pButtonTop) {
    PauseMenu* pPauseMenu = (PauseMenu*)pButtonTop->mHost;

    if (pButtonTop->isPointingTrigger())
        return true;

    if (pPauseMenu->mButtonNew->isPointingTrigger())
        return true;

    return false;
}

kmCall(0x80487720, ButtonTest);

bool IsNewButtonPressed(PauseMenu* pPauseMenu) {
    bool isPressed = false;

    if (pPauseMenu->mButtonBottom->trySelect()) {
        pPauseMenu->mIsUsedNewButton = false;
        isPressed = true;
    }

    if (pPauseMenu->mButtonNew->trySelect()) {
        pPauseMenu->mIsUsedNewButton = true;
        isPressed = true;
    }

    return isPressed;
}

kmWrite32(0x804877B4, 0x7F63DB78);
kmCall(0x804877C0, IsNewButtonPressed);

const char* SysInfoWindowStr(bool isValid, PauseMenu* pPauseMenu) {
    const char* str;

    OSReport("IsValid: %d\n", isValid);

    if (pPauseMenu->mIsUsedNewButton)
        str = "PauseMenu_ConfirmRestartStage";
    else
        str = isValid ? "PauseMenu_ConfirmEndGame" : "PauseMenu_ConfirmBackWorldMap";

    return str;
}

kmWrite32(0x80487C34, 0x7FE4FB78); // mr r4, r31
kmCall(0x80487C38, SysInfoWindowStr); // Call
kmWrite32(0x80487C3C, 0x7C641B78); // mr r4, r3
kmWrite32(0x80487C40, 0x4800000C); // b 0xC

void DisappearNewButton(ButtonPaneController* pButtonTop) {
    PauseMenu* pPauseMenu = (PauseMenu*)pButtonTop->mHost;
    pButtonTop->disappear();
    pPauseMenu->mButtonNew->disappear();
}

kmCall(0x80487B14, DisappearNewButton);

bool IsNewButtonTimingForSelectedSE(ButtonPaneController* pButtonBottom) {
    PauseMenu* pPauseMenu = (PauseMenu*)pButtonBottom->mHost;

    if (pPauseMenu->mButtonBottom->isTimingForSelectedSe() || pPauseMenu->mButtonNew->isTimingForSelectedSe())
        return true;


    return false;
}

//kmWrite32(0x804879C8, 0x7FC3F378);
kmCall(0x804879CC, IsNewButtonTimingForSelectedSE);

//kmWrite32(0x804879C8, 0x807E0030); // lwz r3, 0x30(r30)

//kmWrite32(0x80487A3C, 0x807E0034); // lwz r3, 0x34(r30)


bool IsNewButtonDecidedWait(ButtonPaneController* pButtonBottom) {
    PauseMenu* pPauseMenu = (PauseMenu*)pButtonBottom->mHost;

    if (pButtonBottom->isDecidedWait() || pPauseMenu->mButtonNew->isDecidedWait())
        return true;

    return false;
}
kmCall(0x80487A04, IsNewButtonDecidedWait);

bool CheckButton(PauseMenu* pPauseMenu) {
    //PauseMenu* pPauseMenu = (PauseMenu*)pButtonBottom->mHost;
    if (pPauseMenu->mButtonBottom || pPauseMenu->mButtonNew && pPauseMenu->mButtonBottom->_24 || pPauseMenu->mButtonNew->_24)
        return true;

    return false;
}

kmWrite32(0x804879B0, 0x7FC3F378); // mr r3, r30
kmCall(0x804879B4, CheckButton);
kmWrite32(0x804879B8, 0x2C030000); // cmpwi r3, 0
kmWrite32(0x804879BC, 0x41820080); // beq 0x80
kmWrite32(0x804879C0, 0x48000008); // b 0x8

void DoNewButtonAction(PauseMenu* pPauseMenu, bool isValid) {
    MR::startSystemWipeCircleWithCaptureScreen(0x5A);
    OSReport("Button %d, %s\n", pPauseMenu->mIsUsedNewButton, pPauseMenu->mName);

    if (pPauseMenu->mIsUsedNewButton) { // Broken, somehow. 111?
		GameSequenceFunction::requestChangeScenarioSelect(MR::getCurrentStageName());
		GameSequenceFunction::requestChangeStage(MR::getCurrentStageName(), MR::getCurrentScenarioNo(), MR::getCurrentSelectedScenarioNo(), *(JMapIdInfo*)0);
    }
    else {
        if (isValid)
            GameSequenceFunction::notifyToGameSequenceProgressToEndScene();
        else
            GameSequenceFunction::requestChangeStageWorldMap();
    }
}

kmWrite32(0x80487C9C, 0x60000000); // nop
kmWrite32(0x80487CA4, 0x7C641B78); // mr r4, r3
kmWrite32(0x80487CA8, 0x7FE3FB78); // mr r3, r31
kmCall(0x80487CAC, DoNewButtonAction); // call to func
kmWrite32(0x80487CB0, 0x48000008); // b 0x8
#endif