#pragma once

#include "pt/AreaObj/JumpSwitchArea.h"
#include "pt/Enemy/Anagon.h"
#include "pt/Enemy/BallBeamer.h"
#include "pt/Enemy/CocoSambo.h"
#include "pt/Enemy/DharmaSambo.h"
#include "pt/Enemy/JumpGuarder.h"
#include "pt/Enemy/Mogu.h"
#include "pt/Enemy/Poihana.h"
#include "pt/Map/CollectSwitchCtrl.h"
#include "pt/Map/PlayerSwitchCtrl.h"
#include "pt/Map/SensorDetector.h"
#include "pt/MapObj/Banekiti.h"
#include "pt/MapObj/BlueChip.h"
#include "pt/MapObj/ChooChooTrain.h"
#include "pt/MapObj/CrystalSwitch.h"
#include "pt/MapObj/DeadLeaves.h"
#include "pt/MapObj/FirePressureRadiate.h"
#include "pt/MapObj/HitWallTimerSwitch.h"
#include "pt/MapObj/Jiraira.h"
#include "pt/MapObj/LavaBallRisingPlanetLava.h"
#include "pt/MapObj/NewMorphItemNeo.h"
#include "pt/MapObj/PomponPlant.h"
#include "pt/MapObj/PowerStarSpawner.h"
#include "pt/MapObj/ScrewSwitchBase.h"
#include "pt/MapObj/WatchTowerRotateStep.h"
#include "pt/MapObj/WaterLeakPipe.h"

#include "pt/AreaObj/WarpArea.h"
#include "pt/MapObj/SwitchBox.h"
#include "pt/MapObj/RedCoinSystem/RedCoin.h"
#include "pt/MapObj/RedCoinSystem/RedCoinSwitch.h"
#include "pt/AreaObj/SMG63Area.h"
#include "pt/MapObj/DisplayStar.h"
#include "pt/Map/CometTimerObj.h"
#include "pt/MapObj/BlueCoinSystem/BlueCoin.h"
#include "pt/MapObj/PTimerSwitch.h"
#include "pt/MapObj/LavaSteam.h"

/*
* SMG2 actor class definitions for use with "NameObjFactory::createNameObj". We only need these declarations. The actual
* templated functions already exist so the linker will do the remaining work for us.
*/
class AirFar100m;
class AssemblyBlock;
class BallOpener;
class BeeJumpBall;
class BlueStarCupsulePlanet;
class GoroRockLaneParts;
class GravityFallBlock;
class InvisiblePolygonObj;
class JumpHole;
class ManholeCover;
class Shellfish;
class SimpleEnvironmentObj;
class SimpleTextureSwitchChangeObj;
class WoodBox;


namespace pt {
    template<typename T>
    NameObj* createExtActor(const char *pName) {
        return new T(pName);
    }

    template<s32 COLOR>
    NameObj* createSuperSpinDriverCustomColor(const char *pName) {
        return new SuperSpinDriver(pName, COLOR);
    }

    struct CreateSceneObjEntry {
        s32 mSlotId;
        NameObj* (*mCreationFunc)(void);
    };

    /*
    * Extended NameObjFactory
    */

    template<>
    NameObj* createExtActor<MorphItemNeoFoo>(const char *pName);

    template<>
    NameObj* createExtActor<MorphItemNeoIce>(const char *pName);

    NameObj* createQuakeEffectArea(const char *pName);
    NameObj* createSuperSpinDriverGreen(const char *pName);

    const CreateActorEntry cNewCreateNameObjTable[] = {
        { "ExtraWallCheckCylinder", NameObjFactory::createBaseOriginCylinder<AreaObj> },
        // AreaObj
        { "ForbidJumpArea", NameObjFactory::createNameObj<AreaObj> },
        { "ForbidWaterSearchArea", NameObjFactory::createNameObj<AreaObj> },
        { "JumpSwitchArea", createExtActor<JumpSwitchArea> },
        { "PipeModeCube", NameObjFactory::createBaseOriginCube<AreaObj> },
        { "PlaneCircularModeCube", NameObjFactory::createBaseOriginCube<AreaObj> },
        { "QuakeEffectArea", createQuakeEffectArea },
        { "Anagon", createExtActor<Anagon> },
        // Enemy
        { "BallBeamer", createExtActor<BallBeamer> },
        { "CocoSambo", createExtActor<CocoSambo> },
        { "DharmaSambo", createExtActor<DharmaSambo> },
        { "JumpGuarder", createExtActor<JumpGuarder> },
        { "Mogu", createExtActor<Mogu> },
        { "Poihana", createExtActor<Poihana> },
        { "CollectSwitchCtrl", createExtActor<CollectSwitchCtrl> },
        // Map
        { "PlayerSwitchCtrl", createExtActor<PlayerSwitchCtrl> },
        { "SensorDetector", createExtActor<SensorDetector> },
        { "CometTimerObj", createExtActor<CometTimerObj> },
        { "Banekiti", createExtActor<Banekiti> },
        // MapObj
        { "BlueChip", createExtActor<BlueChip> },
        { "BlueChipGroup", createExtActor<BlueChipGroup> },
        { "CrystalSwitch", createExtActor<CrystalSwitch> },
        { "DeadLeaves", createExtActor<DeadLeaves> },
        { "DisplayStar", createExtActor<DisplayStar> },
        { "FirePressureRadiate", createExtActor<FirePressureRadiate> },
        { "GoldenTurtle", createExtActor<GoldenTurtle> },
        { "HitWallTimerSwitch", createExtActor<HitWallTimerSwitch> },
        { "Jiraira", createExtActor<Jiraira> },
        { "MorphItemCollectionFoo", NameObjFactory::createNameObj<MorphItemCollection> },
        { "MorphItemCollectionIce", NameObjFactory::createNameObj<MorphItemCollection> },
        { "MorphItemNeoFoo", createExtActor<MorphItemNeoFoo> },
        { "MorphItemNeoIce", createExtActor<MorphItemNeoIce> },
        { "MultipleChoiceDice", createExtActor<MultipleChoiceDice> },
        { "Pompon2Plant", createExtActor<PomponPlant> },
        { "PowerStarSpawner", createExtActor<PowerStarSpawner> },
        { "PTimerSwitch", createExtActor<PTimerSwitch> },
        { "ScrewSwitch", createExtActor<ScrewSwitch> },
        { "ScrewSwitchReverse", createExtActor<ScrewSwitchReverse> },
        { "ShellfishBlueChip", NameObjFactory::createNameObj<Shellfish> },
        { "ShellfishPurpleCoin", NameObjFactory::createNameObj<Shellfish> },
        { "UFOBlueStarCupsule", NameObjFactory::createNameObj<BlueStarCupsulePlanet> },
        { "ValveSwitch", createExtActor<ValveSwitch> },
        { "WatchTowerRotateStep", createExtActor<WatchTowerRotateStep>},
        { "WaterLeakPipe", createExtActor<WaterLeakPipe>},
        // Ride
        { "SwingRope", createExtActor<SwingRope> },
        { "SwitchBox", createExtActor<SwitchBox> },
        { "RedCoin", createExtActor<RedCoin> },
        { "RedCoinController", createExtActor<RedCoinController> },
        { "RedCoinSwitch", createExtActor<RedCoinSwitch> },
        // Custom SuperSpinDriver colors
        { "SuperSpinDriverGreen", createSuperSpinDriverGreen },
        { "SuperSpinDriverRed", createSuperSpinDriverCustomColor<3> },
        { "SuperSpinDriverBlue", createSuperSpinDriverCustomColor<4> },
        { "SuperSpinDriverRainbow", createSuperSpinDriverCustomColor<5> },
        { "SuperSpinDriverPurple", createSuperSpinDriverCustomColor<6> },
        { "SuperSpinDriverBlack", createSuperSpinDriverCustomColor<7> },
        { "SuperSpinDriverWhite", createSuperSpinDriverCustomColor<8> },
        { "SuperSpinDriverYellow", createSuperSpinDriverCustomColor<9> },

        #ifdef ALL
            { "LavaSteam", createExtActor<LavaSteam> },
        #endif

        #ifdef SMSS
            { "BlueCoin", createExtActor<BlueCoin> },
        #endif

        #ifdef NOGLE
            { "WarpArea", createExtActor<WarpArea> },
        #endif

        #if defined(SMG63) || defined (SMSS)
            { "SMG63Area", createExtActor<SMG63Area> },
        #endif
    };


    /*
    * Extended ProductMapCreator
    */

    const CreateActorEntry cNewCreateMapObjTable[] = {
        { "AirFar100m", NameObjFactory::createNameObj<AirFar100m> },
        { "AssemblyBlock", NameObjFactory::createNameObj<AssemblyBlock> },
        { "BallOpener", NameObjFactory::createNameObj<BallOpener> },
        { "BeeJumpBall", NameObjFactory::createNameObj<BeeJumpBall> },
        { "ChooChooTrain", createExtActor<ChooChooTrain> },
        { "GoroRockLaneParts", NameObjFactory::createNameObj<GoroRockLaneParts> },
        { "GravityFallBlock", NameObjFactory::createNameObj<GravityFallBlock> },
        { "InvisiblePolygonObj", NameObjFactory::createNameObj<InvisiblePolygonObj> },
        { "JumpHole", NameObjFactory::createNameObj<JumpHole> },
        { "ManholeCover", NameObjFactory::createNameObj<ManholeCover> },
        { "ScaleMapObj", createExtActor<LavaBallRisingPlanetLava>},
        { "SimpleEnvironmentObj", NameObjFactory::createNameObj<SimpleEnvironmentObj> },
        { "SimpleTextureSwitchChangeObj", NameObjFactory::createNameObj<SimpleTextureSwitchChangeObj> },
        { "WoodBox", NameObjFactory::createNameObj<WoodBox> }
    };


    /*
    * Extended SceneObjHolder
    *
    * There is a bunch of free SceneObj slots that we can still use as desired. The following slots
    * are unused as of now: 0x3E, 0x47, 0x48, 0x49, 0x54, 0x5A, 0x68, 0x69 and 0x7B.
    */
    const CreateSceneObjEntry cNewCreateSceneObjTable[] = {
        { SCENE_OBJ_BLUE_CHIP_HOLDER, createBlueChipHolder }
    };
};
