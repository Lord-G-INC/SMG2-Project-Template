#include "revolution.h"
#include "kamek/hooks.h"
#include "pt/Syati/GstRecord.h"
#include "pt/Syati/PadRecord.h"
#include "pt/Util/ActorUtil.h"
#include "pt/MapObj/BlueCoinSystem/BlueCoinUtil.h"
#include "pt/init.h"

typedef void (*Func)();
extern Func __ctor_loc;
extern Func __ctor_end;

void* gStageEventDataTableBCSV;
void* gWarpAreaStageTableBCSV;
void* gFileSelectDataTable;
void* gBoardDataTable;
void* gBlueCoinIDRangeTable;
void* gDummyDisplayModelTable;

namespace {
	// ----------------------------------------------------------------------------------------------------------------
	// Calls all static initializers after MR::initAcosTable. This is necessary to initialize static variables and 
	// constants in the BSS segment. Custom Nerve instances will be stored there, for example.

	void init() {
		for (Func* f = &__ctor_loc; f < &__ctor_end; f++) {
			(*f)();
		}

        const char* name = "Unknown"; 

        #ifdef ALL
        name = "ALL";
        #elif SMSS
        name = "SMSS (GLE + BCS)";
        #elif CA
        name = "CA (GLE)";
        #elif SMG63
        name = "SMG63 (GLE + BCS)";
        #elif GLE
        name = "GLE";
        #elif NOGLE
        name = "NOGLE";
        #else
        name = "N/A";
        #endif

        OSReport("SMG2PTD INIT: Created by Evanbowl and the Lord-G Inc Team\nBuilt: %s, %s\nTarget: %s\n", __DATE__, __TIME__, name);

	}

	// Called after initAcosTable
	kmBranch(0x8003B344, init);

    void handleCustomArchiveLoads() {
        #if defined USEBLUECOIN && !defined SM64BLUECOIN
            BlueCoinUtil::initBlueCoinArray();
        #endif
        #ifdef NOGLE
        gStageEventDataTableBCSV = pt::loadArcAndFile("/SystemData/PTSystemData.arc", "/StageEventDataTable.bcsv");
        gWarpAreaStageTableBCSV = pt::loadArcAndFile("/SystemData/PTSystemData.arc", "/WarpAreaStageTable.bcsv");
        #endif
        #ifdef SMSS
        gFileSelectDataTable = pt::loadArcAndFile("/ObjectData/FileSelectData.arc", "/FileSelectData.bcsv");
        gBoardDataTable = pt::loadArcAndFile("/SystemData/BlueCoinBoardDataTable.arc", "/BlueCoinBoardDataTable.bcsv");
        #endif
        #if defined USEBLUECOIN && !defined SM64BLUECOIN
        gBlueCoinIDRangeTable = pt::loadArcAndFile("/SystemData/BlueCoinIDRangeTable.arc", "/BlueCoinIDRangeTable.bcsv");
        #endif

        // Lord-G/Evan, fix this. Please.
        #ifdef SMG63
        gDummyDisplayModelTable = pt::loadArcAndFile("/SystemData/DummyDisplayModelTable.arc", "/DummyDisplayModelTable.bcsv");
        #else
        gDummyDisplayModelTable = pt::loadArcAndFile("/SystemData/PTSystemData.arc", "/DummyDisplayModelTable.bcsv");
        #endif
    }

    kmBranch(0x804B69F4, handleCustomArchiveLoads);

#ifdef GSTANDPAD
    	// ----------------------------------------------------------------------------------------------------------------
    	// Events to be handled after GameScene::start
    
    	void handleGameSceneStart() {
    		pad::startPadRecorderPrepared();
    	}
    
    	kmBranch(0x80451888, handleGameSceneStart);
    
    
    	// ----------------------------------------------------------------------------------------------------------------
    	// Events to be handled after any scene gets destroyed
    
    	void handleAnySceneDestroyed() {
    		gst::waitGstRecorder();
    		pad::waitPadRecorderNotPrepared();
    	}
    
    #if defined(PAL) || defined(USA) || defined(JPN)
    	kmBranch(0x804BAB30, handleAnySceneDestroyed);
    #elif defined(TWN) || defined(KOR)
    	kmBranch(0x804BABA0, handleAnySceneDestroyed);
    #endif
#endif
}