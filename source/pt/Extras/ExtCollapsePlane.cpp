#if defined USA || defined PAL || defined JPN
#include "Game/MapObj/CollapsePlane.h"
    
namespace pt {
    kmWrite32(0x8033EF04, 0x386000E8); // li r3, 0xE8

	void CollapsePlaneInit(CollapsePlane* pActor, const JMapInfoIter& rIter, MapObjActorInitInfo& rInfo) {
		pActor->mRespawnTime = -1;
		MR::getJMapInfoArg1NoInit(rIter, &pActor->mRespawnTime);
		MR::useStageSwitchReadA(pActor, rIter);

        pActor->initialize(rIter, rInfo);
	}

	kmCall(0x8029016C, CollapsePlaneInit);

    void CollapsePlaneDecideNerve(CollapsePlane* pActor, const Nerve* pNerve) {
        if (pActor->mRespawnTime > -1)
            pActor->setNerve(&NewNrvCollapsePlane::NrvReturn::sInstance);
        else
            pActor->setNerve(pNerve);
    }

    kmCall(0x802903D8, CollapsePlaneDecideNerve);
}

void CollapsePlane::exeReturn() {
	if (MR::isStep(this, mRespawnTime)) {
        MR::showMaterial(this, "PlaneMat_v");
        MR::validateCollisionParts(this);
        setNerve(&NrvCollapsePlane::CollapsePlaneNrvWait::sInstance);
    }
};

namespace NewNrvCollapsePlane {
	void NrvReturn::execute(Spine* pSpine) const {
        ((CollapsePlane*)pSpine->mExecutor)->exeReturn();
    };

	NrvReturn(NrvReturn::sInstance);
}
#endif