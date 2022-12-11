#include "pt/MapObj/RedCoinSwitch.h"
#include "pt/Util/ActorUtil.h"

RedCoinSwitch::RedCoinSwitch(const char* pName) : LiveActor(pName) {
    isOn = false;
    mTime = 0;
    mCollectedPowerStar = -1;
}

void RedCoinSwitch::init(const JMapInfoIter& rIter) {
    MR::processInitFunction(this, rIter, false);
    initHitSensor(1);
	MR::addHitSensorMapObj(this, "Switch", 1, 75.0f, TVec3f(0.0f, 150.0f, 0.0f));
    MR::initCollisionParts(this, "RedCoinSwitch", getSensor("Switch"), (MtxPtr)getBaseMtx());
    MR::getJMapInfoArg0NoInit(rIter, &mTime);
    MR::getJMapInfoArg1NoInit(rIter, &mCollectedPowerStar);

    mTimeLimitLayout = new TimeLimitLayout(1);
    MR::connectToSceneLayout(mTimeLimitLayout);
    mTimeLimitLayout->initWithoutIter();
    
    makeActorAppeared();

    if (mCollectedPowerStar > 0 && MR::hasPowerStarInCurrentStage(mCollectedPowerStar) || MR::isValidSwitchAppear(this)) {
        MR::invalidateHitSensors(this);
        MR::hideModel(this);
        MR::invalidateCollisionParts(this);
    }
}

void RedCoinSwitch::exeOn() {
    MR::startAction(this, "Press");
    MR::invalidateCollisionParts(this);
    isOn = true;
    
    mTimeLimitLayout->setTimeLimit(mTime * 60);
    mTimeLimitLayout->appear();

    if (MR::isValidSwitchA(this))
        MR::onSwitchA(this);
}

void RedCoinSwitch::control() {
    if (mTimeLimitLayout->isReadyToTimeUp())
        MR::forceKillPlayerByGroundRace();

    if (MR::isValidSwitchB(this))
        if (MR::isOnSwitchB(this)) 
            mTimeLimitLayout->kill();

    if (MR::isPowerStarGetDemoActive())
        mTimeLimitLayout->kill();
            
    if (MR::isValidSwitchAppear(this) && !isOn)
        if (MR::isOnSwitchAppear(this)) {
            MR::validateHitSensors(this);
            MR::showModel(this);
            MR::validateCollisionParts(this);
        }
}

bool RedCoinSwitch::receiveMessage(u32 msg, HitSensor* pSender, HitSensor* pReceiver) {
	if (MR::isMsgPlayerHipDropFloor(msg) && !isOn)
        exeOn();

		return false;
}