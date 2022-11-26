#include "pt/AreaObj/RestrictGameLayoutArea.h"

RestrictGameLayoutArea::RestrictGameLayoutArea(const char* pName) : AreaObj(pName) {
}

void RestrictGameLayoutArea::init(const JMapInfoIter& rIter) {
    AreaObj::init(rIter);
	MR::connectToSceneAreaObj(this);
    OSReport("init\n");
}

void RestrictGameLayoutArea::movement() {

    if (isInVolume(*MR::getPlayerPos())) {
        MR::deactivateDefaultGameLayout();

    if (mObjArg0 > -1)
        MR::tryScreenToFrameCinemaFrame();
    }
    else {
        if (!MR::isActiveDefaultGameLayout())
            MR::activateDefaultGameLayout();
    
        if (mObjArg0 > -1 && !MR::isActiveDefaultGameLayout())
            MR::tryFrameToScreenCinemaFrame();
    }
}

const char* RestrictGameLayoutArea::getManagerName() const {
    return "ForbidWaterSearch";
}