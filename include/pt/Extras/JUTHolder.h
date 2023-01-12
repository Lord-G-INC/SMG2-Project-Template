#pragma once

#include <syati.h>
#define arrsize(item) sizeof(item)/sizeof(item[0])

class JUTHolder {
    public:
    JUTTexture* Textures[16];
    void SetTexture(u8 pos, JUTTexture* texture);
    JUTTexture* operator[](u8 pos);
    JUTTexture* getTexture(u8 pos);
    JUTHolder(s32 size);
    ~JUTHolder();

    s32 mSize;
};