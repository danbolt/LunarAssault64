#include <nusys.h>
#include "main.h"
#include "segmentinfo.h"

#include "stage00.h"
#include "dialoguestage.h"
#include "titlescreenstage.h"

#include "kaiju1.h"
#include "kaiju2.h"

void stage00(int);
void dialogue(int);
void titlescreen(int);

volatile int changeScreensFlag;
volatile ScreenSetting screenType;
volatile int currentLevel;

NUContData	contdata[1];
u8 contPattern;

u32 groundTextureROMAddress;
u32 terrainROMAddress;
u32 topographyROMAddress;

void (*initKaijuCallback)();
void (*updateKaijuCallback)(float);
void (*renderKaijuCallback)(DisplayData*);

void loadInStageState(int levelNumber) {
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

  groundTextureROMAddress = (u32)_moon_geoSegmentRomStart;
  terrainROMAddress = (u32)_level1_terrainSegmentRomStart;
  topographyROMAddress = (u32)_level1_topographySegmentRomStart;

  nuPiReadRomOverlay(&segment);
}

void loadInKaiju(int levelNumber) {
  NUPiOverlaySegment segment;

  if (levelNumber == 0) {
    segment.romStart  = _kaiju1SegmentRomStart;
    segment.romEnd    = _kaiju1SegmentRomEnd;
    segment.ramStart  = _kaiju1SegmentStart;
    segment.textStart = _kaiju1SegmentTextStart;
    segment.textEnd   = _kaiju1SegmentTextEnd;
    segment.dataStart = _kaiju1SegmentDataStart;
    segment.dataEnd   = _kaiju1SegmentDataEnd;
    segment.bssStart  = _kaiju1SegmentBssStart;
    segment.bssEnd    = _kaiju1SegmentBssEnd;
  } else if (levelNumber == 1) {
    segment.romStart  = _kaiju2SegmentRomStart;
    segment.romEnd    = _kaiju2SegmentRomEnd;
    segment.ramStart  = _kaiju2SegmentStart;
    segment.textStart = _kaiju2SegmentTextStart;
    segment.textEnd   = _kaiju2SegmentTextEnd;
    segment.dataStart = _kaiju2SegmentDataStart;
    segment.dataEnd   = _kaiju2SegmentDataEnd;
    segment.bssStart  = _kaiju2SegmentBssStart;
    segment.bssEnd    = _kaiju2SegmentBssEnd;
  }

  nuPiReadRomOverlay(&segment);

  if (levelNumber == 0) {
    initKaijuCallback = &initKaiju1;
    updateKaijuCallback = &updateKaiju1;
    renderKaijuCallback = &renderKaiju1;
  } else if (levelNumber == 1) {
    initKaijuCallback = &initKaiju2;
    updateKaijuCallback = &updateKaiju2;
    renderKaijuCallback = &renderKaiju2;
  }

}

void loadInDialogueState() {
  NUPiOverlaySegment segment;

  segment.romStart  = _dialogueSegmentRomStart;
  segment.romEnd    = _dialogueSegmentRomEnd;
  segment.ramStart  = _dialogueSegmentStart;
  segment.textStart = _dialogueSegmentTextStart;
  segment.textEnd   = _dialogueSegmentTextEnd;
  segment.dataStart = _dialogueSegmentDataStart;
  segment.dataEnd   = _dialogueSegmentDataEnd;
  segment.bssStart  = _dialogueSegmentBssStart;
  segment.bssEnd    = _dialogueSegmentBssEnd;

  nuPiReadRomOverlay(&segment);
}

void loadInTitleScreenState() {
  NUPiOverlaySegment segment;

  segment.romStart  = _titlescreenSegmentRomStart;
  segment.romEnd    = _titlescreenSegmentRomEnd;
  segment.ramStart  = _titlescreenSegmentStart;
  segment.textStart = _titlescreenSegmentTextStart;
  segment.textEnd   = _titlescreenSegmentTextEnd;
  segment.dataStart = _titlescreenSegmentDataStart;
  segment.dataEnd   = _titlescreenSegmentDataEnd;
  segment.bssStart  = _titlescreenSegmentBssStart;
  segment.bssEnd    = _titlescreenSegmentBssEnd;

  nuPiReadRomOverlay(&segment);
}

void initAudio(void)
{
  nuAuInit();

  nuAuSeqPlayerBankSet(_audiobank_controlSegmentRomStart, _audiobank_controlSegmentRomEnd - _audiobank_controlSegmentRomStart, _audiobank_tableSegmentRomStart);
  nuAuSeqPlayerSeqSet(_songsSegmentRomStart);
}

/*------------------------
	Main
--------------------------*/
void mainproc(void)
{
  /* The initialization of graphic  */
  nuGfxInit();

  initAudio();

  changeScreensFlag = 0;
  screenType = TitleScreen;

  currentLevel = 0;

  /* The initialization of the controller manager  */
  contPattern = nuContInit();

  while (1) {
    if (screenType == StageScreen) {
      loadInStageState(currentLevel);
      loadInKaiju(currentLevel);
      initStage00();
      nuGfxFuncSet((NUGfxFunc)stage00);
    } else if (screenType == DialogueScreen) {
      loadInDialogueState();
      initDialogue();
      nuGfxFuncSet((NUGfxFunc)dialogue);
    } else if (screenType == TitleScreen) {
      loadInTitleScreenState();
      initTitleScreen();
      nuGfxFuncSet((NUGfxFunc)titlescreen);
    }

    nuGfxDisplayOn();

    while(!changeScreensFlag);

    nuGfxFuncRemove();
    nuGfxDisplayOff();

    if (currentLevel == NUMBER_OF_LEVELS) {
      currentLevel = 0;
      screenType = TitleScreen;
    }

    changeScreensFlag = 0;
  }
}

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

void dialogue(int pendingGfx)
{
  if (changeScreensFlag != 0) {
    return;
  }

  if(pendingGfx < 3) {
    makeDLDialogue();   
  }

  updateDialogue(); 
}

void titlescreen(int pendingGfx)
{
  if (changeScreensFlag != 0) {
    return;
  }

  if(pendingGfx < 3) {
    makeDLTitleScreen();   
  }

  updateTitleScreen(); 
}

