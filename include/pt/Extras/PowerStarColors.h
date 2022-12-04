#pragma once

#include "syati.h"

#if defined (ALL) || defined (NOGLE)
namespace pt {
	s32 getPowerStarColor(const char *pStage, s32 scenarioId);
	s32 getPowerStarColorCurrentStage(s32 scenarioId);
};
#endif
