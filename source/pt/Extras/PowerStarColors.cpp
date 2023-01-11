#include "pt/Extras/PowerStarColors.h"
#include "pt/Util/ActorUtil.h"
#include "Game/MapObj/Tamakoro.h"

/*
* Authors: Aurum
* 
* This changes all code that apply Power Star colors to support new and custom colors as well. To accomplish this, the
* PowerStarType in the scenario file is changed to easily configure the color for a star. The new types are Bronze, Red,
* Blue and LegacyGreen. As the Green type from the base game has to be unlocked first, the LegacyGreen type was added in
* to ignore these conditions effectively.
*/
namespace pt {
	/*
	* This is the function to retrieve the color ID based on the specified scenario's PowerStarType value. This function
	* creates a GalaxyStatusAccessor for the specified stage in order to obtain the value to be found. Following this are
	* the color IDs and their corresponding string names.
	* 0: "Normal", "Hidden", anything else
	* 1: "Bronze"
	* 2: "Green", "LegacyGreen"
	* 3: "Red"
	* 4: nothing, this is the transparent color
	* 5: "Blue"
	* 6: "Silver" [PTD]
	*/

	s32 getPowerStarColor(const char *pStage, s32 scenarioId) {
		const char* type;
		GalaxyStatusAccessor gsa(MR::makeGalaxyStatusAccessor(pStage));
		gsa.mScenarioData->getScenarioString("PowerStarType", scenarioId, &type);

		if (MR::isEqualSubString(type, "Bronze")) {
			return 1;
		}
		else if (MR::isEqualSubString(type, "Green") || MR::isEqualSubString(type, "LegacyGreen")) {
			return 2;
		}
		else if (MR::isEqualSubString(type, "Red")) {
			return 3;
		}
		else if (MR::isEqualSubString(type, "Blue")) {
			return 5;
		}
		else if (MR::isEqualSubString(type, "Silver")) { // Shh, you see nothing.
			return 6;
		}

		return 0;
	}

	/*
	* This function returns the color ID for the specified scenario in the current stage.
	*/
	s32 getPowerStarColorCurrentStage(s32 scenarioId) {
		return getPowerStarColor(MR::getCurrentStageName(), scenarioId);
	}

	#if defined (ALL) || defined (NOGLE) // GLE has its own Star Color system, so we can disable ours.
	/*
	* At the given address in PowerStar::initMapToolInfo, there is a check which always results true, allowing us to
	* rewrite these instructions. Here, we load the color ID based on the PowerStarType setting in ScenarioData.bcsv.
	* All the checks to apply the transparent or bronze color are done later on.
	*/
	kmWrite32(0x802E01B8, 0x7C030378);                 // copy r0 (mPowerStarID) to r3
	kmCall(0x802E01BC, getPowerStarColorCurrentStage); // retrieve color ID
	kmWrite32(0x802E01C0, 0x907E0130);                 // store result (r3) in mColor
	
	/*
	* initMapToolInfo still is not perfect meaning that there is still a thing to take proper care of. If not a Green
	* Star, the color ID should not default to 0 (gold). Instead, we want our previously loaded color to be used. This
	* replaces "li r3, 0".
	*/
	kmWrite32(0x802E01F0, 0x807E0130); // load mColor into r3
	
	/*
	* This adjusts the Star Select scene to apply our new colors correctly. Basically, we replaced the check for Green
	* Stars and extended it with our colors.
	*/
	kmCall(0x802DFC14, getPowerStarColorCurrentStage); // redirection hook
	kmWrite32(0x802DFC20, 0x60000000);                 // NOP-out returning green all the time
	
	/*
	* Lastly, we still have to patch the Star return cutscene. This one could possibly be improved in the future to
	* reduce all the kmWrite calls.
	*/
	kmCall(0x802DF0FC, getPowerStarColor); // redirection hook
	kmWrite32(0x802DF100, 0x7C7C1B78);     // copy result from r3 to r28
	kmWrite32(0x802DF104, 0x2C1C0000);     // compare r28 to 0
	kmWrite32(0x802DF108, 0x41820008);     // branch to apply gold or transparent
	
	/*
	* Whenever a Power Star is created for a cutscene, a different set of functions is used to retrieve and apply the
	* color. This is a simple fix, however. For this, the function MR::isPowerStarGreenInCurrentStageWithDeclarer has
	* been changed to return the Power Star color for the specific scenario instead of checking whether the scenario
	* contains a Green Star. This effectively changes the returned value from bool to s32.
	*/
	kmCall(0x8035BAC0, getPowerStarColorCurrentStage); // redirection hook
	kmWrite32(0x802DF02C, 0x7C7D1B78); // copy result from r3 to r29
	kmWrite32(0x802DF030, 0x4800000C); // skip unnecessary instructions

	kmWrite32(0x804CB8BC, 0x48169D65); // This uses strstr instead of MR::isEqualString in the isPowerStarTypeHidden__12ScenarioDataCFl function. Allows types like BlueHidden to work.
	#endif
	/*
	* Power Star Font Icons
	*
	* On the World Map and Star List, the game displays star icons based on the Power Star type and Comet.
	* However, we've changed this so that new and custom star types can be displayed.
	*
	* Here we load a custom BRFNT from PTSystemData.arc so we do not have to edit the font in all languages.
    *
	* When the game selects which icon to display, it calls MR::addPictureFontCode and r4 is used to deter-
	* mine which icon from the BRFNT should be used. Here we use that to check which icon type to use, 
	* then run a check for Power Star color. 
	*/

	void loadPTPictureFont() {
		pt::loadArcAndFile("/SystemData/PTSystemData.arc", "/Font/PictureFont.brfnt");
	}

	#if defined (ALL) || defined (NOGLE) // GLE has its own Star Color system, so we can disable ours.
		kmCall(0x804B8048, loadPTPictureFont);
	#endif

	void getStarIcon(wchar_t* pStr, s32 type) {
		const char* stage = 0;
		s32 scenarioId;
		s32 icon;

		asm("mr %0, r27" : "=r" (stage));
		asm("mr %0, r31" : "=r" (scenarioId));

		s32 starColor = getPowerStarColor(stage, scenarioId);

		if (type == 0x37) { // Normal Star icons
			icon = 0x100 + starColor;

			if (icon == 0x100)
				icon = 0x37;
			else if (icon == 0x101)
				icon = 0x72;
			else if (icon == 0x106)
				icon = 0x4A;
			}

		else if (type == 0x65) {// Comet Star icons
			icon = 0x110 + starColor;

			if (icon == 0x110)
				icon = 0x65;
			else if (icon == 0x111)
				icon = 0x7D;
			else if (icon == 0x112)
				icon = 0x4F;
			}

		else if (type == 0x71) {// Uncollected Hidden Star icons
			icon = 0x120 + starColor;

			if (icon == 0x120)
			 	icon = 0x71;
			}

		MR::addPictureFontCode(pStr, icon);
	}

	#if defined (ALL) || defined (NOGLE) // GLE has its own Star Color system, so we disable ours.
		kmCall(0x80041E30, getStarIcon); // Normal Star icons
		kmCall(0x80041F0C, getStarIcon); // Comet Star icons
		kmCall(0x80041F94, getStarIcon); // Hidden Star icons
		kmCall(0x80041F48, getStarIcon); // Collected Hidden Star icons
	#endif
	
	/*
	*	Star Ball: Custom Ball and Star Colors
	*
	*	The Power Star inside the Star Ball is not a display model.
	*	Here we set the Star and Ball's color by checking the color
	*	of the Scenario specified by Obj_arg1.
	*
	*	Not really useful, but is neat.
	*/

	void TamakoroCustomPowerStarColors(Tamakoro* actor, const JMapInfoIter& iter) {		
		s32 argScenario = 0;
		s32 colorFrame = -1;
		
		// Check Obj_arg1. This will be the scenario ID to check the Power Star Color of.
		MR::getJMapInfoArg1NoInit(iter, &argScenario);	
		MR::getJMapInfoArg2NoInit(iter, &colorFrame);

		if (colorFrame == -1)
			colorFrame = pt::getPowerStarColorCurrentStage(argScenario);
	
		if (colorFrame == 1)
			MR::startBtkAndSetFrameAndStop(actor, "BallStarColor", 1);
		
		// BTP and BRK animations are started and set using colorFrame.
		MR::startBtpAndSetFrameAndStop(actor, "BallStarColor", colorFrame);
		MR::startBrkAndSetFrameAndStop(actor, "BallColor", colorFrame);

	}
	
	kmWrite32(0x8044461C, 0x7F84E378); // mr r4, r28
	kmCall(0x80444620, TamakoroCustomPowerStarColors);


	// Define new particles.
	// These will be picked using the Star Ball's current BTP frame.
	const char* cNewParticles[7] = {"BreakYellow", "BreakBronze", "BreakGreen", "BreakRed", "BreakClear", "BreakBlue", "BreakSilver"};
	void TamakoroCustomPowerStarColorsParticles(Tamakoro* actor) {
		MR::emitEffect(actor, "Break"); // Emit the original particle. The crystal particles were removed so custom ones can be added to replace it.
		MR::emitEffect(actor, cNewParticles[(s32)MR::getBtpFrame(actor)]); // Emit custom crystal particles picked by the current BTP frame.
	}

	kmCall(0x80446B4C, TamakoroCustomPowerStarColorsParticles);

	const char* starParticleStr[3] = {"Light", "LightBronze", "LightGreen"};
	void greenStarAppearParticleFix(LiveActor* pActor, s32 mColor) {
		MR::emitEffect(pActor, starParticleStr[mColor == 1 || mColor == 2 ? mColor : 0]);
	}

	kmWrite32(0x802E0868, 0x809D0130); // lwz r4, 0x130(r29)
	kmCall(0x802E0870, greenStarAppearParticleFix);

	Color8 starLightColors[2] = {Color8(0, 0, 128, 0), Color8(128, 128, 128, 0)};
	void customPowerStarLightColors(LiveActor* pActor, TVec3f pos, Color8 color, f32 f, s32 mColor) {
		MR::requestPointLight(pActor, pos, mColor > 4 ? starLightColors[mColor - 5]: color, f, -1);
	}

	#if defined (NOGLE) || defined (ALL)
		kmWrite32(0x802DFE00, 0x80DE0130); // lwz r6, 0x130(r30)
		kmCall(0x802DFE04, customPowerStarLightColors);
	#endif

	/*
	*	Silver Star Colors
	*	A suggestion from Alex SMG and others that turned out to be really fun to make.
	*
	*	This will be removed if people do not use it, since it's pointless.
	*/
	
	#ifdef ALL
	void SilverStarColors(LiveActor* actor, const JMapInfoIter& iter) {
		MR::initUseStageSwitchWriteA(actor, iter);
		s32 frame = 6;
		MR::getJMapInfoArg0NoInit(iter, &frame);
		MR::startBtpAndSetFrameAndStop(actor, "Color", frame);
	}

	kmCall(0x8035F830, SilverStarColors);
	#endif
	}
