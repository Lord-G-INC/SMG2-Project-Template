#include "syati.h"
#include "Game/Util.h"
#include "Game/Screen/LayoutActor.h"
#include "Game/System/Misc/TalkMessageCtrl.h"
#include "pt/Util/ActorUtil.h"
#include "JSystem/JUtility/JUTTexture.h"
#include "Game/MapObj/SpinDriverPathDrawer.h"
#include "pt/Extras/JUTHolder.h"

/*
* Authors: Aurum
*/
namespace pt {
	/*
	* Error Message Fallback
	*
	* Missing text will usualy crash the game. This small patch will display the text label whose message data could not be
	* located instead of nothing. This tells developers what text is missing.
	*/
	static wchar_t cErrorMessageBuffer[128]; // Buffer for missing text label. Last wchar should always be NULL.

	const wchar_t* getErrorMessage(const char *pLabel) {
		asm("mr %0, r31" : "=r" (pLabel));

		size_t len = strlen(pLabel) + 1;

		if (len > 127) {
			len = 127;
		}

		mbstowcs(cErrorMessageBuffer, pLabel, len);

		return cErrorMessageBuffer;
	}

	kmCall(0x800413F0, getErrorMessage); // MR::getGameMessageDirect will return the error message instead of NULL

	/*
	* Green + Custom Launch Stars
	*
	* Unsurprisingly, all the BTP and BRK frames for the green Launch Star is still found inside SuperSpinDriver.arc. Here,
	* we hijack calls to initColor in order to check for the green color first. If the Launch Star's color is set to green,
	* we apply its animation frames. Otherwise, we call initColor to set up the other colors.
	*
	* This code also allows new BTP frames and SpinDriverPath textures to be loaded and used.
	* Support for new BRK frames may be added in the future.
	*/

	const char* ColorsStr[] = {"Red.bti", "Blue.bti", "Rainbow.bti", "Purple.bti"};
	const int size = arrsize(ColorsStr);
	JUTHolder<size> Colors = JUTHolder<size>();

	void initSuperSpinDriverCustomColor(SuperSpinDriver *pActor) {
		s32 color = pActor->mColor;

		if (color != 0 && color != 2) {
			MR::startBtpAndSetFrameAndStop(pActor, "SuperSpinDriver", color);
			MR::startBrk(pActor, color == 1 ? "Green" : "Red");

			pActor->mSpinDriverPathDrawer->mColor = color == 1 ? 0 : color;
		} else
			pActor->initColor();
			
        if (color >= 3)
            Colors.SetTexture(color - 3, new JUTTexture(MR::loadTexFromArc("SpinDriverPath.arc", ColorsStr[color - 3], 0), 0));
	}

	kmCall(0x8031E29C, initSuperSpinDriverCustomColor); // redirect initColor in init

	void setSpinDriverPathCustomColor(SpinDriverPathDrawer* pDrawer) {
		if (pDrawer->mColor >= 3)
			Colors.getTexture(pDrawer->mColor - 3)->load(GX_TEXMAP0);

		pDrawer->calcDrawCode(); // Restore original call
	}

	kmCall(0x8030EF28, setSpinDriverPathCustomColor);


	/*
	* The Green Launch Star is coded to load a model from SuperSpinDriverEmpty.arc. This was used for the transparent model
	* in SMG1 to mark its position before all green stars are collected. However, we have no use of this property in SMG2,
	* so we can safely disable this here. This also improves memory usage since this model would be loaded at all times.
	*/
	kmWrite32(0x8031E2A4, 0x60000000); // NOP call to initEmptyModel.

	/*
	* Custom Flag Textures
	*
	* Requires a BTI added to the target Flag.arc ending with a number.
	* This number is what Obj_arg0 will select.
	* 
	* Setting Obj_arg0 to 4 would mean that...
	* (FlagName(4)).bti would be selected in (FlagName).arc
	*/

	ResTIMG* CustomFlagTextures(LiveActor* pActor, const char* pStr, const JMapInfoIter& rIter) {
			s32 flagBti = 0;
			char* outArcStr = new char[24];
			char* outBtiStr = new char[24];
			MR::getJMapInfoArg0NoInit(rIter, &flagBti);
			
			snprintf(outArcStr, 24, "%s.arc", pStr);

			if (flagBti < 1)
				snprintf(outBtiStr, 24, "%s.bti", pStr);
			else
				snprintf(outBtiStr, 24, "%s%d.bti", pStr, flagBti);

			OSReport("%s, %s\n", outArcStr, outBtiStr);
			return MR::loadTexFromArc(pActor, outArcStr, outBtiStr);
	}

	kmWrite32(0x80254880, 0x60000000); // nop
	kmWrite32(0x80254884, 0x7FA5EB78); // mr r5, r29
	kmCall(0x8025488C, CustomFlagTextures);

	///*
	//* Shell-casting Magikoopa
	//*/
	//void initNewKameck(Kameck *pActor, const JMapInfoIter &rIter) {
	//	pActor->initJMapParam(rIter);
	//
	//	if (MR::isValidInfo(rIter)) {
	//		if (MR::isObjectName(rIter, "KameckJetTurtle")) {
	//			pActor->mBeamType = KAMECK_BEAM_TURTLE;
	//		}
	//	}
	//}
	//
	//kmCall(0x801A49D4, initNewKameck);
	//
	///*
	//* While carrying over stuff from the first game, they forgot to update parts of the KameckTurtle actor. Therefore,
	//* it will crash the game and cause various other problems. First of all, it tries to load the animation from SMG1,
	//* which does not exist anymore (Koura.brk was renamed to Color.brk). Also, Mario won't try to pull in the shell
	//* due to the shell's actor name being wrong. For some reason it expects a specific actor name...
	//* Lastly, the actor should be dead by default, but they made it appear nevertheless.
	//*/
	//void initFixKameckTurtle(LiveActor *pActor) {
	//	pActor->mName = "�J���b�N�r�[���p�J��";
	//	MR::startBrk(pActor, "Color");
	//}
	//
	//kmCall(0x801A8CFC, initFixKameckTurtle); // redirect BRK assignment to initialization fix
	//kmWrite32(0x801A8DD0, 0x818C0038);       // Call makeActorDead instead of makeActorAppeared


	/*
	* KeySwitch fix
	*
	* If a KeySwitch is constructed by Teresa or KuriboChief, it will crash the game due to an oversight by the developers.
	* This feature works perfectly fine in SMG1., however, SMG2 introduced a new Obj_arg that does not check if the given
	* JMapInfoIter is valid before attempting to read the Obj_arg value. This will cause the game to access invalid data
	* and thus causing a crash.
	*/
	void initKeySwitchSafeGetShadowDropLength(const JMapInfoIter &rIter, f32 *pDest) {
		if (MR::isValidInfo(rIter)) {
			MR::getJMapInfoArg0NoInit(rIter, pDest);
		}
	}

	kmCall(0x802BDE80, initKeySwitchSafeGetShadowDropLength); // overwrite call to getJMapInfoArg0NoInit


	/*
	* QuakeEffectGenerator fix
	*
	* QuakeEffectGenerator never plays the earthquake sound as its sound object is never initialized. This is likely an
	* oversight from when the SMG2 developers upgraded SMG1's sound system since sounds are loaded slightly different in
	* SMG1.
	*/
	void initQuakeEffectGeneratorSound(LiveActor *pActor) {
		MR::invalidateClipping(pActor);
		pActor->initSound(1, "QuakeEffectGenerator", &pActor->mTranslation, TVec3f(0.0f, 0.0f, 0.0f));
	}

	kmCall(0x8026360C, initQuakeEffectGeneratorSound); // redirection hook

	/*
	* Debugging feature: displaying the file name on the "File isn't exist" error.
	*
	* When the game attempts to load a file into memory, it runs MR::isFileExist to check for the file, and if the file it's checking
	* for doesn't exist, it calls OSFatal,  crashing the game. It also prints "File isn't exist" to the log.
	*
	* Here, the MR::isFileExist call is replaced with a call to this new function, that prints the file name with the error, if the checked file is missing.
	*
	* This is useful for debugging certain things!
	*/

	void printFileNameIfMissing(const char* fileName) {
		if (!MR::isFileExist(fileName, 0))
			OSPanic("FileRipper.cpp", 118, "File \"%s\" isn't exist.", fileName);
	}

	kmCall(0x804B1FE0, printFileNameIfMissing);

	/*
	* Mini Patch: Swimming Death Area
	* 
	* This patch is really useless but I thought it would be nice to include.
	* For example, this could be used to make instant death water/lava.
	*/

	void DeathAreaExtensions(DeathArea* area) {
		MR::getGameSceneLayoutHolder()->mMarioSubMeter->mAirMeter->mLayoutActorFlag.mIsHidden = area->isInVolume(*MR::getPlayerPos());

		bool checkForSwimming;

		if (area->mObjArg1 == -1)
			checkForSwimming = false;
		else
			checkForSwimming = true;

		if (area->isInVolume(*MR::getPlayerPos()) && checkForSwimming ? MR::isPlayerSwimming() : true) 
			MarioAccess::forceKill(checkForSwimming ? 3 : 0, 0);
	}

	kmCall(0x8007401C, DeathAreaExtensions);
	kmWrite32(0x8007402C, 0x60000000);


	bool isInDeathFix(const char* name, const TVec3f& pos) {
		if (MR::isInAreaObj(name, pos))
			if (MR::getAreaObj(name, pos)->mObjArg1 == -1 || MR::isInWater(pos))
				return true;

		return false;
	}

	kmBranch(0x8004AC1C, isInDeathFix);

	
	// Fix for Yoshi since he was coded differently.
	kmWrite32(0x804129EC, 0x7FE3FB78); // mr r3, r31
	kmWrite32(0x804129F4, 0x4BBFCABD); // Replace call to MR::isInAreaObj with MR::isInDeath.

	/*
	* Mini Patch: Custom HipDropSwitch colors
	* 
	* A fun but useless patch suggested by Alex SMG.
	*/

	#if defined (ALL) || defined (SMG63)
	void customHipDropSwitchColors(LiveActor* actor, const JMapInfoIter& iter) {
		MR::needStageSwitchWriteA(actor, iter);

		s32 frame = 0;
		MR::getJMapInfoArg1NoInit(iter, &frame);
		MR::startBtpAndSetFrameAndStop(actor, "ButtonColor", frame);
	}
	
	kmCall(0x802AF524, customHipDropSwitchColors);
	#endif

	/*
	* Mini Patch: Ocean Sphere Texture Patch
	* 
	* The TearDropGalaxy and SkullSharkGalaxy checks for setting the texture are in SMG2.
	* Here we change it to read Obj_arg0, so the second texture can be used in custom galaxies.
	*/

	s32 OceanSphereTexturePatch(const JMapInfoIter& rIter) {
		s32 arg = 0;
		MR::getJMapInfoArg0NoInit(rIter, &arg);
		return arg;
	}

	kmWrite32(0x8025CE34, 0x7FC3F378); // mr r3, r30
	kmCall(0x8025CE38, OceanSphereTexturePatch); // Hook

	/*
	* Mini Patch: Yes/No Dialogue Extensions
	* 
	* Adds the ability to create custom Yes/No MSBF dialogue options.
	* 
	* Be sure to add text entries to /SystemMessage.arc/Select.msbt.
	* The format is "Select_New#_Yes", and "Select_New#_No".
	* 
	* Knowledge of MSBF is required for this to be of any use in game.
	*/

	const char* YesNoDialogueExtensions(const TalkMessageCtrl* msg) {
		s16 selectTxt = ((s16*)msg->mTalkNodeCtrl->getNextNodeBranch())[4];

		char* str = new char[5];
		sprintf(str, "New%d", selectTxt - 18);

		return selectTxt < 18 ? msg->getBranchID() : str;
	}

	kmCall(0x80379A84, YesNoDialogueExtensions);

	 void smssKillSamboHeadIfInWater(LiveActor* pActor) {
     if (MR::isInWater(pActor->mTranslation) || MR::isBindedGroundSinkDeath(pActor))
        pActor->kill();
    }

    kmCall(0x801F8290, smssKillSamboHeadIfInWater);

} 