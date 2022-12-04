#pragma once

#include "syati.h"
#include "Game/Screen/GameSceneLayoutHolder.h"

#if defined (ALL) || defined (SMG63)
class RestrictGameLayoutArea : public AreaObj {
public:
    RestrictGameLayoutArea(const char* pName);
    virtual void init(const JMapInfoIter& rIter);
    virtual void movement();
    virtual const char* getManagerName() const;
};
#endif