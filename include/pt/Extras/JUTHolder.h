#pragma once

#include <syati.h>
#define arrsize(item) sizeof(item)/sizeof(item[0])

class JUTHolder {
    public:
    const size_t SIZE;
    JUTTexture** Textures;
    void SetTexture(u8 pos, JUTTexture* texture) {
        // Get Address of pointers, delete old one if adresses dont match, otherwise do nothing.
        u64 ladr = (u64)Textures[pos];
        u64 radr = (u64)texture;
        if (ladr != radr) {
            if (ladr != NULL) {
                OSReport("Texture %d has been deleted.\n", pos);
                memset(Textures[pos], 0, sizeof(JUTTexture));
            } else {
                OSReport("Texture %d now has a value.\n", pos);
            }
            memmove(Textures[pos], texture, sizeof(JUTTexture));
            delete texture;
        } else {
            OSReport("Address did not change, not changing Texture %d\n", pos);
        }
    }
    JUTTexture* operator[](u8 pos) {
        return Textures[pos];
    }
    JUTHolder(size_t size) : SIZE(size) {
        OSReport("Size: %d\n", SIZE);
        Textures = new JUTTexture*[size];
        memset(Textures, 0, size*sizeof(JUTTexture*));
    }
    ~JUTHolder() {
        delete [] Textures;
    }
};