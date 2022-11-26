#pragma once

#include "syati.h"
#include "Game/System/Misc/GameSceneLayoutHolder.h"

class RestrictGameLayoutArea : public AreaObj {
public:
    RestrictGameLayoutArea(const char* pName);
    virtual void init(const JMapInfoIter& rIter);
    virtual void movement();
    virtual const char* getManagerName() const;
};