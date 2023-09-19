#include "syati.h"
#include "Game/Util.h"
#include "Game/Screen/LayoutActor.h"
#include "Game/NPC/TalkMessageCtrl.h"
#include "pt/Util/ActorUtil.h"

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
			
		return MR::loadTexFromArc(pActor, outArcStr, outBtiStr);
	}

	kmWrite32(0x80254880, 0x60000000); // nop
	kmWrite32(0x80254884, 0x7FA5EB78); // mr r5, r29
	kmCall(0x8025488C, CustomFlagTextures);

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
		if (area->mObjArg0 < 0) {
			MR::getGameSceneLayoutHolder()->mMarioSubMeter->mAirMeter->mLayoutActorFlag.mIsHidden = area->isInVolume(*MR::getPlayerPos());

			bool checkForSwimming;

			if (area->mObjArg1 == -1)
				checkForSwimming = false;
			else
				checkForSwimming = true;

			if (area->isInVolume(*MR::getPlayerPos()) && checkForSwimming ? MR::isPlayerSwimming() : true) 
				MarioAccess::forceKill(checkForSwimming ? 3 : 0, 0);
		}
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

	void customHipDropSwitchColors(LiveActor* actor, const JMapInfoIter& iter) {
		MR::needStageSwitchWriteA(actor, iter);

		f32 frame = 0;
		MR::getJMapInfoArg1NoInit(iter, &frame);
		MR::startBtpAndSetFrameAndStop(actor, "ButtonColor", frame);
	}
	
	kmCall(0x802AF524, customHipDropSwitchColors);
	
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
		u16 selectTxt = ((u16*)msg->mTalkNodeCtrl->getNextNodeBranch())[4];

		char* str = new char[5];
		sprintf(str, "New%d", selectTxt - 18);

		return selectTxt < 18 ? msg->getBranchID() : str;
	}

	kmCall(0x80379A84, YesNoDialogueExtensions);

	void killSamboHeadIfInWater(LiveActor* pActor) {
     if (MR::isInWater(pActor->mTranslation) || MR::isBindedGroundSinkDeath(pActor))
        pActor->kill();
    }

    kmCall(0x801F8290, killSamboHeadIfInWater);
	
	#ifdef WIP
	void restartObjInitMessage(LiveActor* pActor, const JMapInfoIter& rIter, const char* pStr) {
		MR::processInitFunction(pActor, rIter, pStr, false);

		s32 msgID = -1;
		MR::getJMapInfoMessageID(rIter, &msgID);

		LayoutActor* pLayout = MR::createSimpleLayout("textlyt", "TextLayout", 0);
		pLayout->initWithoutIter();

		MR::setTextBoxFormatRecursive(pLayout, "Text00", L"");
		if (msgID > -1) {
		char* text = new char[0xE];
		snprintf(text, 0xE, "RestartObj%03d", msgID);
		MR::setTextBoxGameMessageRecursive(pLayout, "Text00", text);
		}
	
		asm("stw %0, 0x9C(%1)" : "=r" (pLayout) : "=r" (pActor));
	}


	kmWrite32(0x8033FC44, 0x386000A0); // li r3 0xA4

	//kmWrite32(0x802F175C, 0x80DF009C); // lwz r6, 0x9C(r31)
	kmCall(0x802F1778, restartObjInitMessage);

	void restartObjActivateMessage(LiveActor* pActor, LayoutActor* pLayout) {
		MR::startActionSound(pActor, "Get", -1, -1, -1);

		pLayout->appear();
		//MR::startAnim(pLayout, "Show", 0);
	}

	kmWrite32(0x802F1968, 0x809F009C);
	kmCall(0x802F1978, restartObjActivateMessage);
	#endif
	
	// Suggested by Xandog
	const wchar_t* CustomGreenStarNames(GalaxyStatusAccessor accessor, const char* pStageName, s32 starid) {
		char textName[256];
	    snprintf(textName, 256, "ScenarioName_%s_GreenStar%d", pStageName, starid-accessor.getPowerStarNum()/2);
	
	    TalkMessageInfo info;
	    MessageSystem::getGameMessageDirect(&info, textName);
	
	    if (info.mMessage)
	        return info.mMessage;
	
	    snprintf(textName, 256, "ScenarioName_GreenStar%d", starid-accessor.getPowerStarNum()/2);
	    return MR::getGameMessageDirect(textName);
	}
	
	kmWrite32(0x8004159C, 0x7FC4F378); // mr r4, r30
	kmWrite32(0x800415A0, 0x7FE5FB78); // mr r5, r31
	kmCall(0x800415A4, CustomGreenStarNames);
	kmWrite32(0x800415A8, 0x48000050); // b 0x48

	//kmWrite32(0x8048ED84, 0x38600000); // li r3, 0

	#if defined (SMG63) || defined (ALL)
	void SnowBallDieInWater(LiveActor* pActor, const TVec3f& rPos1, const TVec3f& rPos2) {
		MR::makeMtxUpNoSupportPos(&((TMtx34f*)pActor)[0x3], rPos1, rPos2);

		if (MR::isInWater(pActor->mTranslation)) {
			AreaObj* pArea = MR::getAreaObj("WaterArea", pActor->mTranslation);

			if (pArea)
				if (pArea->mObjArg2 >= 0)
					pActor->kill();
		}
	}

	kmWrite32(0x80301C64, 0x7FA3EB78);
	kmCall(0x80301C6C, SnowBallDieInWater);
	#endif
	
	//void stinkyAudioLib(AudSoundNameConverter* pConverter, int* pInt, const char* pStr) {
	//	pConverter->getSoundID(pInt, pStr);
	//	OSReport("amogus: %p, %d, %s\n", pConverter, *pInt, pStr);
	//}
//
	//kmCall(0x8005B3E8, stinkyAudioLib);
	//kmCall(0x8005B424, stinkyAudioLib);
	//kmCall(0x8005AE94, stinkyAudioLib);
} 