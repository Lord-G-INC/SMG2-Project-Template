#pragma once

#include "pt/AreaObj/JumpSwitchArea.h"
#include "pt/Enemy/Anagon.h"
#include "pt/Enemy/BallBeamer.h"
#include "pt/Enemy/CocoSambo.h"
#include "pt/Enemy/DharmaSambo.h"
#include "pt/Enemy/JumpGuarder.h"
#include "pt/Enemy/Mogu.h"
#include "pt/Enemy/Poihana.h"
#include "pt/Extras/ExtQuakeEffectArea.h"
#include "pt/Extras/ExtSuperSpinDriver.h"
#include "pt/Map/CollectSwitchCtrl.h"
#include "pt/Map/PlayerSwitchCtrl.h"
#include "pt/Map/SensorDetector.h"
#include "pt/MapObj/Banekiti.h"
#include "pt/MapObj/BlueChip.h"
#include "pt/MapObj/CrystalSwitch.h"
#include "pt/MapObj/DeadLeaves.h"
#include "pt/MapObj/FirePressureRadiate.h"
#include "pt/MapObj/HitWallTimerSwitch.h"
#include "pt/MapObj/Jiraira.h"
#include "pt/MapObj/NewMorphItemNeo.h"
#include "pt/MapObj/PomponPlant.h"
#include "pt/MapObj/PowerStarSpawner.h"
#include "pt/MapObj/ScrewSwitchBase.h"
#include "pt/MapObj/WatchTowerRotateStep.h"
#include "pt/MapObj/WaterLeakPipe.h"
#include "sample/GstRecord.h"
#include "sample/PadRecord.h"

// PTD Includes
#include "pt/AreaObj/WarpArea.h"
#include "pt/MapObj/SwitchBox.h"
#include "pt/MapObj/RedCoinSystem/RedCoin.h"
#include "pt/MapObj/RedCoinSystem/RedCoinSwitch.h"
#include "pt/AreaObj/SMG63Area.h"
#include "pt/MapObj/DisplayStar.h"
#include "pt/Map/CometTimerObj.h"
#include "pt/MapObj/BlueCoinSystem/BlueCoin.h"
#include "pt/MapObj/BlueCoinSystem/BlueCoinBoard.h"
#include "pt/MapObj/PTimerSwitch.h"
#include "pt/MapObj/LavaSteam.h"

class BlueStarCupsulePlanet;

namespace {
    template<typename T>
    NameObj* createExtActor(const char *pName) {
        return new T(pName);
    }

    template<>
    NameObj* createExtActor<pt::MorphItemNeoFoo>(const char *pName);

    template<>
    NameObj* createExtActor<pt::MorphItemNeoIce>(const char *pName);

    template<>
    NameObj* createExtActor<pt::PowerStarSpawner>(const char *pName);

    // ----------------------------------------------------------------------------------------------------------------

    const CreateActorEntry cNewCreateNameObjTable[] = {
        // AreaObj
        { "ExtraWallCheckCylinder", NameObjFactory::createBaseOriginCylinder<AreaObj> },
        { "ForbidJumpArea", NameObjFactory::createNameObj<AreaObj> },
        { "ForbidWaterSearchArea", NameObjFactory::createNameObj<AreaObj> },
        { "JumpSwitchArea", createExtActor<pt::JumpSwitchArea> },
        { "PipeModeCube", NameObjFactory::createBaseOriginCube<AreaObj> },
        { "PlaneCircularModeCube", NameObjFactory::createBaseOriginCube<AreaObj> },
        { "QuakeEffectArea", pt::createQuakeEffectArea },

        // Enemy
        { "Anagon", createExtActor<pt::Anagon> },
        { "BallBeamer", createExtActor<pt::BallBeamer> },
        { "CocoSambo", createExtActor<pt::CocoSambo> },
        { "DharmaSambo", createExtActor<pt::DharmaSambo> },
        { "JumpGuarder", createExtActor<pt::JumpGuarder> },
        { "Mogu", createExtActor<pt::Mogu> },
        { "Poihana", createExtActor<pt::Poihana> },

        // Map
        { "CollectSwitchCtrl", createExtActor<pt::CollectSwitchCtrl> },
        #ifdef GSTANDPAD
        { "GhostLuigiRecordHelper", createExtActor<gst::GhostLuigiRecordHelper> },
        { "PadRecordHelper", createExtActor<pad::PadRecordHelper> },
        #endif
        { "PlayerSwitchCtrl", createExtActor<pt::PlayerSwitchCtrl> },
        { "SensorDetector", createExtActor<pt::SensorDetector> },

        // Map PTD
        { "CometTimerObj", createExtActor<CometTimerObj> },

        // MapObj
        { "Banekiti", createExtActor<pt::Banekiti> },
        { "BlueChip", createExtActor<pt::BlueChip> },
        { "BlueChipGroup", createExtActor<pt::BlueChipGroup> },
        { "CrystalSwitch", createExtActor<pt::CrystalSwitch> },
        { "DeadLeaves", createExtActor<pt::DeadLeaves> },
        { "FirePressureRadiate", createExtActor<pt::FirePressureRadiate> },
        { "GoldenTurtle", createExtActor<GoldenTurtle> },
        { "HitWallTimerSwitch", createExtActor<pt::HitWallTimerSwitch> },
        { "Jiraira", createExtActor<pt::Jiraira> },
        { "MorphItemCollectionFoo", NameObjFactory::createNameObj<MorphItemCollection> },
        { "MorphItemCollectionIce", NameObjFactory::createNameObj<MorphItemCollection> },
        { "MorphItemNeoFoo", createExtActor<pt::MorphItemNeoFoo> },
        { "MorphItemNeoIce", createExtActor<pt::MorphItemNeoIce> },
        { "MultipleChoiceDice", createExtActor<MultipleChoiceDice> },
        { "Pompon2Plant", createExtActor<pt::PomponPlant> },
        { "PowerStarSpawner", createExtActor<pt::PowerStarSpawner> },
        { "ScrewSwitch", createExtActor<pt::ScrewSwitch> },
        { "ScrewSwitchReverse", createExtActor<pt::ScrewSwitchReverse> },
        { "ShellfishBlueChip", NameObjFactory::createNameObj<Shellfish> },
        { "ShellfishPurpleCoin", NameObjFactory::createNameObj<Shellfish> },
        { "SuperSpinDriverGreen", pt::createSuperSpinDriverCustomColor<2> },
        { "UFOBlueStarCupsule", NameObjFactory::createNameObj<BlueStarCupsulePlanet> },
        { "ValveSwitch", createExtActor<pt::ValveSwitch> },
        { "WatchTowerRotateStep", createExtActor<pt::WatchTowerRotateStep>},
        { "WaterLeakPipe", createExtActor<pt::WaterLeakPipe>},

        // MapObj PTD
        { "DisplayStar", createExtActor<DisplayStar> },
        { "RedCoin", createExtActor<RedCoin> },
        { "RedCoinController", createExtActor<RedCoinController> },
        { "RedCoinSwitch", createExtActor<RedCoinSwitch> },
        { "SuperSpinDriverRed", pt::createSuperSpinDriverCustomColor<3> },
        { "SuperSpinDriverBlue", pt::createSuperSpinDriverCustomColor<4> },
        { "SuperSpinDriverRainbow", pt::createSuperSpinDriverCustomColor<5> },
        { "SuperSpinDriverPurple", pt::createSuperSpinDriverCustomColor<6> },
        { "SuperSpinDriverBlack", pt::createSuperSpinDriverCustomColor<7> },
        { "SuperSpinDriverWhite", pt::createSuperSpinDriverCustomColor<8> },
        { "SuperSpinDriverYellow", pt::createSuperSpinDriverCustomColor<9> },
        { "SwitchBox", createExtActor<SwitchBox> },
        
        // Ride
        { "SwingRope", createExtActor<SwingRope> },

        // Misc PTD
        #ifdef WIP
            { "LavaSteam", createExtActor<LavaSteam> },
        #endif

        #ifdef USEBLUECOIN
            { "BlueCoin", createExtActor<BlueCoin> },
        #endif

        #if defined USEBLUECOIN && !defined SM64BLUECOIN && defined SMSS
            { "BlueCoinSign", createExtActor<BlueCoinSign> },
        #endif

        #ifdef NOGLE
            { "WarpArea", createExtActor<pt::WarpArea> },
        #endif

        #if defined SMG63 || defined SMSS
            { "SMG63Area", createExtActor<SMG63Area> },
        #endif
    };

    const s32 cNewCreateNameObjTableCount = sizeof(cNewCreateNameObjTable) / sizeof(CreateActorEntry);
};
