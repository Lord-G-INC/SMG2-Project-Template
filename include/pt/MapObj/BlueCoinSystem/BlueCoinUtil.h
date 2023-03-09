#if defined (ALL) || defined (SMSS)
#pragma once
#include "syati.h"

extern bool** gBlueCoinData; // the one the only

namespace BlueCoinUtil {
    void LoadBlueCoinData();
    void SaveBlueCoinData();
    void initBlueCoinArray();
    s32 getCurrentFileNum();
    void setBlueCoinGotOnCurrentFile(u8 id, bool collected);
    bool isBlueCoinGot(u8 file, u8 id);
    bool isBlueCoinGotCurrentFile(u8 id);
    bool isBlueCoinGot240(u8 fileID);
    void resetAllBlueCoin(u8 file);
    void resetAllBlueCoinAllFileNoSave();
    s32 getTotalBlueCoinNum(u8 file);
    s32 getTotalBlueCoinNumCurrentFile();
    s32 getTotalBlueCoinRangeNumFromBcsv(const char* pStageName);
    void showAmountPaneFromBcsv(LayoutActor* lyt, const char* pStageName);
}
#endif