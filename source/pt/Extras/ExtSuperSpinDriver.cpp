#include "pt/Extras/ExtSuperSpinDriver.h"
#include "Game/MapObj/SuperSpinDriver.h"
#include "Game/Util/ActorAnimUtil.h"
#include "JSystem/JUtility/JUTTexture.h"
#include "pt/Extras/JUTTextureHolder.h"

/*
* Author: Aurum
*/

namespace pt {
	namespace {
	/*
	* Green + Custom Launch Stars
	*
	* Unsurprisingly, all the BTP and BRK frames for the green Launch Star is still found inside SuperSpinDriver.arc. Here,
	* we hijack calls to initColor in order to check for the green color first. If the Launch Star's color is set to green,
	* we apply its animation frames. Otherwise, we call initColor to set up the other colors.
	*
	* This code also allows new BTP frames and SpinDriverPath textures to be loaded and used.
	* Support for new BRK frames may be added in the future.
	*/

	const char* ColorsStr[] = {"Red.bti", "Blue.bti", "Rainbow.bti", "Purple.bti", "White.bti", "White.bti", "Yellow.bti"};

	JUTTextureHolder Colors = arrsize(ColorsStr);

	void initSuperSpinDriverCustomColor(SuperSpinDriver *pActor) {
		s32 color = pActor->mColor;

		if (color != 0 && color != 2) {
			MR::startBtpAndSetFrameAndStop(pActor, "SuperSpinDriver", color);
			MR::startBrk(pActor, color == 1 ? "Green" : "Red");

			pActor->mSpinDriverPathDrawer->mColor = color == 1 ? 0 : color;
		} else
			pActor->initColor();
			
        if (color >= 3)
            Colors.SetTexture(color - 3, new JUTTexture(MR::loadTexFromArc("SpinDriverPath.arc", ColorsStr[color - 3], 0), 0));
	}

	kmCall(0x8031E29C, initSuperSpinDriverCustomColor); // redirect initColor in init

	void setSpinDriverPathCustomColor(SpinDriverPathDrawer* pDrawer) {
		if (pDrawer->mColor >= 3)
			Colors[pDrawer->mColor - 3]->load(GX_TEXMAP0);

		pDrawer->calcDrawCode(); // Restore original call
	}

	kmCall(0x8030EF28, setSpinDriverPathCustomColor);
	}
}
