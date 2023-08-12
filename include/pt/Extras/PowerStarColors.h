
#pragma once

#include "syati.h"

namespace pt {
	#ifdef NOGLE
	s32 getPowerStarColor(const char *pStage, s32 scenarioId);
	s32 getPowerStarColorCurrentStage(s32 scenarioId);
	#endif
	void TamakoroCustomPowerStarColorsParticles(LiveActor* pActor);
};