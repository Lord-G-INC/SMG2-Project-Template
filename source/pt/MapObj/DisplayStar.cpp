#include "pt/MapObj/DisplayStar.h"
#include "pt/Extras/PowerStarColors.h"

/*
* Display Power Star
*
* A "fake" power star used for decoration or being a dummy star.
*
* Work in progress. Exclusive to PTD.
*/

DisplayStar::DisplayStar(const char* pName) : LiveActor(pName) {
    mIsRotate = -1;
    mIsGrandStar = false;
    mScenario = 0;
    mFrame = -1;
}

void DisplayStar::init(const JMapInfoIter& rIter) {
        MR::getJMapInfoArg0NoInit(rIter, &mScenario);
        MR::getJMapInfoArg1NoInit(rIter, &mIsRotate);
        MR::getJMapInfoArg2NoInit(rIter, &mIsGrandStar);
        MR::getJMapInfoArg3NoInit(rIter, &mFrame);

        MR::useStageSwitchReadA(this, rIter);

		const char* enname = mIsGrandStar ? "GrandStar" : "PowerStar";

        initModelManagerWithAnm(enname, NULL, NULL, 0);
        initEffectKeeper(1, enname, 0);

        MR::calcGravity(this);
        MR::initDefaultPos(this, rIter);
        MR::connectToSceneMapObj(this);
        MR::registerDemoSimpleCastAll(this);

		if (mIsRotate) {
			MR::setRotation(this, mRotation);
			MR::setPosition(this, mTranslation);
		}

		MR::invalidateShadowAll(this); // Shadows are not needed so they are hidden.

		// Set up the color to match the PowerStarType

	    if (mIsGrandStar) {
			MR::hideMaterial(this, "GrandStarBronze");
			MR::hideMaterial(this, MR::hasPowerStarInCurrentStage(mScenario) ? "FooMat" : "GrandStarEmpty");
		}
        else {
			MR::startBva(this, "PowerStarColor");
            setupColor(mScenario);
        }

        MR::emitEffect(this, "Light");

		appear();
}

void DisplayStar::control() {
    if (MR::isValidSwitchA(this))
        if (MR::isOnSwitchA(this))
            makeActorDead();
}

void DisplayStar::setupColor(s32 scenario) {
    if (!MR::hasPowerStarInCurrentStage(mScenario)) { // Checks if you have the specified star. If not, set up the color by setting animation frames.
		
	#ifdef NOGLE
		if (mFrame == -1)
			mFrame = pt::getPowerStarColorCurrentStage(mScenario);
	#endif

		MR::startBtpAndSetFrameAndStop(this, "PowerStarColor", mFrame);
		MR::startBrkAndSetFrameAndStop(this, "PowerStarColor", mFrame);
		MR::startBtkAndSetFrameAndStop(this, "PowerStarColor", mFrame);

		MR::setBvaFrameAndStop(this, 0);
	}
}