#include "pt/Extras/StageEventDataTable.h"
#include "pt/Extras/WarpAreaSystem.h"
#include "Game/Util.h"
#include "pt/Util/ActorUtil.h"
#include "Game/Screen/LayoutActor.h"
#include "Game/System/AllData/GameSequenceFunction.h"
#include "Game/System.h"
#include "Game/LiveActor.h"

using namespace pt;

/*
* Authors: Evanbowl
* 
* StageEventDataTable:
*
* Code optimation help by Galaxy Master
*
* These functions un-hardcode certain options a stage can have. Those being:
* Chimp, Glider, Disable Pause, Play Star Chance, Purple Coin Over 100, and Story Book.
*/

#if defined (ALL) || defined (NOGLE)
namespace StageEventDataTable {

	const char* flags;
	void* StageEventDataTableBCSV = pt::loadArcAndFile("/SystemData/PTSystemData.arc", "/System/StageEventDataTable.bcsv");
		
	//StageEventDataTable Parser
	bool readTable(const char* value, const char* stageName) {

		JMapInfo* StageDataTable = new JMapInfo();
		StageDataTable->attach(StageEventDataTableBCSV);

		for (s32 i = 0; i < MR::getCsvDataElementNum(StageDataTable); i++) {
			const char *exceptStage = 0;
			s32 exceptScenario = 0;

			MR::getCsvDataStr(&exceptStage, StageDataTable, "StageName", i);
			MR::getCsvDataS32(&exceptScenario, StageDataTable, "ScenarioNo", i);
			MR::getCsvDataStr(&flags, StageDataTable, "Flags", i);

			if (!flags)
				OSReport("(StageEventDataTable) \"Flags\" is null.");

			if ((exceptScenario == 0 || exceptScenario == MR::getCurrentScenarioNo()) && MR::isEqualStringCase(stageName, exceptStage) && strstr(flags, value))
				return true;
		}
		return false;
	}

	bool readTableCurrentStage(const char* value) {
		return StageEventDataTable::readTable(value, MR::getCurrentStageName());
	}

	/*
	* Makes the set galaxy a Chimp stage. If you apply this to a non-chimp star, the high scores will not work.
	* If the high scores did work, that would mean that the save file would only work on hacks with PT.
	*/

	bool isStageScoreAttack() {
		return StageEventDataTable::readTableCurrentStage("Chimp"); 
	}

	bool isStageDisablePauseMenu() {
		return StageEventDataTable::readTableCurrentStage("DisablePause"); //Completely disables the pause menu in the set galaxy.
	}

	bool isStageGlider() {
		return StageEventDataTable::readTableCurrentStage("Glider"); //Makes the set galaxy a Glider stage.
	}

	bool isStagePlayStarChance() {
		return !StageEventDataTable::readTableCurrentStage("DisableStarChanceBGM"); //Makes the set galaxy not set the BGM to star chance after a star spawn.
	}

	bool isStagePurpleCoinsMoreThan100() {
		return StageEventDataTable::readTableCurrentStage("PurpleCoinsOver100"); //Makes purple coins in the set galaxy behave like Tall Trunk/Rolling Coaster.
	}
	
	bool isStageStoryBook() {
	if (MR::isEqualStageName("FileSelect")) //FileSelect will crash if MR::isStageStoryBook returns true on this stage.
		return false;
	return StageEventDataTable::readTableCurrentStage("StoryBook"); //Makes the set galaxy a Story Book stage, adding a story book border and preventing return to the Starship.
	}
	
	bool isStageUseTamakoroBGM() { //Makes the set galaxy play the Slider music when a Star Ball is jumped on.
		return StageEventDataTable::readTableCurrentStage("TamakoroSliderBGM");
	}

	void isStageDisableFallFailsafe() { //Makes the set stage disable the game's failsafe that kills the player if falling for too long.
	if (!StageEventDataTable::readTableCurrentStage("DisableFallFailsafe"))
			MR::forceKillPlayerByAbyss();
	}

	void isStageDisableWorldMapEvents(NerveExecutor* nrv) { // Makes the set stage not trigger World Map events on star get. They'll still happen, but you won't get the cutscene.
		if (!StageEventDataTable::readTable("DisableWorldMapEvents", GameSequenceFunction::getClearedStageName()))
			nrv->updateNerve();
	}

	bool isStageStarPieceFollowGroupLimit() {
		return StageEventDataTable::readTableCurrentStage("StarPieceFollowGroupLimit");
	}


	kmBranch(0x800568F0, isStageScoreAttack);

	kmBranch(0x80056B40, isStageDisablePauseMenu);

	kmBranch(0x800570C0, isStageGlider);

	kmBranch(0x80056DF0, isStagePlayStarChance);

	kmBranch(0x80056D70, isStagePurpleCoinsMoreThan100);

	kmBranch(0x80056BE0, isStageStoryBook);

	kmCall(0x804477B4, isStageUseTamakoroBGM);
	kmWrite32(0x800857EC, 0x38600001);	

	kmCall(0x80387F64, isStageDisableFallFailsafe);

	kmCall(0x804EF3B8, isStageDisableWorldMapEvents);

	kmBranch(0x800567D0, isStageStarPieceFollowGroupLimit);
}
#endif