#pragma once
#include "syati.h"

extern bool** gBlueCoinData;
static bool resetted = false;

namespace BlueCoinUtil {
    void GetData();
    void SaveData();
}