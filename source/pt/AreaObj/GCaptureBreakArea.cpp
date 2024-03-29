#include "Game/MapObj/GCapture.h"
#include "pt/AreaObj/GCaptureBreakArea.h"
#include "Game/Util.h"
#include "kamek/hooks.h"
GCapture *currentGC;
void storeCurrentGCapture (GCapture *gc, HitSensor *dunno) {
    currentGC = gc;
    gc->requestBind(dunno);
}
kmCall(0x802A5B68, storeCurrentGCapture);

namespace pt {
    GCaptureBreakArea::GCaptureBreakArea (const char *pName) : AreaObj(pName) {
        mKeepPlayerMomentum = false;
    }
    GCaptureBreakArea::~GCaptureBreakArea () {}
    void GCaptureBreakArea::init(const JMapInfoIter& rIter) {
    	AreaObj::init(rIter);
        MR::getJMapInfoArg0NoInit(rIter, &mKeepPlayerMomentum);
    	MR::connectToSceneAreaObj(this);
    }
    void GCaptureBreakArea::movement () {
        if (isInVolume(*MR::getPlayerPos()) && currentGC) {
            if (!mKeepPlayerMomentum) 
                currentGC->requestCancelBind();
            currentGC->exeBreak();
        }
    }
    const char *GCaptureBreakArea::getManagerName() const {
    	return "GCaptureBreakArea";
    }
}