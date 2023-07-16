#if defined USEBLUECOIN && !defined SM64BLUECOIN
#pragma once
#include "syati.h"

extern bool** gBlueCoinData;

namespace BlueCoinUtil {
    // Loads BlueCoinData.bin into gBlueCoinData bool array.
    void loadBlueCoinData();
    // Saves gBlueCoinData bool array into BlueCoinData.bin.
    void saveBlueCoinData();
    // Initalizes gBlueCoinData bool array.
    void initBlueCoinArray();
    // Gets the current save file number being played.
    s32 getCurrentFileNum();
    // Sets a blue coin to true on the current save file.
    void setBlueCoinGotCurrentFile(u8 id);
    // Checks if a specific blue coin is true.
    bool isBlueCoinGot(u8 file, u8 id);
    // Checks if a specific blue coin is true on the current save file.
    bool isBlueCoinGotCurrentFile(u8 id);
    // Checks if 240 blue coins on a specified file are true.
    bool isBlueCoinGot240(u8 fileID);
    // Sets all blue coin slots to false and starts a save.
    void resetAllBlueCoin(u8 file);
    // Sets all blue coins on all files to false.
    void resetAllBlueCoinAllFileNoSave();
    // Updates the Blue Coin counter
    void startCounterCountUp();
    // Gets the collected number of collected blue coins.
    s32 getTotalBlueCoinNum(u8 file);
    // Gets the collected number of collected blue coins on the current file.
    s32 getTotalBlueCoinNumCurrentFile();
    // Gets the number of blue coins a stage has, or the number collected in a stage.
    // Definitions must be added to the BCSV. Set pStageName to 0 for current stage.
    s32 getBlueCoinRangeData(const char* pStageName, bool collectedCoinsOnly);
}
#endif