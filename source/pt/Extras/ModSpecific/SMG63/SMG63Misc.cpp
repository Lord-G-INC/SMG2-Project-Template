#ifdef SMG63
#include "Game/Util/BgmUtil.h"
#include "Game/Util/SoundUtil.h"
#include "kamek/hooks.h"
bool gIsFileSelectUsable;

// This is experimental, so it is currently only used for SMG63.
void playFileSelectMusic () {
    MR::stopSystemSE("SE_SY_LV_TITLE_BGM", 18);
    MR::startSubBGM("STM_TITLE", false);
    gIsFileSelectUsable = true;
}
kmBranch(0x8024CB90, playFileSelectMusic);

void controlFileSelectBGM (const char *, s32, s32) {
    if (!gIsFileSelectUsable) 
        MR::startSystemLevelSE("SE_SY_LV_TITLE_BGM", -1, -1);
}
kmCall(0x80251400, controlFileSelectBGM);

void onInitFileSelect () {
    gIsFileSelectUsable = false;
}
kmBranch(0x8024F480, onInitFileSelect);

#endif