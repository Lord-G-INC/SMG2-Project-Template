#ifdef WIP
#include "pt/MapObj/LavaSteam.h"

LavaSteam::LavaSteam(const char* pName) : LiveActor(pName) {
    _90 = TVec3f(0.0f, 1.0f, 0.0f);
    _9C = TVec3f(1.0f, 1.0f, 1.0f);
}

void LavaSteam::init(const JMapInfoIter& rIter) {
    MR::initDefaultPos(this, rIter);
    initModelManagerWithAnm("LavaSteam", 0, 0, false);
    initEffectKeeper(0, 0, false);
    MR::setEffectHostSRT(this, "Sign", &mTranslation, &mRotation, &_9C);
    initHitSensor(1);
    MR::addHitSensorMapObj(this, "body", 8, 250.0f, TVec3f(0.0f, 0.0f, 0.0f));
    MR::setClippingTypeSphere(this, 250.0f, &mTranslation);
    MR::setGroupClipping(this, rIter, 0x10);
    initNerve(&NrvLavaSteam::HostTypeWait::sInstance, 0);
    
    if (MR::useStageSwitchReadA(this, rIter)) {
        setNerve(&NrvLavaSteam::HostTypeWaitForSwitchOn::sInstance);
        MR::listenStageSwitchOnA(this, MR::Functor(this, &startSteam));
    }
    else if (MR::tryRegisterDemoCast(this, rIter)) {
        setNerve(&NrvLavaSteam::HostTypeWaitForSwitchOn::sInstance);
        MR::registerDemoActionFunctor(this, MR::Functor(this, &startSteam), 0);
    }

    MR::useStageSwitchReadParam(this, rIter);
    initSound(4, 0, &mTranslation, TVec3f(0.0f, 0.0f, 0.0f));
    MR::connectToSceneNoSilhouettedMapObj(this);
}

void LavaSteam::initAfterPlacement() {
}

void LavaSteam::attackSensor(HitSensor* pSender, HitSensor* pReceiver) {
    f32 float_31;
    
    if (!isNerve(&NrvLavaSteam::HostTypeWait::sInstance) && !isNerve(&NrvLavaSteam::HostTypeWaitForSwitchOn::sInstance)) {
        if (MR::isSensorPlayerOrRide(pReceiver)) {
            float_31 = 0.69999999*pReceiver->mRadius;

            if (isNerve(&NrvLavaSteam::HostTypeSteam::sInstance)){
                _90*400.0f;
                TVec3f test = mTranslation;
                __asm {
                    psq_l     f1, 0x30(r1), 0, 0
                    psq_l     f0, 0(r4), 0, 0
                    psq_l     f2, 8(r4), 1, 0
                    ps_add    f0, f0, f1
                    psq_l     f3, 0x38(r1), 1, 0
                    ps_add    f1, f2, f3
                    psq_st    f0, 0(r4), 0, 0
                    psq_st    f1, 8(r4), 1, 0
                }
            }
        }
    }
}

void LavaSteam::startClipped() {
    LiveActor::startClipped();
    MR::deleteEffectAll(this);
}

void LavaSteam::endClipped() {
    LiveActor::endClipped();
    isNerve(&NrvLavaSteam::HostTypeWaitForSwitchOn::sInstance);
}

void LavaSteam::startSteam() {
    setNerve(&NrvLavaSteam::HostTypeSteam::sInstance);
}

void LavaSteam::exeWait() {
    if (MR::isFirstStep(this)) {
        MR::invalidateHitSensors(this);
        MR::emitEffect(this, "Sign");
    }

    if (MR::isGreaterStep(this, 82)) {
        _9C.setAll(MR::getEaseInValue((f32)getNerveStep(), 0.001f, 1.0f, 1.0f));
    }

    if (MR::isGreaterStep(this, 90))
        MR::forceDeleteEffect(this, "Sign");

    if (MR::isGreaterStep(this, 120))
        setNerve(&NrvLavaSteam::HostTypeSteam::sInstance);
}

void LavaSteam::exeSteam() {
    if (MR::isFirstStep(this)) {
        MR::emitEffect(this, "Steam");
        MR::validateHitSensors(this);
    }
    
    MR::startActionSound(this, "SE_OJ_LV_LAVA_STEAM_OUT", -1, -1, -1);

    if (MR::isStep(this, 90)) {
        MR::deleteEffect(this, "Steam");
        setNerve(&NrvLavaSteam::HostTypeSteamEnd::sInstance);
    }
}

namespace NrvLavaSteam {
    void HostTypeSteamEnd::execute(Spine* pSpine) const {
        LavaSteam* pActor = (LavaSteam*)pSpine->mExecutor;

        if (MR::isStep(pActor, 0x5A))
            pActor->setNerve(&NrvLavaSteam::HostTypeWait::sInstance);
    }

    void HostTypeSteam::execute(Spine* pSpine) const {
        ((LavaSteam*)pSpine->mExecutor)->exeSteam();
    }

    void HostTypeWaitForSwitchOn::execute(Spine* pSpine) const {}

    void HostTypeWait::execute(Spine* pSpine) const {
        ((LavaSteam*)pSpine->mExecutor)->exeWait();
    }

    HostTypeSteamEnd(HostTypeSteamEnd::sInstance);
    HostTypeSteam(HostTypeSteam::sInstance);
    HostTypeWait(HostTypeWait::sInstance);
    HostTypeWaitForSwitchOn(HostTypeWaitForSwitchOn::sInstance);
}
#endif