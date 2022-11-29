#include "pt/MapObj/PowerStarSpawner.h"
#include "pt/Extras/PowerStarColors.h"
#include "Game/Util.h"
#include "Game/Player/MarioAccess.h"
#include "Game/LiveActor.h"
#include "Game/System/GameSystem.h"
#include "pt/LayoutActor/TimerLayout.h"

namespace pt {

/*
* This version of the PowerStarSpawner is exclusive to PT Debug.
*
* Created by Aurum, Evanbowl & Someone, with Group ID support thanks to Zyphro.
* 
* Spawns the specified Power Star if its SW_A is activated. The delay until the star spawns as well
* as the position from where the star spawns can be specified as well.
*
* This object is a bit overdeveloped but is one of my favorites.
*
* One of my favorite objects!
*
	Obj_args:
	0: Power Star ID, int
	1: Spawn Type, int
	2: Spawn Delay, int
	3: Use Sound Effect, bool
	4: Use Mario's Position, bool
	5: Use Display Model, int
	6: Y Offset for spawning at the player, float, Default is 300.
*/

PowerStarSpawner::PowerStarSpawner(const char* pName) : LiveActor(pName) {
	mScenario = 1;
	mSpawnMode = -1;
	mDelay = 0;
	mUseSE = 0;
	mFromMario = 0;
	mDisplayStarMode = -1;
	mElapsed = -1;
	mCamInfo = 0;
	mFrame = -1;
}

void PowerStarSpawner::init(JMapInfoIter const& rIter) {
	MR::initDefaultPos(this, rIter);

	MR::processInitFunction(this, "StarPiece", false);
	MR::hideModel(this); // A model is specified then hidden since it is not necessary, or else the ModelObj will crash the game.
	MR::connectToSceneMapObj(this);
	MR::invalidateClipping(this); // This object will never unload when offscreen.
	MR::calcGravity(this);

	MR::useStageSwitchReadA(this, rIter); // Reads SW_A.
	MR::useStageSwitchReadB(this, rIter); // Reads SW_B

	MR::getJMapInfoArg0NoInit(rIter, &mScenario); // Star ID
	MR::getJMapInfoArg1NoInit(rIter, &mSpawnMode); // Time Stop/Instant Appear/Squizzard Spawn
	MR::getJMapInfoArg2NoInit(rIter, &mDelay); // Delay before spawn.
	MR::getJMapInfoArg3NoInit(rIter, &mUseSE); // Play a sound effect?
	MR::getJMapInfoArg4NoInit(rIter, &mFromMario); // Should the Star start it's spawn path at Mario?
	MR::getJMapInfoArg5NoInit(rIter, &mDisplayStarMode); // Show display model?
	MR::getJMapInfoArg6NoInit(rIter, &mFrame);

	initSound(1, "PowerStarSpawner", &mTranslation, 0);

	MR::declarePowerStar(this, mScenario); // Declares the star determined by mScenario.
	makeActorAppeared();

	if (mDisplayStarMode > -1) {
		createDisplayStar(); // Creates the Power Star Display model
		MR::calcGravity(DisplayStar);
	}
}

void PowerStarSpawner::movement() {

	if (MR::isValidSwitchB(this)) {
		if (MR::isOnSwitchB(this)) {
			MR::showModel(DisplayStar);
			MR::tryEmitEffect(DisplayStar, "Light");
		}
		else {
			MR::hideModel(DisplayStar);
			MR::tryDeleteEffect(DisplayStar, "Light");
		}
	}

	if (mDisplayStarMode == 1)
		MR::rotateMtxLocalYDegree((MtxPtr)&DisplayStarMtx, 3.0f);

	if (MR::isOnSwitchA(this))
		spawnStar();
}

void PowerStarSpawner::spawnStar() {
	if (mFromMario) {
		MR::setPosition(this, *MR::getPlayerPos());
		JMAVECScaleAdd((Vec*)MarioAccess::getPlayerActor()->getGravityVec(), (Vec*)&mTranslation, (Vec*)&mTranslation, -250);
	}

	mElapsed++;

	if (mElapsed == 0 && mUseSE)
		MR::startLevelSound(this, "OjPowerStarSpawnerSpawn", -1, -1, -1);

	if (mElapsed >= mDelay && !MR::isPlayerInRush()) {
		switch (mSpawnMode) { // I do not know how to improve this section.
			case -1:
			MR::appearEventPowerStar("PowerStarSpawner", mScenario, &mTranslation, 0, 0, 0);
			break;
			case 0:
			MR::appearEventPowerStar("PowerStarSpawner", mScenario, &mTranslation, 1, 0, 0);
			break;
			case 1:
			MR::appearEventPowerStar("PowerStarSpawner", mScenario, &mTranslation, 0, 1, 0);
			break;
			case 2:
			MR::appearEventPowerStar("PowerStarSpawner", mScenario, &mTranslation, 0, 0, 1);
			break;
			case 3:
			MR::appearEventPowerStar("PowerStarSpawner", mScenario, &mTranslation, 1, 0, 1);
			break;
		}

		if (mDisplayStarMode > -1)
			DisplayStar->makeActorDead();

		makeActorDead();
	}
}

// Display Star
void PowerStarSpawner::createDisplayStar() {
		DisplayStar = new ModelObj("パワースター", "PowerStar", mDisplayStarMode ? (MtxPtr)DisplayStarMtx : NULL, -2, -2, -2, false);

		if (mDisplayStarMode == 0) {
			DisplayStar->mRotation.set(mRotation);
			DisplayStar->mTranslation.set(mTranslation);
		}

		MR::setMtxTrans((MtxPtr)DisplayStarMtx, mTranslation); // Set the mtx translation to the PowerStarSpawner's mTranslation.

		MR::invalidateShadowAll(DisplayStar); // Shadows are not needed so they are hidden.

		if (mDisplayStarMode == 1) {
		upVec.set<f32>(-mGravity); // Sets the up vector to what the gravity is. This allows the DisplayStar to calculate it's gravity, like the normal PowerStar.
		MR::makeMtxUpFront((TMtx34f*)&DisplayStarMtx, upVec, mTranslation);
		MR::setMtxTrans((MtxPtr)&DisplayStarMtx, mTranslation);
		}

		// Set up the color to match the PowerStarType
			MR::startBva(DisplayStar, "PowerStarColor");

		if (!MR::hasPowerStarInCurrentStage(mScenario)) { // Checks if you have the specified star. If not, set up the color by setting animation frames.
			
			#ifndef GLE
			if (mFrame == -1)
				mFrame = pt::getPowerStarColorCurrentStage(mScenario);
			#endif

			MR::startBtp(DisplayStar, "PowerStarColor");
			MR::startBrk(DisplayStar, "PowerStarColor");
			MR::startBtk(DisplayStar, "PowerStarColor");

			MR::setBtpFrameAndStop(DisplayStar, mFrame);
			MR::setBrkFrameAndStop(DisplayStar, mFrame);
			MR::setBtkFrameAndStop(DisplayStar, mFrame);
			MR::setBvaFrameAndStop(DisplayStar, 0);
		}

		DisplayStar->appear();

		if (MR::isValidSwitchB(this))
			MR::hideModel(DisplayStar);
		else
			MR::emitEffect(DisplayStar, "Light"); // Starts the PowerStar effect "Light" on the DisplayStar.
}
}