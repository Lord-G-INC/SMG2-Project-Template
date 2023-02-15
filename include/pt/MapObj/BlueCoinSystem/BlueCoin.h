#pragma once
#include "syati.h"
#include "pt/MapObj/BlueCoinSystem/BlueCoinManager.h"

class BlueCoin : public Coin {
    BlueCoinInstance* mInstance;
    public:
    u8 mID;
    BlueCoin(const char*);
    virtual bool receiveMessage(u32, HitSensor *, HitSensor *);
    virtual void init(const JMapInfoIter&);
};