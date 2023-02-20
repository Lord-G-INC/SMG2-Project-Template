#pragma once
#include "syati.h"

extern bool** gBlueCoinData;
bool resetted = false;

namespace BlueCoinUtil {
    void GetData(u8);
    void SaveData(u8);
}