//#if defined (ALL) || defined (SMSS)
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

kmCall(0x80486D58, InitNewButton); // Call

void ButtonControl(TVec2f* pPos, PauseMenu* pPauseMenu, const char* pStr) {
    MR::copyPaneTrans(&pPauseMenu->mButtonTopFollowPos, pPauseMenu, pStr);
    MR::copyPaneTrans(&pPauseMenu->mButtonNewFollowPos, pPauseMenu, "NewButtonPos");

    pPauseMenu->mButtonNew->update();
}

kmCall(0x8048727C, ButtonControl); // Call

void ForceToWaitNewButton(PauseMenu* pPauseMenu) {
    pPauseMenu->mButtonTop->forceToWait();
    pPauseMenu->mButtonNew->forceToWait();
}

kmWrite32(0x80487504, 0x60000000); // nop (Skip overwriting r3)
kmCall(0x80487508, ForceToWaitNewButton); // Call

void AppearNewButton(PauseMenu* pPauseMenu) {
    pPauseMenu->mButtonTop->appear();
    pPauseMenu->mButtonNew->appear();
}

kmWrite32(0x80487560, 0x7FE3FB78); // mr r3, r31 (PauseMenu* into r3)
kmCall(0x80487564, AppearNewButton); // Call

bool IsNewButtonPointingTrigger(PauseMenu* pPauseMenu) {
    return pPauseMenu->mButtonTop->isPointingTrigger() || pPauseMenu->mButtonNew->isPointingTrigger();
}

kmWrite32(0x80487714, 0x7F63DB78); // mr r3, r27 (PauseMenu* into r3)
kmCall(0x80487720, IsNewButtonPointingTrigger);

bool IsNewButtonPressed(PauseMenu* pPauseMenu) {
    bool isPressed = false;

    if (pPauseMenu->mButtonBottom) {
        if (pPauseMenu->mButtonBottom->trySelect()) {
            pPauseMenu->mIsUsedNewButton = false;
            isPressed = true;
        }
    }

    if (pPauseMenu->mButtonNew->trySelect()) {
        pPauseMenu->mIsUsedNewButton = true;
        isPressed = true;
    }

    return isPressed;
}

kmWrite32(0x804877B4, 0x7F63DB78); // mr r3, r27 (PauseMenu* into r3)
kmCall(0x804877C0, IsNewButtonPressed); // Call

const char* SysInfoWindowStr(bool isValid, PauseMenu* pPauseMenu) {
    return pPauseMenu->mIsUsedNewButton ? "PauseMenu_ConfirmRestartStage" : isValid ? "PauseMenu_ConfirmEndGame" : "PauseMenu_ConfirmBackWorldMap";
}

kmWrite32(0x80487C34, 0x7FE4FB78); // mr r4, r31 (PauseMenu* into r4)
kmCall(0x80487C38, SysInfoWindowStr); // Call
kmWrite32(0x80487C3C, 0x7C641B78); // mr r4, r3 (Store returned const char* into r4)
kmWrite32(0x80487C40, 0x4800000C); // b 0xC (Skip useless instructions)

void DisappearNewButton(PauseMenu* pPauseMenu) {
    pPauseMenu->mButtonTop->disappear();
    pPauseMenu->mButtonNew->disappear();
}

kmWrite32(0x80487B10, 0x7FC3F378); // mr r3, r30 (PauseMenu* into r3)
kmCall(0x80487B14, DisappearNewButton); // Call

bool IsNewButtonTimingForSelectedSE(PauseMenu* pPauseMenu) {
    if (pPauseMenu->mButtonBottom) {
        if (pPauseMenu->mButtonBottom->isTimingForSelectedSe())
            return true;
    }

    if (pPauseMenu->mButtonNew) {
        if (pPauseMenu->mButtonNew->isTimingForSelectedSe())
            return true;
    }

    return false;
}


kmWrite32(0x804879C8, 0x7FC3F378); // mr r3, r30
kmCall(0x804879CC, IsNewButtonTimingForSelectedSE); // Call

bool IsNewButtonDecidedWait(PauseMenu* pPauseMenu) {
    if (pPauseMenu->mButtonBottom) {
        if (pPauseMenu->mButtonBottom->isDecidedWait())
            return true;
    }

    if (pPauseMenu->mButtonNew) {
        if (pPauseMenu->mButtonNew->isDecidedWait())
            return true;
    }

    return false;
}

kmWrite32(0x80487A00, 0x7FC3F378); // mr r3, r30 (PauseMenu* into r3)
kmCall(0x80487A04, IsNewButtonDecidedWait); // Call

void DoNewButtonAction(PauseMenu* pPauseMenu, bool isValid) {
    MR::startSystemWipeCircleWithCaptureScreen(0x5A);
    
    if (pPauseMenu->mIsUsedNewButton) {
        const char* currStage = MR::getCurrentStageName();
        GameSequenceFunction::requestChangeScenarioSelect(currStage);
		GameSequenceFunction::requestChangeStage(currStage, MR::getCurrentScenarioNo(), MR::getCurrentSelectedScenarioNo(), *(JMapIdInfo*)0);
    }
    else {
        if (isValid)
            GameSequenceFunction::notifyToGameSequenceProgressToEndScene();
        else
            GameSequenceFunction::requestChangeStageWorldMap();
    }
}

kmWrite32(0x80487CA4, 0x7C641B78); // mr r4, r3 (isValid into r4)
kmWrite32(0x80487CA8, 0x7FE3FB78); // mr r3, r31 (PauseMenu* into r3)
kmCall(0x80487CAC, DoNewButtonAction); // Call
kmWrite32(0x80487CB0, 0x48000008); // b 0x8 (Skip useless instructions)

bool ButtonBottomCrashFix(PauseMenu* pPauseMenu) {
    if (pPauseMenu->mButtonNew) {
        if (pPauseMenu->mButtonNew->_24) {
            return true;
        }
    }

    if (!pPauseMenu->mButtonBottom)
        return false;
    
    if (!pPauseMenu->mButtonBottom->_24)
        return false;

    return true;
}

kmWrite32(0x804879B0, 0x7FC3F378);
kmCall(0x804879B4, ButtonBottomCrashFix);
kmWrite32(0x804879B8, 0x2C030000);
kmWrite32(0x804879BC, 0x41820080);
kmWrite32(0x804879C0, 0x48000008);


bool ButtonStarListCrashFix(PauseMenu*pPauseMenu) {
    if (!pPauseMenu->mButtonStarList)
        return false;
    
    if (!pPauseMenu->mButtonStarList->_24)
        return false;

    return true;
}

kmWrite32(0x80487A3C, 0x7FC3F378);
kmCall(0x80487A40, ButtonStarListCrashFix);
kmWrite32(0x80487A44, 0x2C030000);
kmWrite32(0x80487A48, 0x41820064);
kmWrite32(0x80487A4C, 0x60000000);
kmWrite32(0x80487A50, 0x807E0034);
#endif