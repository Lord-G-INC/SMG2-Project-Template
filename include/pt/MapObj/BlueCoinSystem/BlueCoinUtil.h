#if defined USEBLUECOIN && !defined SM64BLUECOIN
#pragma once
#include "syati.h"

struct BlueCoinData {
    bool** collectionData;
    u8 spentData[3];
    bool hasSeenTextBox[3];
};

extern BlueCoinData* gBlueCoinData;

namespace BlueCoinUtil {
    // Loads BlueCoinData.bin into gBlueCoinData bool array.
    void loadBlueCoinData();
    // Saves gBlueCoinData bool array into BlueCoinData.bin.
    void saveBlueCoinData();
    // Prints the Blue Coin counts of each save file and gBlueCoinFlag's value to the log.
    void printBlueCoinSaveFileInfo();
    // Sets all blue coins on all files to false.
    void clearBlueCoinData();
    // Initializes gBlueCoinData bool array.
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
    bool isBlueCoinGot240(u8 file);
    // Returns the value of the "seen" variable of BlueCoinCounter's SysInfoWindow.
    bool isOnBlueCoinFlagCurrentFile();
    // Sets the "seen" variable of BlueCoinCounter's SysInfoWindow to true.
    void setOnBlueCoinFlagCurrentFile();
    // Checks if the BlueCoinCounter's SysInfoWindow is appeared.
    bool isBlueCoinTextBoxAppeared();
    // Sets all blue coin slots to false and starts a save.
    void resetAllBlueCoin(u8 file);
    // Updates the Blue Coin counter
    void startCounterCountUp();
    // Spends an amount of Blue Coins on the current file. Using 0 defaults to 30.
    void spendBlueCoinCurrentFile(u8 numcoin);
    //
    s32 getSpentBlueCoinNum(u8 file);
    //
    s32 getSpentBlueCoinNumCurrentFile();
    // Gets the collected number of collected blue coins.
    s32 getTotalBlueCoinNum(u8 file, bool ignoreSpent);
    // Gets the collected number of collected blue coins on the current file.
    s32 getTotalBlueCoinNumCurrentFile(bool ignoreSpent);
    // Gets the number of blue coins a stage has, or the number collected in a stage.
    // Definitions must be added to the BCSV. Set pStageName to 0 for current stage.
    s32 getBlueCoinRangeData(const char* pStageName, bool collectedCoinsOnly);
}
#endif