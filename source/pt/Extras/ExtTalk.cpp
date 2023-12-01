#include "syati.h"
#include "pt/MapObj/BlueCoinSystem/BlueCoinUtil.h"

namespace pt {
    void customLMSBranchConditions(TalkNodeCtrl* pCtrl, bool result) {
        u16 condType = ((u16*)pCtrl->getCurrentNodeBranch())[3];
        u16 condParam = ((u16*)pCtrl->getCurrentNodeBranch())[4];

        switch (condType) {
            case 30:
                result = BlueCoinUtil::isBlueCoinGotCurrentFile(condParam);
            break;
            case 31:
                result = BlueCoinUtil::getTotalBlueCoinNumCurrentFile(false) >= condParam;
            break;
            case 32:
                result = BlueCoinUtil::getTotalBlueCoinNumCurrentFile(true) >= condParam;
            break;
            case 33: 
                result = BlueCoinUtil::isOnBlueCoinFlagCurrentFile(condParam);
            break;
            case 34:
                result = BlueCoinUtil::isBlueCoinBoardCompletedCurrentFile();
            break;
        }

        pCtrl->forwardCurrentBranchNode(result);
    }

    kmWrite32(0x8037B134, 0x7FC3F378); // mr r3, r30
    kmWrite32(0x8037B138, 0x7CC43378); // mr r4, r6
    kmCall(0x8037B13C, customLMSBranchConditions);
    kmWrite32(0x8037B140, 0x4BFFFE9C); // b -0x164

    bool customLMSEventFunctions(TalkNodeCtrl* pNode) {
        u16 eventType = ((u16*)pNode->getCurrentNodeEvent())[2];
        u16 eventParam = ((u16*)pNode->getCurrentNodeEvent())[5];

        switch (eventType) {
            case 30:
                BlueCoinUtil::spendBlueCoinCurrentFile(eventParam);
            break;
            case 31:
                BlueCoinUtil::startCounterCountUp();
            break;
            case 32:
                BlueCoinUtil::setOnBlueCoinFlagCurrentFile(eventParam);
            break;
        }

        return pNode->isExistNextNode();
    }

    kmCall(0x8037B38C, customLMSEventFunctions);
}