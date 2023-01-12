#pragma once

#include <syati.h>
#define arrsize(item) sizeof(item)/sizeof(item[0])

template <s32 SIZE>
class JUTHolder {
    public:
    JUTTexture* Textures[SIZE];
    void SetTexture(u8 pos, JUTTexture* texture);
    JUTTexture* operator[](u8 pos);
    JUTTexture* getTexture(u8 pos);
    JUTHolder();
    ~JUTHolder();

    s32 mSize;
};