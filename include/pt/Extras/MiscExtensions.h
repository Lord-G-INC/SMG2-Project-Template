#pragma once

#include <syati.h>

class JUTHolder {
    public:
    JUTTexture* Textures[4]{};
    void SetTexture(u8 pos, JUTTexture* texture) {
        // Get Address of pointers, delete old one if adresses dont match, otherwise do nothing.
        u64 ladr = (u64)Textures[pos];
        u64 radr = (u64)texture;
        if (ladr != radr) {
            delete Textures[pos];
            Textures[pos] = texture;
        }
    }
    JUTTexture* operator[](u8 pos) {
        return Textures[pos];
    }
};