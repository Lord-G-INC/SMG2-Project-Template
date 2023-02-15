#include "pt/MapObj/BlueCoinSystem/BlueCoin.h"

void BlueCoin::init(const JMapInfoIter& rIter) {
    MR::getJMapInfoArg0NoInit(rIter, (s32*)&mID);
    mInstance = gBlueCoinMainager[mID];
}