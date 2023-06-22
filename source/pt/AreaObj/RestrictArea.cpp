#ifdef SMG63
#include "pt/AreaObj/RestrictArea.h"

RestrictArea::RestrictArea(const char* pName) : AreaObj(pName) {
    mIsLeftArea = false;
}

void RestrictArea::init(const JMapInfoIter& rIter) {
    AreaObj::init(rIter);
	MR::connectToSceneAreaObj(this);
}

void RestrictArea::movement() {
    if (!mIsLeftArea) {
        if (isInVolume(*MR::getPlayerPos())) {
            if (mObjArg0 != 1) {
                MR::deactivateDefaultGameLayout();

                if (mObjArg0 == 0)
                MR::tryScreenToFrameCinemaFrame();
            }
            else
                MR::disableStarPointerShootStarPiece();

            mIsLeftArea = true;
        }
    }

    if (mIsLeftArea) {
        if (!isInVolume(*MR::getPlayerPos())) {
            if (mObjArg0 != 1) {
                MR::activateDefaultGameLayout();

                if (mObjArg0 == 0)
                    MR::tryFrameToScreenCinemaFrame();
            }
            else
                MR::enableStarPointerShootStarPiece();

            mIsLeftArea = false;
        }
    }
}

const char* RestrictArea::getManagerName() const {
    return "ForbidWaterSearch";
}
#endif