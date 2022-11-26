#include "pt/LayoutActor/TimerLayout.h"
#include "Game/Util.h"
#include "Game/Player.h"
#include "Game/System.h"
#include "Game/System/AllData/GameSequenceFunction.h"
#include "Game/Screen/CounterLayoutController.h"

#ifdef ALL
using namespace pt;

s32 numScenario = 1; // Checked scenario is 1 by default.
bool isHidden = false;

TimerLayout::TimerLayout() : LayoutActor("TimerLayout", 0) {
	currTime = 0;
	savedTime = 0;
	currTimeDisplay = L"";
	savedTimeDisplay = L"";
	newRecordDisplay = L"";
}

void TimerLayout::init(const JMapInfoIter &rIter) {
	MR::connectToSceneLayout(this);
	initLayoutManager("TimerLayout", 1);
	appear();
}

void TimerLayout::control() {
	if (!MR::isStageStateScenarioOpeningCamera() || MR::isBeginScenarioStarter()) {

		if (MR::testCorePadTrigger1(0))
				mLayoutActorFlag.mIsHidden = mLayoutActorFlag.mIsHidden ? false : true;

		if (MR::testCorePadTrigger2(0) && mLayoutActorFlag.mIsHidden == false)
			numScenario += 1;

		if (numScenario > MR::getCurrentStagePowerStarNum())
				numScenario = 1;

		if (!MR::isPowerStarGetDemoActive()) {
			currTime = GameSequenceFunction::getGameSequenceInGame()->getPlayResultInStageHolder()->mCurrentClearTime + 1;
			savedTime = GameDataFunction::makeSomeGalaxyStorage(MR::getCurrentStageName())->getScenarioAccessor(numScenario)->mBestTime;

			MR::makeClearTimeString(currTimeDisplay, currTime);
			MR::showPaneRecursive(this, "TxtCurrentTime");
			MR::setTextBoxFormatRecursive(this, "TxtCurrentTime", L"Time: %ls", currTimeDisplay);

			if (MR::hasPowerStarInCurrentStage(numScenario)) {
				MR::makeClearTimeString(savedTimeDisplay, savedTime);
				MR::showPaneRecursive(this, "TxtBestTime");
				MR::setTextBoxFormatRecursive(this, "TxtBestTime", L"Star %d Best: %ls", numScenario, savedTimeDisplay);
			}
			else {
				MR::showPaneRecursive(this, "TxtBestTime");
				MR::setTextBoxFormatRecursive(this, "TxtBestTime", L"Star %d Best: None", numScenario);
			}
		}

		if (MR::isPowerStarGetDemoActive() && MR::hasPowerStarInCurrentStage(numScenario)) {
			if (currTime < savedTime) {
				MR::makeClearTimeString(newRecordDisplay, savedTime - currTime);
				MR::showPaneRecursive(this, "TxtNewRecord");
				MR::setTextBoxFormatRecursive(this, "TxtNewRecord", L"New record! %ls", newRecordDisplay);
			}
		}
	}
}

//void initTimerLayout(LayoutActor* layout) {// This function keeps the layout initialized whenever possible.
//	MR::connectToSceneLayout(layout);
//
//	TimerLayout* TimerLayout = new pt::TimerLayout();
//
//	if (!MR::isStageMarioFaceShipOrWorldMap() && !MR::isEqualStageName("FileSelect"))
//		TimerLayout->initWithoutIter();
//		TimerLayout->mLayoutActorFlag.mIsHidden = true;
//}
//
//kmCall(0x804657A0, initTimerLayout);

void initTimerLayout(CounterLayoutController* layout) {
	MR::connectToSceneLayout(layout);

	layout->mTimerLayout = new TimerLayout();

	if (!MR::isStageMarioFaceShipOrWorldMap() && !MR::isEqualStageName("FileSelect"))
		layout->mTimerLayout->initWithoutIter();
}

kmCall(0x804657A0, initTimerLayout);

kmWrite32(0x80471780, 0x38600050);

#endif
