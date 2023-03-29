#pragma once

#include "syati.h"

class SwitchBox : public LiveActor {
public:
	SwitchBox(const char *pName);

	virtual void init(const JMapInfoIter &rIter);
	virtual void appear();
	virtual void kill();
	virtual bool receiveMsgPlayerAttack(u32 msg, HitSensor *pSender, HitSensor *pReceiver);
	virtual bool receiveMsgEnemyAttack(u32 msg, HitSensor *pSender, HitSensor *pReceiver);
	virtual bool receiveOtherMsg(u32 msg, HitSensor *pSender, HitSensor *pReceiver);

	void exeWait();
	void exeHit();
	void doHit();

	u16 _8C;
	u16 _8E;
	u16 _90;
	bool initShadows;
};

namespace NrvSwitchBox {
	NERVE(SwitchBoxNrvWait);
	NERVE(SwitchBoxNrvHit);
};