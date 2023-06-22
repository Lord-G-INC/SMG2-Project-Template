#ifdef SMG63
#pragma once

#include "syati.h"
#include "Game/Screen/GameSceneLayoutHolder.h"

class RestrictArea : public AreaObj {
public:
    RestrictArea(const char* pName);
    virtual void init(const JMapInfoIter& rIter);
    virtual void movement();
    virtual const char* getManagerName() const;

    bool mIsLeftArea;
};
#endif