#ifdef SMG63
#include "Game/Util/BgmUtil.h"
#include "Game/Util/SoundUtil.h"
#include "kamek/hooks.h"
bool gIsFileSelectUsable = false;

// This is experimental, so it is currently only used for SMG63.
void playFileSelectMusic () {
    MR::stopSystemSE("SE_SY_LV_TITLE_BGM", 0);
    /*MR::stopSystemSE("SE_SY_LV_TITLE_BGM_RHYTHM", 0);
    MR::stopSystemSE("BGM_TITLE", 0);
    MR::stopSystemSE("MBGM_TITLE", 0);*/
    MR::startSubBGM("STM_TITLE", false);
    gIsFileSelectUsable = true;
}
kmBranch(0x8024CB90, playFileSelectMusic);

void controlFileSelectBGM (const char *seName, s32, s32) {
    if (!gIsFileSelectUsable) 
        MR::startSystemLevelSE(seName, -1, -1);
}

kmCall(0x80251400, controlFileSelectBGM);
#endif