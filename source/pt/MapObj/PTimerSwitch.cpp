#include "pt/MapObj/PTimerSwitch.h"

PTimerSwitch::PTimerSwitch(const char* pName) : LiveActor(pName) {
    mCollisionParts = 0;
    mConnector = 0;
    _94 = 0;
    _98 = false;
}

void PTimerSwitch::reset() {
    LiveActor::appear();
    setNerve(&NrvPTimerSwitch::PTimerSwitchNrvOff::sInstance);
    MR::validateCollisionParts(mCollisionParts);
    MR::startBck(this, "Wait", 0);
}

void PTimerSwitch::init(const JMapInfoIter& rIter) {
    OSReport("Init\n");
    MR::initDefaultPos(this, rIter);
    MR::processInitFunction(this, rIter, false);
    MR::connectToSceneMapObj(this);
    initHitSensor(2);
    MR::addHitSensorMapObj(this, "body", 0x10, 0.0f, TVec3f(0.0f, 0.0f, 0.0f));
    MR::addHitSensor(this, "hit", 0x4F, 0x10, 120.0f, TVec3f(0.0f, 0.0f, 50.0f));
    MR::initCollisionParts(this, "PTimerSwitch", getSensor("body"), 0);
    mCollisionParts = MR::createCollisionPartsFromLiveActor(this, "Move", getSensor("hit"), (MR::CollisionScaleType)2);
    initSound(4, "PTimerSwitch", &mTranslation, TVec3f(0.0f, 0.0f, 0.0f));
    MR::needStageSwitchWriteA(this, rIter);
    initNerve(&NrvPTimerSwitch::PTimerSwitchNrvOff::sInstance, 0);
    initEffectKeeper(0, "PTimerSwitch", false);
    MR::startBck(this, "Move", 0);
    mConnector = new MapObjConnector(this);
    appear();
}

void PTimerSwitch::initAfterPlacement() {
    mConnector->attachToUnder();
}

void PTimerSwitch::appear() {
    reset();
}

void PTimerSwitch::kill() {
    LiveActor::kill();
    MR::tryEmitEffect(this, "Delete");
    MR::invalidateCollisionParts(mCollisionParts);
}

void PTimerSwitch::calcAnim() {
	LiveActor::calcAnim();
	TMtx34f mtx;
	PSMTXCopy(MR::getJointMtx(this, "Move"), mtx);
	mCollisionParts->setMtx(mtx);
}

void PTimerSwitch::control() {
    if (_98)
        ++_94;
    else if (_94 == 0)
        --_94;

    _98 = false;
}

void PTimerSwitch::calcAndSetBaseMtx() {
    LiveActor::calcAndSetBaseMtx();
    mConnector->connect();
}

bool PTimerSwitch::receiveOtherMsg(u32 msg, HitSensor* pSender, HitSensor* pReceiver) {
    return false;
}

bool PTimerSwitch::receiveMsgPlayerAttack(u32 msg, HitSensor* pSender, HitSensor* pReceiver) {
    if (msg == 0x5D) {
        if (isNerve(&NrvPTimerSwitch::PTimerSwitchNrvOff::sInstance)) {
            _98 = true;
            return true;
        }
    }
    return false;
}

bool PTimerSwitch::trySwitchDown() {
    if (_94 == 0) {
        setNerve(&NrvPTimerSwitch::PTimerSwitchNrvSwitchDown::sInstance);
        return true;
    }
    return false;
}

bool PTimerSwitch::tryOn() {
    if (MR::isBckStopped(this)) {
        setNerve(&NrvPTimerSwitch::PTimerSwitchNrvOn::sInstance);
        return true;
    }
    return false;
}

void PTimerSwitch::exeOff() {
    if (MR::isFirstStep(this)) {
        MR::validateClipping(this);
        MR::offSwitchA(this);
    }
    trySwitchDown();
}

void PTimerSwitch::exeSwitchDown() {
    if (MR::isFirstStep(this)) {
        MR::startBck(this, "On", 0);
        MR::invalidateClipping(this);
    }
    tryOn();
}

void PTimerSwitch::exeOn() {
    if (MR::isFirstStep(this)) {
        MR::onSwitchA(this);
        MR::shakeCameraNormal();
        MR::startSound(this, "SE_OJ_SWITCH_1", -1, -1);
    }

    if (MR::isGreaterStep(this, 25)) {
        kill();
    }
}

namespace NrvPTimerSwitch {
    void PTimerSwitchNrvOn::execute(Spine* pSpine) const {
        ((PTimerSwitch*)pSpine->mExecutor)->exeOn();
    }

    void PTimerSwitchNrvSwitchDown::execute(Spine* pSpine) const {
        ((PTimerSwitch*)pSpine->mExecutor)->exeSwitchDown();
    }

    void PTimerSwitchNrvOff::execute(Spine* pSpine) const {
        ((PTimerSwitch*)pSpine->mExecutor)->exeOff();
    }

    PTimerSwitchNrvOn(PTimerSwitchNrvOn::sInstance);
    PTimerSwitchNrvSwitchDown(PTimerSwitchNrvSwitchDown::sInstance);
    PTimerSwitchNrvOff(PTimerSwitchNrvOff::sInstance);
}