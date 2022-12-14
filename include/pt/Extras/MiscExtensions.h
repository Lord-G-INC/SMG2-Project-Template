#pragma once

#include <syati.h>

const char* ColorsStr[] = {"Red.bti", "Blue.bti", "Rainbow.bti", "Purple.bti"};

const int TexSize = sizeof(ColorsStr)/sizeof(const char*);

class JUTHolder {
    public:
    JUTTexture* Textures[TexSize];
    void SetTexture(u8 pos, JUTTexture* texture) {
        // Get Address of pointers, delete old one if adresses dont match, otherwise do nothing.
        u64 ladr = (u64)Textures[pos];
        u64 radr = (u64)texture;
        if (ladr != radr) {
            if (ladr != NULL) {
                OSReport("Texture %d has been deleted.\n", pos);
                delete Textures[pos];
            } else {
                OSReport("Texture %d now has a value.\n", pos);
            }
            Textures[pos] = texture;
        } else {
            OSReport("Address did not change, not changing Texture %d\n", pos);
            delete texture;
        }
    }
    JUTTexture* operator[](u8 pos) {
        return Textures[pos];
    }
    JUTHolder() {
        OSReport("TexSize: %d\n", TexSize);
        for (int i = 0; i < TexSize; i++) {
            Textures[i] = 0;
        }
    }
    ~JUTHolder() {
        for (int i = 0; i < TexSize; i++) {
            delete Textures[i];
        }
    }
};