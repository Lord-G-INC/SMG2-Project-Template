#pragma once

#include "syati.h"

#ifndef GLE
namespace StageEventDataTable {
    bool readTable(const char* value, const char* stageName);
    bool readTableCurrentStage(const char* value);
};
#endif 
