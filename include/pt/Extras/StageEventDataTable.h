#pragma once

#include "syati.h"

class NerveExecutor;

namespace StageEventDataTable {
    bool readTable(const char* value, const char* stageName);
    bool readTableCurrentStage(const char* value);
};
