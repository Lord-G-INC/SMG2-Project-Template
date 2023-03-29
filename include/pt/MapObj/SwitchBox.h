#pragma once 

#include "syati.h"

class SwitchBox : public LiveActor {
public:
	SwitchBox(const char* pName);
	virtual void init(const JMapInfoIter& rIter);
	void exeWait();
	void exeHit();
	virtual void appear();
	virtual void kill();
	void doHit(HitSensor* sensor1, HitSensor* sensor2);
    virtual bool receiveMsgPlayerAttack(u32 msg, HitSensor* pSender, HitSensor* pReceiver);
    virtual bool receiveMsgEnemyAttack(u32 msg, HitSensor* pSender, HitSensor* pReceiver);
    virtual bool receiveOtherMsg(u32 msg, HitSensor* pSender, HitSensor* pReceiver);
};

namespace NrvSwitchBox {
	NERVE(SwitchBoxNrvWait);
	NERVE(SwitchBoxNrvHit);
};
