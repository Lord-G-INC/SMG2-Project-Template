#include "pt/MapObj/BlueCoinSystem/BlueCoin.h"

void BlueCoin::init(const JMapInfoIter& rIter) {
    MR::getJMapInfoArg0NoInit(rIter, (s32*)&mID);
    mInstance = gBlueCoinMainager[mID];
}

bool BlueCoin::receiveMessage(u32 msg, HitSensor* pSender, HitSensor* pReciver) {
    if (MR::isMsgItemGet(msg)) {
        mInstance->mIsCollected = true;
        // TODO: Make the coin clear instead, like a star.
        MR::hideModel(this);
    }
}