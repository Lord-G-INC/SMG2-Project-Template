#include "syati.h"
#include "pt/MapObj/BlueCoinSystem/BlueCoinUtil.h"

namespace pt {
    /*
    * New Dummy items
    *
    * New variants can be easily added and created here. This extends the existing set of dummy models. This replaces
    * all calls to MR::tryCreateDummyModel, but uses the original function to initialize the base game's items.
    */

    DummyDisplayModelInfo cNewDummyDisplayModels[] = {
        /* 15 */ { "PowerUpFire",       NULL, { 0.0f, 70.0f, 0.0f }, 16, NULL, false },
        /* 16 */ { "PowerUpIce",        NULL, { 0.0f, 70.0f, 0.0f }, 16, NULL, false },
        /* 17 */ { "PowerUpBee",        NULL, { 0.0f, 70.0f, 0.0f }, 16, NULL, false },
        /* 18 */ { "PowerUpHopper",     NULL, { 0.0f, 70.0f, 0.0f }, 16, NULL, false },
        /* 19 */ { "PowerUpTeresa",     NULL, { 0.0f, 70.0f, 0.0f }, 16, NULL, false },
        /* 20 */ { "PowerUpFoo",        NULL, { 0.0f, 70.0f, 0.0f }, 16, NULL, false },
        /* 21 */ { "PowerUpCloud",      NULL, { 0.0f, 70.0f, 0.0f }, 16, NULL, false },
        /* 22 */ { "PowerUpRock",       NULL, { 0.0f, 70.0f, 0.0f }, 16, NULL, false },
        /* 23 */ { "PowerUpInvincible", NULL, { 0.0f, 70.0f, 0.0f }, 16, NULL, false },
        /* 24 */ { "RedCoin",           "Dummy", { 0.0f, 70.0f, 0.0f }, 16, NULL, false },

        #ifdef USEBLUECOIN
        /* 25 */ { "BlueCoin",          "Dummy", { 0.0f, 70.0f, 0.0f }, 16, NULL, false },
        /* 26 */ { "BlueCoinClear",     "Dummy", { 0.0f, 70.0f, 0.0f }, 16, NULL, false },
        #endif

        /* 25/27 */ { "PurpleCoin",     NULL, { 0.0f, 70.0f, 0.0f }, 16, NULL, false }
    };

    DummyDisplayModel* tryCreateNewDummyModel(LiveActor *pHost, const JMapInfoIter &rIter, s32 defaultId, int v4) {
        s32 modelId = MR::getDummyDisplayModelId(rIter, defaultId);

        if (modelId < 0 || modelId > 14 + sizeof(cNewDummyDisplayModels) / sizeof(cNewDummyDisplayModels[0])) {
            return NULL;
        }

        if (modelId <= 14) {
            return MR::tryCreateDummyModel(pHost, rIter, modelId, v4);
        }

        s32 colorId = 0;

        if (MR::isValidInfo(rIter)) {
            MR::getJMapInfoArg6NoInit(rIter, &colorId);
        }
        
        #if defined USEBLUECOIN && !defined SM64BLUECOIN
            if (modelId == 25 && BlueCoinUtil::isBlueCoinGotCurrentFile(colorId))
                modelId = 26;
        #endif
        
        DummyDisplayModelInfo *pInfo = &cNewDummyDisplayModels[modelId - 15];
        DummyDisplayModel *pModel = new DummyDisplayModel(pHost, pInfo, v4, modelId, colorId);
        pModel->initWithoutIter();
        return pModel;
    }

    // Hooks
    kmCall(0x80295BB0, tryCreateNewDummyModel);
    kmCall(0x80295C28, tryCreateNewDummyModel);
    kmCall(0x80295CAC, tryCreateNewDummyModel);
    kmCall(0x80295D04, tryCreateNewDummyModel);
    kmCall(0x80295D88, tryCreateNewDummyModel);

    // Skip repeated reading of Obj_arg7 field
    kmWrite32(0x801D0314, 0x7CA32B78);
    kmWrite32(0x801D0318, 0x60000000);

    s32 spinCustomDisplayModels() {
        s32 modelID = 0;
        asm("lwz %0, 0xA4(r30)" : "=r" (modelID));

        if (modelID >= 24 && modelID <= 27)
            return 0;

        return modelID;
    }

    kmCall(0x80295A10, spinCustomDisplayModels);
    kmWrite32(0x80295A14, 0x2C030000);
    kmWrite32(0x80295A1C, 0x2C030007);
    kmWrite32(0x80295A24, 0x2C03000D);
}
