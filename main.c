#include <nusys.h>
#include "main.h"
#include "segmentinfo.h"

#include "stage00.h"

#include "kaiju1.h"

void stage00(int);

volatile int changeScreensFlag;
volatile ScreenSetting screenType;

NUContData	contdata[1];
u8 contPattern;

u32 groundTextureROMAddress;
u32 terrainROMAddress;
u32 topographyROMAddress;

extern u8 _moon_geoSegmentRomStart[];
extern u8 _level1_terrainSegmentRomStart[];
extern u8 _level1_topographySegmentRomStart[];

void (*initKaijuCallback)();
void (*updateKaijuCallback)(float);
void (*renderKaijuCallback)(DisplayData*);

void loadInStageState() {
  NUPiOverlaySegment segment;

  segment.romStart  = _stageSegmentRomStart;
  segment.romEnd    = _stageSegmentRomEnd;
  segment.ramStart  = _stageSegmentStart;
  segment.textStart = _stageSegmentTextStart;
  segment.textEnd   = _stageSegmentTextEnd;
  segment.dataStart = _stageSegmentDataStart;
  segment.dataEnd   = _stageSegmentDataEnd;
  segment.bssStart  = _stageSegmentBssStart;
  segment.bssEnd    = _stageSegmentBssEnd;

  nuPiReadRomOverlay(&segment);
}

/*------------------------
	Main
--------------------------*/
void mainproc(void)
{
  /* The initialization of graphic  */
  nuGfxInit();

  changeScreensFlag = 0;
  screenType = StageScreen;

  /* The initialization of the controller manager  */
  contPattern = nuContInit();

  while (1) {
    groundTextureROMAddress = (u32)_moon_geoSegmentRomStart;
    terrainROMAddress = (u32)_level1_terrainSegmentRomStart;
    topographyROMAddress = (u32)_level1_topographySegmentRomStart;
    initKaijuCallback = &initKaiju1;
    updateKaijuCallback = &updateKaiju1;
    renderKaijuCallback = &renderKaiju1;

    loadInStageState();

    initStage00();

    nuGfxFuncSet((NUGfxFunc)stage00);
    nuGfxDisplayOn();

    while(!changeScreensFlag);

    nuGfxFuncRemove();
    nuGfxDisplayOff();

    changeScreensFlag = 0;
  }
}

/*-----------------------------------------------------------------------------
  The call-back function 

  pendingGfx which is passed from Nusystem as the argument of the call-back 
  function is the total of RCP tasks that are currently processing and 
  waiting for the process. 
-----------------------------------------------------------------------------*/
void stage00(int pendingGfx)
{
  if (changeScreensFlag != 0) {
    return;
  }

  /* Provide the display process if 2 or less RCP tasks are processing or
	waiting for the process.  */
  if(pendingGfx < 3)
    makeDL00();		

  /* The process of game progress  */
  updateGame00(); 
}

