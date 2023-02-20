#pragma once
#include "syati.h"

extern bool** gBlueCoinData;

namespace BlueCoinUtil {
    void GetData();
    void SaveData();
    s32 getCurrentFileNum();
    void setBlueCoinGotOnCurrentFile(u8 id, bool collected);
    bool isBlueCoinGot(u8 file, u8 id);
    void resetAllBlueCoin(u8 file);
}