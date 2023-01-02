#include "pt/Map/CometTimerObj.h"

CometTimerObj::CometTimerObj(const char* pName) : LiveActor(pName) {
    mTime = 60;
    mKillPlayer = false;
    mIsAppeared = false;
}

void CometTimerObj::init(const JMapInfoIter& rIter) {
    MR::initDefaultPos(this, rIter);
    MR::connectToSceneMapObjMovement(this);
    MR::getJMapInfoArg0NoInit(rIter, &mTime);
    MR::getJMapInfoArg1NoInit(rIter, &mKillPlayer);

    OSReport("init\n");

    MR::useStageSwitchReadA(this, rIter);
    MR::useStageSwitchWriteB(this, rIter);
    
    mLayout = new TimeLimitLayout(1);
    mLayout->setDisplayModeOnNormal(true);
    MR::connectToSceneLayout(mLayout);
    mLayout->initWithoutIter();
}

void CometTimerObj::movement() {
    if (MR::isValidSwitchA(this))
        if (MR::isOnSwitchA(this) && !mIsAppeared) {
            if (MR::isHiddenLayout(mLayout))
                MR::showLayout(mLayout);
                
            mLayout->setTimeLimit(mTime * 60);
            mLayout->appear();
            mIsAppeared = true;
        }
    
    if (mLayout->isReadyToTimeUp() && mIsAppeared) {
        mIsAppeared = false;
        MR::hideLayout(mLayout);
        mKillPlayer ? MR::forceKillPlayerByGroundRace() : hideLayoutAndSwitchOn();
    }
}

void CometTimerObj::hideLayoutAndSwitchOn() {
    MR::startSystemSE("SE_SY_TIMER_A_0", -1, -1);
    if (MR::isValidSwitchB(this))
        MR::onSwitchB(this);

    if (MR::isValidSwitchA(this))
        MR::offSwitchA(this);
}