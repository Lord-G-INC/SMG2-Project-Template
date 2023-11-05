#if defined USA || defined PAL || defined JPN
#include "Game/MapObj/CollapsePlane.h"
    
namespace pt {
    kmWrite32(0x8033EF04, 0x386000E8); // li r3, 0xE8

	void CollapsePlaneInit(CollapsePlane* pActor, const JMapInfoIter& rIter, HitSensor* pSensor, MtxPtr pMtx) {
		pActor->mRespawnTime = -1;
		MR::getJMapInfoArg1NoInit(rIter, &pActor->mRespawnTime);
		MR::useStageSwitchReadA(pActor, rIter);
		OSReport("%d\n", pActor->mRespawnTime);
		
		MR::initCollisionPartsAutoEqualScale(pActor, "Move", pSensor, pMtx);
	}

	kmWrite32(0x802902BC, 0x7FA4EB78); // mr r4, r29
	kmCall(0x802902C0, CollapsePlaneInit);

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