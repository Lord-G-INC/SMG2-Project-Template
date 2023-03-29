#include "pt/MapObj/SwitchBox.h"
#include "pt/Util/ActorUtil.h"
#include "JSystem/JGeometry/TVec.h"

SwitchBox::SwitchBox(const char *pName) : LiveActor(pName) {
	initShadows = true;
}

void SwitchBox::init(const JMapInfoIter &rIter) {
	MR::initDefaultPos(this, rIter);
	MR::processInitFunction(this, rIter, false);
	initNerve(&NrvSwitchBox::SwitchBoxNrvWait::sInstance, 0);
	f32 newScale = mScale.x * 90.0f;
	initHitSensor(2);
	TVec3f sensorOffset = TVec3f(0.0f, 0.0f, 0.0f);
	sensorOffset.set(0.0f, newScale, 0.0f);
	MR::addHitSensorMapObj(this, "body", 8, newScale, sensorOffset);

	MR::getJMapInfoArg1NoInit(rIter, &initShadows);

	setNerve(&NrvSwitchBox::SwitchBoxNrvWait::sInstance);
	MR::initCollisionParts(this, "SwitchBox", getSensor("body"), 0);
	MR::validateClipping(this);

	if (initShadows) {
		pt::initShadowVolumeBox(this, TVec3f(150.0f, 150.0f, 150.0f));
		MR::setShadowVolumeStartDropOffset(this, 0, 77.0f);
		MR::setShadowVolumeEndDropOffset(this, 0, 150.0f);
		MR::excludeCalcShadowToMyCollision(this, 0);
		MR::onShadowVolumeCutDropLength(this, 0);
		MR::onCalcGravity(this);
		MR::onCalcShadowOneTime(this, 0);
	}

	MR::addToAttributeGroupSearchTurtle(this);

	if (MR::useStageSwitchReadAppear(this, rIter)) {
		MR::syncStageSwitchAppear(this);
	}

	appear();
}

void SwitchBox::exeWait() {
	if (_8C != 0) {
		--_8C;
	}

	if (_8E != 0) {
		--_8E;
		if (_8E == 0) {
			_90 = 1;
		}
	}

	if (_90 == 0) {
		setNerve(&NrvSwitchBox::SwitchBoxNrvHit::sInstance);
	}
}

void SwitchBox::exeHit() {
	if (MR::isFirstStep(this))
		getSensor("body")->invalidate();

	if (MR::isStep(this, 5))
		MR::hideModel(this);

	if (MR::isStep(this, 15))
		kill();
}

void SwitchBox::appear() {
	MR::offSwitchDead(this);

	_90 = 1;
	_8E = 0;
	_8C = 0;

	LiveActor::appear();
	MR::showModel(this);
	setNerve(&NrvSwitchBox::SwitchBoxNrvWait::sInstance);
}

void SwitchBox::kill() {
	MR::onSwitchDead(this);
	LiveActor::kill();
}

void SwitchBox::doHit() {
	if (_90 != 0)
		--_90;

	if (_90 == 0)
		MR::invalidateCollisionParts(this);

	_8E = 30;

	if (MR::isInWater(this, TVec3f(0.0f, 0.0f, 0.0f))) {
		MR::startSound(this, "SE_OJ_S_BLOCK_BREAK_W", -1, -1);
		MR::emitEffect(this, "BreakWater");
	} else {
		MR::startSound(this, "SE_OJ_S_BLOCK_BREAK", -1, -1);
		MR::emitEffect(this, "Break");
	}
}

bool SwitchBox::receiveMsgPlayerAttack(u32 msg, HitSensor *pSender, HitSensor *pReceiver) {
	if (MR::isMsgPlayerUpperPunch(msg) && _90 != 0) {
		doHit();
		pSender->receiveMessage(ACTMES_REFLECT_V, pReceiver);
		return true;
	}

	if (MR::isMsgPlayerHitAll(msg) && _90 != 0) {
		doHit();
		return true;
	}

	if (MR::isMsgPlayerHipDrop(msg) || MR::isMsgPlayerHipDropFloor(msg)) {
		if (_8C != 0) {
			doHit();
			return true;
		}

		return true;
	}

	return false;
}

bool SwitchBox::receiveMsgEnemyAttack(u32 msg, HitSensor *pSender, HitSensor *pReceiver) {
	if (_90 != 0) {
		doHit();
		return true;
	}

	return false;
}

bool SwitchBox::receiveOtherMsg(u32 msg, HitSensor *pSender, HitSensor *pReceiver) {
	//if (msg == 0x1B) message 0x1B in smg2 is what?
	//	return _90;

	if (MR::isMsgPlayerUpperPunch(msg))
		return _90;

	if (MR::isMsgBallDashWall(msg))
		return !_90;

	if (MR::isMsgFloorTouch(msg)) {
		_8C = 2;
	}

	return false;
}

namespace NrvSwitchBox {
	void SwitchBoxNrvHit::execute(Spine *pSpine) const {
		((SwitchBox*)pSpine->mExecutor)->exeHit();
	}

	void SwitchBoxNrvWait::execute(Spine *pSpine) const {
		((SwitchBox*)pSpine->mExecutor)->exeWait();
	}

	SwitchBoxNrvHit(SwitchBoxNrvHit::sInstance);
	SwitchBoxNrvWait(SwitchBoxNrvWait::sInstance);
};