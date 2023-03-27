#include "pt/MapObj/SwitchBox.h"
#include "Game/Util.h"

/*
 Attempted decomp of SwitchBox from SMG1 Korean
*/

SwitchBox::SwitchBox(const char* pName) : LiveActor(pName) {}

void SwitchBox::init(const JMapInfoIter& rIter) {
	// float stuff here
	MR::initDefaultPos(this, rIter);
	initModelManagerWithAnm("SwitchBox", 0, 0, false);
	MR::connectToSceneMapObjNoCalcAnim(this);
	initSound(2, "SwitchBox", &mTranslation, TVec3f(0.0f, 0.0f, 0.0f));
	initNerve(&NrvSwitchBox::SwitchBoxNrvWait::sInstance, 0);
	initEffectKeeper(3, 0, false);
	initHitSensor(1);
	MR::addHitSensorMapObj(this, "body", 8, 0.0f, mTranslation);

	// checks for... something
	// also gets obj arg1

	setNerve(&NrvSwitchBox::SwitchBoxNrvWait::sInstance);
	MR::initCollisionParts(this, "SwitchBox", getSensor("body"), 0);
	MR::validateClipping(this);

	// more checks
	// Shadow Volume box is removed in smg2

	MR::setShadowVolumeStartDropOffset(this, 0, 77.0f);
	MR::setShadowVolumeEndDropOffset(this, 0, 150.0f);
	MR::onShadowVolumeCutDropLength(this, 0);
	MR::excludeCalcShadowToMyCollision(this, 0);
	MR::onCalcGravity(this);
	MR::onCalcShadowOneTime(this, 0);
	
	MR::addToAttributeGroupSearchTurtle(this);

	if (MR::useStageSwitchReadAppear(this, rIter))
		MR::useStageSwitchSyncAppear(this, rIter);

	MR::needStageSwitchWriteDead(this, rIter);

	// more float stuff
}

// initAfterPlacement ignored- it is a BLR and nothing more

void SwitchBox::exeWait() {
	// what are we comparing???
	setNerve(&NrvSwitchBox::SwitchBoxNrvHit::sInstance);
}

// Done
void SwitchBox::exeHit() {
	if (getNerveStep() == 0)
		getSensor("body")->invalidate();

	if (getNerveStep() == 5)
		MR::hideModel(this);

	if (getNerveStep() == 15)
		appear();
}

void SwitchBox::appear() {
	MR::offSwitchDead(this);
	// stuff here
	LiveActor::appear();
	MR::showModel(this);
	setNerve(&NrvSwitchBox::SwitchBoxNrvWait::sInstance);
}

// Done
void SwitchBox::kill() {
	MR::onSwitchDead(this);
	LiveActor::kill();
}

void SwitchBox::doHit(HitSensor* sensor1, HitSensor* sensor2) {
	// idk
	// compares 0x90
	MR::invalidateCollisionParts(this);

	// not mTranslation
	if (MR::isInWater(this, mTranslation)) {
		MR::startSound(this, "SE_OJ_S_BLOCK_BREAK_W", -1, -1);
		MR::emitEffect(this, "BreakWater");
	}
	else {
		MR::startSound(this, "SE_OJ_S_BLOCK_BREAK", -1, -1);
		MR::emitEffect(this, "Break");
	}
}


bool receiveMsgPlayerAttack(u32 msg, HitSensor* pSender, HitSensor* pReceiver) {
	return 0;
}


bool receiveMsgEnemyAttack(u32 msg, HitSensor* pSender, HitSensor* pReceiver) {
	return 0;
}

bool receiveOtherMsg(u32 msg, HitSensor* pSender, HitSensor* pReceiver) {
	return 0;
}

namespace NrvSwitchBox {
	void SwitchBoxNrvWait::execute(Spine* pSpine) const {
		((SwitchBox*)pSpine->mExecutor)->exeWait();
	}

	void SwitchBoxNrvHit::execute(Spine* pSpine) const {
		((SwitchBox*)pSpine->mExecutor)->exeHit();
	}

	SwitchBoxNrvWait(SwitchBoxNrvWait::sInstance);
	SwitchBoxNrvHit(SwitchBoxNrvHit::sInstance);
}
