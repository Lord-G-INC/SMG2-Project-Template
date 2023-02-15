#pragma once

#include "syati.h"

class ActorCameraInfo;

namespace pt {
	class PowerStarSpawner : public LiveActor {
	public:
		PowerStarSpawner(const char *);

		virtual void init(const JMapInfoIter& rIter);
		virtual void movement();

		void spawnStar();
		bool spawnCondition();
		void createDisplayStar();

		ActorCameraInfo* mCamInfo;
		s32 mScenario; // Obj_arg0
		s32 mSpawnMode; // Obj_arg1
		s32 mDelay; // Obj_arg2
		bool mUseSE; // Obj_arg3
		bool mFromMario; // Obj_arg4
		s32 mElapsed;
		Mtx DisplayStarMtx;
		s32 mSpawnCondition; // Obj_arg7
    };
};