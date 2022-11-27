#pragma once

#include "syati.h"

#if defined (ALL) || defined (NOGLE)
namespace StageEventDataTable {
    bool readTable(const char* value, const char* stageName);
    bool readTableCurrentStage(const char* value);
};
#endif 
