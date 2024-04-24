#ifdef SMSS
#include "syati.h"	
bool gIsFileSelectUsable;

extern "C" {
	void onSwitchB__2MRFP9LiveActor();
	void stopStageBGM__2MRFUl();
}
asm void megahammerActivateSwitch() {
	stwu r1, 0x10(r1)
	mflr r0
	stw r0, 0x24(r1)
	stw r30, 0x1C(r1)
	mr r30, r3
	bl onSwitchB__2MRFP9LiveActor
	li r3, 0
	bl stopStageBGM__2MRFUl
	lwz r0, 0x24(r1)
	lwz r30, 0x1C(r1)
	mtlr r0
	addi r1, r1, 0x10
	blr
}
kmWrite32(0x800DAABC, 0x60000000);
kmCall(0x800DFCB0, megahammerActivateSwitch);
kmWrite32(0x800DFCE8, 0x38600001);

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