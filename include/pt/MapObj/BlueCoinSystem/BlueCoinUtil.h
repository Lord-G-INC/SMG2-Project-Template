#if defined USEBLUECOIN && !defined SM64BLUECOIN
#pragma once
#include "syati.h"

struct BlueCoinData {
    bool** collectionData;
    bool flags[3][8];
    u8 spentData[3];
    bool hasSeenTextBox[3];
};

extern BlueCoinData* gBlueCoinData;

namespace BlueCoinUtil {
    /// @brief Loads BlueCoinData.bin into gBlueCoinData bool array.
    void loadBlueCoinData();
    
    /// @brief Saves gBlueCoinData bool array into BlueCoinData.bin.
    void saveBlueCoinData();

    /// @brief Prints the Blue Coin counts of each save file and gBlueCoinFlag's value to the log.
    void printBlueCoinSaveFileInfo();

    /// @brief Clears all Blue Coin Save File data
    void clearBlueCoinData();
    
    /// @brief Initializes the BlueCoinData struct
    void initBlueCoinArray();

    /// @brief Gets the current save file ID
    /// @return s32
    s32 getCurrentFileNum();

    /// @brief Sets a blue coin to true on the current save file.
    /// @param id Blue Coin ID
    void setBlueCoinGotCurrentFile(u8 id);

    /// @brief Checks if a specific blue coin is collected on the specified save file.
    /// @param file File ID
    /// @param id Blue Coin ID
    /// @return bool 
    bool isBlueCoinGot(u8 file, u8 id);

    /// @brief Checks if a specific blue coin is collected on the current save file.
    /// @param id Blue Coin ID
    /// @return bool 
    bool isBlueCoinGotCurrentFile(u8 id);
   
    /// @brief Checks the one-time "OnFirstBlueCoin" flag
    /// @return bool
    bool hasSeenBlueCoinTextBoxCurrentFile();

    /// @brief Sets the one-time "OnFirstBlueCoin" flag to true on the current file.
    void setSeenBlueCoinTextBoxCurrentFile();

    /// @brief Checks if the one-time "OnFirstBlueCoin" text box is being displayed.
    /// @return bool
    bool isBlueCoinTextBoxAppeared();

    /// @brief Checks the specified BlueCoinFlag on the current file.
    /// @param flag File ID
    /// @return bool
    void setOnBlueCoinFlagCurrentFile(u8 flag);

    /// @brief Checks the specified BlueCoinFlag on the current file.
    /// @param flag File ID
    /// @return bool
    bool isOnBlueCoinFlagCurrentFile(u8 flag);

    /// @brief Wipes Blue Coin data on the target file and starts a save.
    /// @param file File ID
    void resetAllBlueCoin(u8 file);

    /// @brief Updates the Blue Coin counter
    void startCounterCountUp();

    /// @brief Spends an amount of Blue Coins on the current file. Using 0 defaults to 30.
    /// @param numcoin Number of coins to spend
    void spendBlueCoinCurrentFile(u8 numcoin);
    
    /// @brief Returns the total number of spent blue coins on the specified file.
    /// @param file File ID
    /// @return s32
    s32 getSpentBlueCoinNum(u8 file);
    
    /// @brief Returns the total number of spent blue coins on the currrent file.
    /// @return s32
    s32 getSpentBlueCoinNumCurrentFile();

    /// @brief Gets the collected number of collected blue coins.
    /// @param file 
    /// @param ignoreSpent Do not count spent blue coins.
    /// @return s32
    s32 getTotalBlueCoinNum(u8 file, bool ignoreSpent);

    /// @brief Gets the collected number of collected blue coins on the current file.
    /// @param ignoreSpent Do not count spent blue coins.
    /// @return s32
    s32 getTotalBlueCoinNumCurrentFile(bool ignoreSpent);
    
    /// @brief Gets the number of blue coins a stage has, or the number collected in a stage. Requires a properly set up BCSV.
    /// @param pStageName Stage Name to search for in table.
    /// @param collectedCoinsOnly Only count collected coins in range.
    /// @return s32
    s32 getBlueCoinRangeData(const char* pStageName, bool collectedCoinsOnly);

    /// @brief Creates a Blue Coin that can be used for spawning.
    /// @param pActor Source Actor.
    /// @param id Blue Coin ID.
    /// @return LiveActor*
    LiveActor* createBlueCoinForSpawning(LiveActor* pActor, s32 id);

    /// @brief Appears a Blue Coin
    /// @param pSourceActor Source Actor
    /// @param pBlueCoin Blue Coin to appear
    void appearBlueCoin(LiveActor* pSourceActor, LiveActor* pBlueCoin);
    };
#endif