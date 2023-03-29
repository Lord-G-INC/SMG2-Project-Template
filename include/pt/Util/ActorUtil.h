#pragma once

#include "syati.h"
#include "Game/MapObj/CoinHolder.h"

class Coin;

namespace pt {
	void moveAndTurnToPlayer(LiveActor *pActor, const MR::ActorMoveParam &rParam);
	void moveAndTurnToDirection(LiveActor *pActor, const TVec3f &rDir, const MR::ActorMoveParam &rParam);
	void turnToDirectionUpFront(LiveActor *pActor, TVec3f rUp, TVec3f rFront);
	void turnToDirectionGravityFront(LiveActor *pActor, TVec3f rFront);

	void initRailToNearestAndRepositionWithGravity(LiveActor *pActor);
	void getTransRotateFromRailWithGravity(LiveActor *pActor);

	AnimScaleController* createSamboAnimScaleController(AnimScaleParam *pAnimScaleParam);

	void* loadArcAndFile(const char *pArc, const char *pFile);
	//void* loadFile(const char* pFile);

	void initShadowVolumeBox(LiveActor* pActor, const TVec3f& rPos);
};
