#include "pt/Util/ActorUtil.h"
#include "pt/MapObj/BlueCoinHolder.h"

BlueCoinHolder::BlueCoinHolder() {
    void* bcsv = pt::loadArcAndFile("/SytemData/BlueCoinCount.arc", "/BlueCoinCount/BlueCoinCount.bcsv");
    JMapInfo* table = new JMapInfo();
    table->attach(bcsv);
    int num = MR::getCsvDataElementNum(table);
    for (int i = 0; i < num; i++) {
        MR::getCsvDataU8(&Count, table, "Count", i);
    }
    Coins = new BlueCoinInstance[Count];
}