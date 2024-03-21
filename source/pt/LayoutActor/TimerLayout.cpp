#ifdef ALL
#include "pt/LayoutActor/TimerLayout.h"
#include "Game/System/AllData/GameSequenceFunction.h"
#include "pt/Game/Screen/GameSceneLayoutHolderExt.h"

/*
* Clear Time In Stage Layout
*
* A basic layout made to display your current clear time while in a stage.
*
* Only available on the "ALL" configuration.
*/

GameSceneLayoutHolderExt::GameSceneLayoutHolderExt() : GameSceneLayoutHolder() {}

namespace pt {

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
	initNerve(&NrvTimerLayout::NrvWait::sInstance);
	mLayoutActorFlag.mIsHidden = true;
	appear();
}

void TimerLayout::control() {
	if (!MR::isStageStateScenarioOpeningCamera() || !MR::isBeginScenarioStarter()) {

		if (MR::testCorePadTrigger1(0) && !MR::isStageMarioFaceShipOrWorldMap() && !MR::isEqualStageName("FileSelect"))
				mLayoutActorFlag.mIsHidden = mLayoutActorFlag.mIsHidden ? false : true;

		if (MR::testCorePadTrigger2(0) && mLayoutActorFlag.mIsHidden == false)
			numScenario += 1;

		if (numScenario > MR::getCurrentStagePowerStarNum())
				numScenario = 1;

		if (!MR::isPowerStarGetDemoActive()) {
			currTime = GameSequenceFunction::getGameSequenceInGame()->getPlayResultInStageHolder()->mStageClearTime + 1;
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
			setNerve(&NrvTimerLayout::NrvOnStarGet::sInstance);
		}
	}
}

void TimerLayout::exeAppear() {
	MR::showLayout(this);
}

void TimerLayout::exeDisappear() {
	MR::hideLayout(this);
}

void TimerLayout::exeOnStarGet() {
	if (currTime < savedTime) {
		MR::makeClearTimeString(newRecordDisplay, savedTime - currTime);
		MR::showPaneRecursive(this, "TxtNewRecord");
		MR::setTextBoxFormatRecursive(this, "TxtNewRecord", L"New record! %ls", newRecordDisplay);
	}
}
namespace NrvTimerLayout {
	void NrvWait::execute(Spine* pSpine) const {
		TimerLayout* pActor = (TimerLayout*)pSpine->mExecutor;
	}

	void NrvAppear::execute(Spine* pSpine) const {
		TimerLayout* pActor = (TimerLayout*)pSpine->mExecutor;
		pActor->exeAppear();
	}

	void NrvDisappear::execute(Spine* pSpine) const {
		TimerLayout* pActor = (TimerLayout*)pSpine->mExecutor;
		pActor->exeDisappear();
	}

	void NrvOnStarGet::execute(Spine* pSpine) const {
		TimerLayout* pActor = (TimerLayout*)pSpine->mExecutor;
		pActor->exeOnStarGet();
	}

	NrvWait(NrvWait::sInstance);
	NrvAppear(NrvAppear::sInstance);
	NrvDisappear(NrvDisappear::sInstance);
	NrvOnStarGet(NrvOnStarGet::sInstance);
}

GameSceneLayoutHolderExt* createGameSceneLayoutHolderExt() {
	return new GameSceneLayoutHolderExt();
}

kmCall(0x8045A0A4, createGameSceneLayoutHolderExt);
kmWrite32(0x8045A0A8, 0x48000874);

void initTimerLayout(GameSceneLayoutHolderExt* pHolder, const char* pStr) {
	MR::joinToNameObjGroup(pHolder, pStr);
	if (!MR::isStageFileSelect()) {
		pHolder->mTimeLayout = new TimerLayout();
		pHolder->mTimeLayout->initWithoutIter();
	}
}

kmCall(0x80471650, initTimerLayout);
}
#endif
