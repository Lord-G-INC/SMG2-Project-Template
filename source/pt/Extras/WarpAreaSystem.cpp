#include "pt/Extras/WarpAreaSystem.h"
#include "Game/Util.h"
#include "pt/Util/ActorUtil.h"
#include "Game/System/GameSequenceInGame.h"
#include "c_stdlib.h"

/*
* Authors: Evanbowl
* 
* WarpAreaSystem:
*
* The other half of WarpArea's functionality.
* Reads the BCSV at /SystemData/PTSystemData.arc/WarpAreaStageTable.bcsv to determine what galaxy it should take the player to.
*
* WarpAreaErrorLayout appears when specific parts of the WarpArea are set up incorrectly.
*
* This feature is exclusive to PTD.
* This feature is disabled on GLE builds.
*/

#ifdef NOGLE

void* sWarpAreaStageTableBCSV = pt::loadArcAndFile("/SystemData/PTSystemData.arc", "/System/WarpAreaStageTable.bcsv");
static s32 gLastTableIndex = -1;

namespace WarpAreaStageTable {
	void readTable(s32 selectedindex, bool useErrors) {

		JMapInfo table = JMapInfo();
		table.attach(sWarpAreaStageTableBCSV);
		s32 elementNum = MR::getCsvDataElementNum(&table);
		s32 targetLine = -1;
		s32 index;

		for (s32 i = 0; i < elementNum; i++) {
			MR::getCsvDataS32(&index, &table, "Index", i);

			if (selectedindex == index) {
				targetLine = i;
				break;
			}
		}

		if (targetLine == -1) {
		}

		const char* stageName;
		s32 scenarioNo;
		s32 greenStarNo;
		
		MR::getCsvDataStr(&stageName, &table, "StageName", targetLine);
		MR::getCsvDataS32(&scenarioNo, &table, "ScenarioNo", targetLine);
		MR::getCsvDataS32(&greenStarNo, &table, "GreenStarScenarioNo", targetLine);

		bool canWarp = true;

		if (scenarioNo < 1 || scenarioNo > 8) {		
			canWarp = false;
		}

		if (greenStarNo < -1 || greenStarNo > 4 || greenStarNo == 0) {
			canWarp = false; 
		}
		else
			greenStarNo + 3;

		char str[128];
		sprintf(str, "/StageData/%s/%sMap.arc", stageName, stageName);

		if (!MR::isFileExist(str, false)) {
			canWarp = false; 
		}


		if (canWarp) {
			gLastTableIndex = targetLine;
			GameSequenceFunction::changeToScenarioSelect(stageName);
			GameSequenceFunction::changeSceneStage(stageName, scenarioNo, greenStarNo, 0);
		}
		else {
			MR::openSystemWipeCircle(45);
			MR::onPlayerControl(1);
		}
	}

	void selectWipeClose(s32 type, s32 fadeTime) {
	if (fadeTime == -1)
		fadeTime = 45;

	switch (type) {
		default:
		case 0:
		MR::closeSystemWipeCircle(fadeTime);
		break;
		case 1:
		MR::closeSystemWipeFade(fadeTime);
		break;
		case 2:
		MR::closeSystemWipeWhiteFade(fadeTime);
		break;
		case 3:
		MR::closeSystemWipeCircleWithCaptureScreen(fadeTime);
		break;
		case 4:
		MR::closeSystemWipeFadeWithCaptureScreen(fadeTime);
		break;
		case 5:
		MR::closeSystemWipeMario(fadeTime);
		break;
		}
	}

	void selectWipeOpen(s32 type, s32 fadeTime) {
	if (fadeTime == -1)
		fadeTime = 45;

	switch (type) {
		default:
		case 0:
		MR::openSystemWipeCircle(fadeTime);
		break;
		case 1:
		MR::openSystemWipeFade(fadeTime);
		break;
		case 2:
		MR::openSystemWipeWhiteFade(fadeTime);
		break;
		case 3:
		MR::openSystemWipeMario(fadeTime);
		}
	}


	// Doesn't work
	void setWipeOnStageLoad() {
		JMapInfo table = JMapInfo();
		table.attach(sWarpAreaStageTableBCSV);

		s32 wipeType = 0;
		s32 wipeTime = 0;

		if (gLastTableIndex > -1) {
			MR::getCsvDataS32(&wipeType, &table, "WipeType", gLastTableIndex);
			MR::getCsvDataS32(&wipeType, &table, "WipeTime", gLastTableIndex);
			selectWipeOpen(wipeType, wipeTime);
			gLastTableIndex = -1;
		}
		else
			MR::openSystemWipeWhiteFade(90); //If no, use the default wipe and wipe time.
	}

	kmCall(0x804B44D0, setWipeOnStageLoad); //sub_804B4490 + 0x40
	

	// Error Layout Code
}

	ErrorLayout::ErrorLayout() : LayoutActor("WarpAreaErrorLayout", 0) {}

	void ErrorLayout::init(const JMapInfoIter& rIter) {
		MR::connectToSceneLayout(this);
		initLayoutManager("WarpAreaErrorLayout", 0);
		MR::setTextBoxFormatRecursive(this, "Text00", L"");
		appear();
	}

	//This function prints text to the TxtText and ShaText panes.
	void ErrorLayout::printf(bool canPrint, const char* format, ...) {
		va_list arg;
		char string[128];

		va_start(arg, format);
		vsnprintf(string, sizeof(string), format, arg);
		va_end(arg);

		if (canPrint)
			MR::setTextBoxFormatRecursive(this, "Text00", L"%s", string);
	}
#endif