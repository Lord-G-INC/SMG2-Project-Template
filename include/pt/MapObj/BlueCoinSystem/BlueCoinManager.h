#pragma once
#include "syati.h"

extern bool** gBlueCoinData;

namespace BlueCoinUtil {
    void GetData();
    void SaveData();
    s32 getCurrentFileNum();
    void setBlueCoinGotOnCurrentFile(u8 id, bool collected);
    bool isBlueCoinGot(u8 file, u8 id);
    bool isBlueCoinGotCurrentFile(u8 id);
    void resetAllBlueCoin(u8 file);
    void resetAllBlueCoinAllFileNoSave();
    s32 getTotalBlueCoinNum(u8 file);
    s32 getTotalBlueCoinNumCurrentFile();
    s32 getTotalBlueCoinRangeNumFromBcsv(const char* pStageName);
    void calcBlueCoinTexFromBcsv(LayoutActor* lyt, const char* pStageName);
}