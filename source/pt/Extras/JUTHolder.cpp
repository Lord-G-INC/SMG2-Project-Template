#include "pt/Extras/JUTHolder.h"

template <s32 SIZE>
JUTHolder<SIZE>::JUTHolder() {
    
    OSReport("Size: %d\n", mSize);
    for (int i = 0; i < mSize; i++) {
        Textures[i] = 0;
    }
}

template <s32 SIZE>
JUTHolder<SIZE>::~JUTHolder() {
    for (int i = 0; i < mSize; i++) {
        delete Textures[i];
    }
}

template <s32 SIZE>
void JUTHolder<SIZE>::SetTexture(u8 pos, JUTTexture* texture) {
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

template <s32 SIZE>
JUTTexture* JUTHolder<SIZE>::operator[](u8 pos) {
    return Textures[pos];
}

template <s32 SIZE>
JUTTexture* JUTHolder<SIZE>::getTexture(u8 pos) {
    return Textures[pos];
}