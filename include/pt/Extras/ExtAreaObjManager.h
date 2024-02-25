#pragma once
#include "syati.h"
#include "Game/AreaObj/AreaObjContainer.h"

static ManagerEntry cNewMgrTable[] = {
    {"WarpArea", 100, AreaObjContainer::createManager<AreaObjMgr> },
    {"JumpSwitchArea", 16, AreaObjContainer::createManager<AreaObjMgr> },
    {"ChildSwitchArea", 32, AreaObjContainer::createManager<AreaObjMgr> },
    {"SMG63Area", 16, AreaObjContainer::createManager<AreaObjMgr> },
    {"NumberedSwitchArea", 16, AreaObjContainer::createManager<AreaObjMgr> }
};

const s32 cNumNewMgrs = sizeof(cNewMgrTable) / sizeof(ManagerEntry);
