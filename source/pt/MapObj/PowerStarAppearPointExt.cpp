#include "pt/MapObj/PowerStarAppearPointExt.h"
#include "Game/Util.h"
#include "Game/Player/MarioAccess.h"

/*
* PowerStarAppearPoint Extensions
* This actor is exclusive to PT Debug.
*
* Created by Evanbowl
*
* A reimplementation of the PowerStarAppearPoint.
* Adds the ability to spawn a Power Star above the player's position.
*/
namespace pt {

PowerStarAppearPointExt::PowerStarAppearPointExt(const char* pName) : LiveActor(pName) {
    mSpawnAtPlayer = false;
    mYOffset = 275;
}

void PowerStarAppearPointExt::init(const JMapInfoIter& rIter) {
    MR::initDefaultPos(this, rIter);
    MR::connectToSceneMapObjMovement(this);
    MR::invalidateClipping(this);

    MR::joinToGroupArray(this, rIter, "パワースター出現ポイントグループ", 16);
    mCamInfo = new ActorCameraInfo(rIter);
    MR::initActorCamera(this, rIter, &mCamInfo);

    MR::getJMapInfoArg0NoInit(rIter, &mSpawnAtPlayer);
    MR::getJMapInfoArg1NoInit(rIter, &mYOffset);
}

void PowerStarAppearPointExt::movement() {
    if (mSpawnAtPlayer) {
        MR::setPosition(this, *MR::getPlayerPos());
        JMAVECScaleAdd((Vec*)MarioAccess::getPlayerActor()->getGravityVec(), (Vec*)&mTranslation, (Vec*)&mTranslation, -mYOffset);
    }
}

/*
* PowerStarAppearPointExt Creation
* Here we redirect the PowerStarAppearPoint's creation to the reimplemented version of the object.
*/
NameObj* createPowerStarAppearPointExt(const char* pName) {
    return new PowerStarAppearPointExt(pName);
}

kmCall(0x8033FB98, createPowerStarAppearPointExt);

}