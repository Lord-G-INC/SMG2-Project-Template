#ifdef NOGLE
#include "pt/Extras/StageEventDataTable.h"
#include "pt/Extras/WarpAreaSystem.h"
#include "Game/Util.h"
#include "pt/Util/ActorUtil.h"
#include "Game/System/AllData/GameSequenceFunction.h"
#include "pt/init.h"

namespace pt {

/*
* Authors: Evanbowl
* 
* StageEventDataTable:
*
* Code optimation help by Galaxy Master
*
* These functions un-hardcode certain options a stage can have. Those being:
* Chimp, Glider, Disable Pause, Play Star Chance, Purple Coin Over 100, and Story Book.
*
* This feature is exclusive to PTD.
* This feature is disabled on GLE builds.
*/

namespace StageEventDataTable {

	const char* flags;
		
	//StageEventDataTable Parser
	bool readTable(const char* value, const char* stageName) {

		JMapInfo* StageDataTable = new JMapInfo();
		StageDataTable->attach(gStageEventDataTableBCSV);

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

	void isStageDisableFallFailsafe() { //Makes the set stage disable the game's failsafe that kills the player if falling for too long.
		if (!StageEventDataTable::readTableCurrentStage("DisableFallFailsafe"))
				MR::forceKillPlayerByAbyss();
	}

	void isStageDisableWorldMapEvents(NerveExecutor* nrv) { // Makes the set stage not trigger World Map events on star get. They'll still happen, but you won't get the cutscene.
		if (!StageEventDataTable::readTable("DisableWorldMapEvents", GameSequenceFunction::getClearedStageName()))
			nrv->updateNerve();
	}


	kmBranch(0x800568F0, isStageScoreAttack);

	kmBranch(0x80056B40, isStageDisablePauseMenu);

	kmBranch(0x800570C0, isStageGlider);

	kmBranch(0x80056DF0, isStagePlayStarChance);

	kmBranch(0x80056D70, isStagePurpleCoinsMoreThan100);

	kmBranch(0x80056BE0, isStageStoryBook);

	kmCall(0x80387F64, isStageDisableFallFailsafe);

	kmCall(0x804EF3B8, isStageDisableWorldMapEvents);
}

}
#endif