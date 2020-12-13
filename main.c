#include <nusys.h>
#include <nualsgi_n.h>
#include "main.h"
#include "segmentinfo.h"
#include "hvqm.h"

#include "stage00.h"
#include "fmvstage.h"
#include "dialoguestage.h"
#include "titlescreenstage.h"
#include "introcardstage.h"
#include "creditsscreen.h"
#include "retryscreen.h"

#include "kaiju0.h"
#include "kaiju1.h"
#include "kaiju2.h"
#include "kaiju3.h"

void stage00(int);
void dialogue(int);
void titlescreen(int);
void introcard(int);
void fmvtick(int);
void credits(int);
void retry(int);

volatile int changeScreensFlag;
volatile ScreenSetting screenType;
volatile int currentLevel;
volatile u8 finishedLevel;

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

  // TODO: level-specific textures
  if (levelNumber == 0) {
    groundTextureROMAddress = (u32)_simulator_geoSegmentRomStart;
  } else {
    groundTextureROMAddress = (u32)_moon_geoSegmentRomStart;
  }

  if (levelNumber == 0) {
    terrainROMAddress = (u32)_level0_terrainSegmentRomStart;
    topographyROMAddress = (u32)_level0_topographySegmentRomStart;
  }  else if (levelNumber == 1) {
    terrainROMAddress = (u32)_level1_terrainSegmentRomStart;
    topographyROMAddress = (u32)_level1_topographySegmentRomStart;
  } else if (levelNumber == 2) {
    terrainROMAddress = (u32)_level2_terrainSegmentRomStart;
    topographyROMAddress = (u32)_level2_topographySegmentRomStart;
  } else if (levelNumber >= 3) {
    terrainROMAddress = (u32)_level3_terrainSegmentRomStart;
    topographyROMAddress = (u32)_level3_topographySegmentRomStart;
  }

  nuPiReadRomOverlay(&segment);
}

void loadInKaiju(int levelNumber) {
  NUPiOverlaySegment segment;

  if (levelNumber == 0) {
    segment.romStart  = _kaiju0SegmentRomStart;
    segment.romEnd    = _kaiju0SegmentRomEnd;
    segment.ramStart  = _kaiju0SegmentStart;
    segment.textStart = _kaiju0SegmentTextStart;
    segment.textEnd   = _kaiju0SegmentTextEnd;
    segment.dataStart = _kaiju0SegmentDataStart;
    segment.dataEnd   = _kaiju0SegmentDataEnd;
    segment.bssStart  = _kaiju0SegmentBssStart;
    segment.bssEnd    = _kaiju0SegmentBssEnd;
  } else if (levelNumber == 1) {
    segment.romStart  = _kaiju1SegmentRomStart;
    segment.romEnd    = _kaiju1SegmentRomEnd;
    segment.ramStart  = _kaiju1SegmentStart;
    segment.textStart = _kaiju1SegmentTextStart;
    segment.textEnd   = _kaiju1SegmentTextEnd;
    segment.dataStart = _kaiju1SegmentDataStart;
    segment.dataEnd   = _kaiju1SegmentDataEnd;
    segment.bssStart  = _kaiju1SegmentBssStart;
    segment.bssEnd    = _kaiju1SegmentBssEnd;
  } else if (levelNumber == 2) {
    segment.romStart  = _kaiju2SegmentRomStart;
    segment.romEnd    = _kaiju2SegmentRomEnd;
    segment.ramStart  = _kaiju2SegmentStart;
    segment.textStart = _kaiju2SegmentTextStart;
    segment.textEnd   = _kaiju2SegmentTextEnd;
    segment.dataStart = _kaiju2SegmentDataStart;
    segment.dataEnd   = _kaiju2SegmentDataEnd;
    segment.bssStart  = _kaiju2SegmentBssStart;
    segment.bssEnd    = _kaiju2SegmentBssEnd;
  } else if (levelNumber >= 3) {
    segment.romStart  = _kaiju3SegmentRomStart;
    segment.romEnd    = _kaiju3SegmentRomEnd;
    segment.ramStart  = _kaiju3SegmentStart;
    segment.textStart = _kaiju3SegmentTextStart;
    segment.textEnd   = _kaiju3SegmentTextEnd;
    segment.dataStart = _kaiju3SegmentDataStart;
    segment.dataEnd   = _kaiju3SegmentDataEnd;
    segment.bssStart  = _kaiju3SegmentBssStart;
    segment.bssEnd    = _kaiju3SegmentBssEnd;
  }

  nuPiReadRomOverlay(&segment);

  if (levelNumber == 0) {
    initKaijuCallback = &initKaiju0;
    updateKaijuCallback = &updateKaiju0;
    renderKaijuCallback = &renderKaiju0;
  } else if (levelNumber == 1) {
    initKaijuCallback = &initKaiju1;
    updateKaijuCallback = &updateKaiju1;
    renderKaijuCallback = &renderKaiju1;
  } else if (levelNumber == 2) {
    initKaijuCallback = &initKaiju2;
    updateKaijuCallback = &updateKaiju2;
    renderKaijuCallback = &renderKaiju2;
  } else if (levelNumber >= 3) {
    initKaijuCallback = &initKaiju3;
    updateKaijuCallback = &updateKaiju3;
    renderKaijuCallback = &renderKaiju3;
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

void loadInIntroState() {
  NUPiOverlaySegment segment;

  segment.romStart  = _introcardstageSegmentRomStart;
  segment.romEnd    = _introcardstageSegmentRomEnd;
  segment.ramStart  = _introcardstageSegmentStart;
  segment.textStart = _introcardstageSegmentTextStart;
  segment.textEnd   = _introcardstageSegmentTextEnd;
  segment.dataStart = _introcardstageSegmentDataStart;
  segment.dataEnd   = _introcardstageSegmentDataEnd;
  segment.bssStart  = _introcardstageSegmentBssStart;
  segment.bssEnd    = _introcardstageSegmentBssEnd;

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

void loadInCreditsState() {
  NUPiOverlaySegment segment;

  segment.romStart  = _creditsscreenSegmentRomStart;
  segment.romEnd    = _creditsscreenSegmentRomEnd;
  segment.ramStart  = _creditsscreenSegmentStart;
  segment.textStart = _creditsscreenSegmentTextStart;
  segment.textEnd   = _creditsscreenSegmentTextEnd;
  segment.dataStart = _creditsscreenSegmentDataStart;
  segment.dataEnd   = _creditsscreenSegmentDataEnd;
  segment.bssStart  = _creditsscreenSegmentBssStart;
  segment.bssEnd    = _creditsscreenSegmentBssEnd;

  nuPiReadRomOverlay(&segment);
}

void loadInRetryState() {
  NUPiOverlaySegment segment;

  segment.romStart  = _retryscreenSegmentRomStart;
  segment.romEnd    = _retryscreenSegmentRomEnd;
  segment.ramStart  = _retryscreenSegmentStart;
  segment.textStart = _retryscreenSegmentTextStart;
  segment.textEnd   = _retryscreenSegmentTextEnd;
  segment.dataStart = _retryscreenSegmentDataStart;
  segment.dataEnd   = _retryscreenSegmentDataEnd;
  segment.bssStart  = _retryscreenSegmentBssStart;
  segment.bssEnd    = _retryscreenSegmentBssEnd;

  nuPiReadRomOverlay(&segment);
}

void loadInFMVScreenState() {
  NUPiOverlaySegment segment;

  segment.romStart  = _fmvSegmentRomStart;
  segment.romEnd    = _fmvSegmentRomEnd;
  segment.ramStart  = _fmvSegmentStart;
  segment.textStart = _fmvSegmentTextStart;
  segment.textEnd   = _fmvSegmentTextEnd;
  segment.dataStart = _fmvSegmentDataStart;
  segment.dataEnd   = _fmvSegmentDataEnd;
  segment.bssStart  = _fmvSegmentBssStart;
  segment.bssEnd    = _fmvSegmentBssEnd;

  nuPiReadRomOverlay(&segment);
}

// Coped from nuAuInit
#define SMALLER_AU_HEAP_SIZE (200000 - 4150)
#define SMALLER_AU_HEAP_ADDR (NU_GFX_FRAMEBUFFER_ADDR - 200000)
s32 smallerHeapAuinit(void)
{
    /* Initialize the Audio Manager.  */
    nuAuMgrInitEx((void*)SMALLER_AU_HEAP_ADDR, SMALLER_AU_HEAP_SIZE, &nuAuSynConfig);

    /* Initialize the Sequence Player.  */
    nuAuSeqPlayerInit(&nuAuSeqpConfig, 0x8000, NU_AU_SEQ_PLAYER0);

    /* Initialize the Sequence Player.  */
    nuAuSeqPlayerInit(&nuAuSeqpConfig, 0x8000, NU_AU_SEQ_PLAYER1);

    /* Initialize the Sound Player. */
    nuAuSndPlayerInit(&nuAuSndpConfig);

    /* Initialize the audio control callback function. */
    nuAuMgrFuncSet(nuAuSeqPlayerControl);

    /* Register the PRE NMI processing function.  */
    nuAuPreNMIFuncSet(nuAuPreNMIProc);

    /* Return the size of the heap area used. */
    return nuAuHeapGetUsed();
}

void initAudio(void)
{
  smallerHeapAuinit();

  nuAuSeqPlayerBankSet(_audiobank_controlSegmentRomStart, _audiobank_controlSegmentRomEnd - _audiobank_controlSegmentRomStart, _audiobank_tableSegmentRomStart);
  nuAuSeqPlayerSeqSet(_songsSegmentRomStart);
  // nuAuSeqPlayerSetVol(0, 0x6A0F);

  nuAuSndPlayerBankSet(_sfxbank_controlSegmentRomStart, _sfxbank_controlSegmentRomEnd - _sfxbank_controlSegmentRomStart, _sfxbank_tableSegmentRomStart);
}

// #define PAL_ROM

#ifdef PAL_ROM
void callback_prenmi()
{
    osViSetYScale(1);
    nuGfxDisplayOff();
}

#endif

/*------------------------
	Main
--------------------------*/
void mainproc(void)
{
  /* The initialization of graphic  */
  // nuGfxInit();
  hvqmGfxInit();

#ifdef PAL_ROM
  osViSetMode(&osViModeTable[OS_VI_FPAL_LAN1]);
  osViSetYScale(0.833); // Dirty but works
  nuPreNMIFuncSet((NUScPreNMIFunc)callback_prenmi);
#endif

  /* The initialization of the controller manager  */
  contPattern = nuContInit();
  changeScreensFlag = 0;

  screenType = FMVScreen;

  loadInFMVScreenState();
  initFMVStage();
  nuGfxFuncSet((NUGfxFunc)fmvtick);

  nuGfxDisplayOn();

  while(!changeScreensFlag);

  nuGfxFuncRemove();

  initAudio();

  screenType = TitleScreen;
  currentLevel = 0;
  finishedLevel = 0;

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
    } else if (screenType == IntroCardScreen) {
      loadInIntroState();
      initIntroScreen();
      nuGfxFuncSet((NUGfxFunc)introcard);
    } else if (screenType == CreditsScreen) {
      loadInCreditsState();
      initCreditscreen();
      nuGfxFuncSet((NUGfxFunc)credits);
    } else if (screenType == RetryScreen) {
      loadInRetryState();
      initRetryScreen();
      nuGfxFuncSet((NUGfxFunc)retry);
    }

    nuGfxDisplayOn();

    while(!changeScreensFlag);

    nuGfxFuncRemove();
    nuGfxDisplayOff();

    if (currentLevel == NUMBER_OF_LEVELS) {
      currentLevel = 0;
      screenType = CreditsScreen;
    }

    changeScreensFlag = 0;
  }
}

void stage00(int pendingGfx)
{
  if (changeScreensFlag != 0) {
    return;
  }

  if(pendingGfx < 3)
    makeDL00();		

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


void introcard(int pendingGfx)
{
  if (changeScreensFlag != 0) {
    return;
  }

  if(pendingGfx < 3) {
    makeDLIntroScreen();   
  }

  updateIntroScreen(); 
}

void fmvtick(int pendingGfx)
{
  if (changeScreensFlag != 0) {
    return;
  }

  if(pendingGfx < 3) {
    makeFMVDL();   
  }

  updateFMVStage(); 
}

void credits(int pendingGfx)
{
  if (changeScreensFlag != 0) {
    return;
  }

  if(pendingGfx < 3) {
    makeDLCreditsScreen();   
  }

  updateCreditsScreen(); 
}

void retry(int pendingGfx)
{
  if (changeScreensFlag != 0) {
    return;
  }

  if(pendingGfx < 3) {
    makeDLRetryScreen();   
  }

  updateRetryScreen(); 
}

