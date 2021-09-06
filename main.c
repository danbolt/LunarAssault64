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
#include "smallfont.h"

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
void splash(int);

volatile int changeScreensFlag;
volatile ScreenSetting screenType;
volatile int currentLevel;
volatile u8 finishedLevel;

s8 cameraYInvert = -1;

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


static u8 splashScreenBuffer[37520] __attribute__((aligned(8)));
static OSTime splashTime = 0;
static OSTime splashStartTime = 0;
static u32 splashDeltaMS = 0;

#define LOGOTYPE_TEX_WIDTH 280
#define LOGOTYPE_TEX_HEIGHT 134

#define LOGO_X_POS ((SCREEN_WD - LOGOTYPE_TEX_WIDTH) / 2)
#define LOGO_Y_POS ((SCREEN_HT - LOGOTYPE_TEX_HEIGHT) / 2)

#define SPLASH_LEAD_IN 700
#define SPLASH_HOLD 1800
#define SPLASH_LEAD_OUT 400
#define SPLASH_EXTRA 300

void initSplashScreen() {
  changeScreensFlag = 0;
  splashStartTime = OS_CYCLES_TO_USEC(osGetTime());
  splashDeltaMS = 0;

  nuPiReadRom((u32)_logotype_rgbaSegmentRomStart, splashScreenBuffer, (u32)(37520));
}


void makeSplashDL(void) {
  DisplayData* dynamicp;
  char conbuf[20];
  int i;
  u32 len;
  int nudgeX = (guRandom() % 4);
  int nudgeY = (guRandom() % 4);

  dynamicp = &gfx_dynamic[gfx_gtask_no];
  glistp = &gfx_glist[gfx_gtask_no][0];

  gfxRCPInit();
  gfxClearCfb();


  gDPSetCycleType(glistp++, G_CYC_1CYCLE);
  gDPSetScissor(glistp++, G_SC_NON_INTERLACE, 0, 0, SCREEN_WD - 1, SCREEN_HT - 1);
  gDPSetTextureFilter(glistp++, G_TF_BILERP);
  gDPSetRenderMode(glistp++, G_RM_AA_XLU_SURF, G_RM_AA_XLU_SURF);
  gDPSetTexturePersp(glistp++, G_TP_NONE);


  if (splashDeltaMS < SPLASH_LEAD_IN) {
    float t = (splashDeltaMS / (float)SPLASH_LEAD_IN);
    gDPSetPrimColor(glistp++, 0, 0, (255), (255), (255), (t * 255));
    gDPSetCombineMode(glistp++, G_CC_MODULATEI_PRIM, G_CC_MODULATEI_PRIM);
  } else if (splashDeltaMS < (SPLASH_LEAD_IN + SPLASH_HOLD)) {
    gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);
  } else if (splashDeltaMS < (SPLASH_LEAD_IN + SPLASH_HOLD + SPLASH_LEAD_OUT)) {
    float t = 1.f - ((splashDeltaMS - (SPLASH_LEAD_IN + SPLASH_HOLD)) / (float)(SPLASH_LEAD_OUT));
    gDPSetPrimColor(glistp++, 0, 0, (255), (255), (255), (t * 255));
    gDPSetCombineMode(glistp++, G_CC_MODULATEI_PRIM, G_CC_MODULATEI_PRIM);
  } else {
    gDPSetPrimColor(glistp++, 0, 0, (255), (255), (255), 0);
    gDPSetCombineMode(glistp++, G_CC_MODULATEI_PRIM, G_CC_MODULATEI_PRIM);
  }

  
  guOrtho(&(dynamicp->orthoHudProjection), 0, SCREEN_WD - 1, 0, SCREEN_HT - 1, 0.f, 10.f, 1.f);
  gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->orthoHudProjection)), G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
  guMtxIdent(&(dynamicp->orthoHudModelling));
  gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->orthoHudModelling)), G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
  gDPPipeSync(glistp++);
  gSPTexture(glistp++, 0xffff, 0xffff, 0, G_TX_RENDERTILE, G_ON);
  gSPClearGeometryMode(glistp++,0xFFFFFFFF);
  gSPSetGeometryMode(glistp++, G_SHADE | G_SHADING_SMOOTH | G_CULL_BACK);

  for (i = 0; i < (LOGOTYPE_TEX_HEIGHT / 3); i++) {
    gDPLoadTextureTile(glistp++, splashScreenBuffer, G_IM_FMT_IA, G_IM_SIZ_8b, LOGOTYPE_TEX_WIDTH, LOGOTYPE_TEX_HEIGHT, 0, (i * 3), LOGOTYPE_TEX_WIDTH - 1, ((i + 1) * 3) - 1, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD );
    gSPTextureRectangle(glistp++, ((0 + LOGO_X_POS)) << 2, (LOGO_Y_POS + (i * 3)) << 2, ((0 + LOGO_X_POS) + LOGOTYPE_TEX_WIDTH) << 2, (LOGO_Y_POS + ((i + 1) * 3)) << 2, 0, ((0) << 5) | (nudgeX << 1), ((i * 3) << 5) | (nudgeY << 1), (1 << 10), (1 << 10));
  }


  gDPFullSync(glistp++);
  gSPEndDisplayList(glistp++);

  nuGfxTaskStart(&gfx_glist[gfx_gtask_no][0], (s32)(glistp - gfx_glist[gfx_gtask_no]) * sizeof (Gfx), NU_GFX_UCODE_F3DLP_REJ , NU_SC_SWAPBUFFER);

  gfx_gtask_no = (gfx_gtask_no + 1) % 3;
}

void updateSplashScreen(void) {

  OSTime delta;
  splashTime = OS_CYCLES_TO_USEC(osGetTime());

  guRandom();

  delta = splashTime - splashStartTime;
  splashDeltaMS = delta / 1000;

  if (splashDeltaMS > (SPLASH_LEAD_IN + SPLASH_HOLD + SPLASH_LEAD_OUT + SPLASH_EXTRA)) {
    changeScreensFlag = 1;
  }
}

void splash(int pendingGfx)
{
  if (changeScreensFlag != 0) {
    return;
  }

  if(pendingGfx < 3)
    makeSplashDL();   

  updateSplashScreen(); 
}


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

  initSplashScreen();

  nuGfxFuncSet((NUGfxFunc)splash);

  nuGfxDisplayOn();

  while (!changeScreensFlag);

  nuGfxFuncRemove();

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

