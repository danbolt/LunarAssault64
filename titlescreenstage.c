
#include "titlescreenstage.h"

#include <nusys.h>
#include <nualsgi_n.h>
#include "main.h"
#include "soundid.h"
#include "segmentinfo.h"
#include "smallfont.h"

#define FADE_IN_TIME 1.2f
#define FADE_OUT_TIME 1.2f

#define STICK_Y_DEADZONE 20

char testBuf[128];

u8 titleScreenBackgroundBuffer[153600] __attribute__((aligned(8)));
float fadeTimePassed;
u32 isFading = 0;
u32 isFadingOut = 0;

static OSTime time = 0;
static OSTime delta = 0;

#define NUMBER_OF_MENU_ITEMS 2
u32 menuIndex = 0;

const char* menuStrings[] = {
	" Start ",
	"Credits"
};

u32 stickPressed = 0;

void initTitleScreen(void) {
	// nuAuSeqPlayerStop(0);
	// nuAuSeqPlayerSetNo(0, 0);
	// nuAuSeqPlayerPlay(0);

	nuPiReadRom((u32)_title_screen_bgSegmentRomStart, titleScreenBackgroundBuffer, 153600);

	fadeTimePassed = 0.f;
	isFading = 1;
	isFadingOut = 0;

	nuAuSndPlayerPlay(SOUND_INTRO);

    time = OS_CYCLES_TO_USEC(osGetTime());
	delta = 0;

	menuIndex = 0;
	stickPressed = 0;
}


void makeDLTitleScreen(void) {
	DisplayData* dynamicp;
	char conbuf[20];
	int i;

	dynamicp = &gfx_dynamic[gfx_gtask_no];
	glistp = &gfx_glist[gfx_gtask_no][0];

	gfxRCPInit();
	gfxClearCfb();

	if (!isFadingOut) {
		gDPSetColorImage(glistp++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WD, osVirtualToPhysical(nuGfxCfb_ptr));
		gDPSetFillColor(glistp++, (GPACK_RGBA5551(255, 255, 255, 1) << 16 | GPACK_RGBA5551(255, 255, 255, 1)));
		gDPFillRectangle(glistp++, 0, 0, SCREEN_WD-1, SCREEN_HT-1);
		gDPPipeSync(glistp++);
	}

	gDPSetCycleType(glistp++, G_CYC_1CYCLE);
    gDPSetCombineMode(glistp++, G_CC_SHADE, G_CC_SHADE);
    gDPSetScissor(glistp++, G_SC_NON_INTERLACE, 0, 0, SCREEN_WD - 1, SCREEN_HT - 1);
    gDPSetRenderMode(glistp++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
	gDPPipeSync(glistp++);
    gSPClearGeometryMode(glistp++,0xFFFFFFFF);
    gSPSetGeometryMode(glistp++, G_SHADE | G_SHADING_SMOOTH | G_CULL_BACK);

    guOrtho(&(dynamicp->orthoHudProjection), 0, SCREEN_WD - 1, 0, SCREEN_HT - 1, 0.f, 10.f, 1.f);
    gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->orthoHudProjection)), G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    guMtxIdent(&(dynamicp->orthoHudModelling));
    gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->orthoHudModelling)), G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);

	gSPTexture(glistp++, 0xffff, 0xffff, 0, G_TX_RENDERTILE, G_ON);
	gDPPipeSync(glistp++);

	if (isFading) {
		int tVal;
		float t = (fadeTimePassed / FADE_IN_TIME);

		tVal = t * 255;
		gDPSetPrimColor(glistp++, 0, 0, (255), (255), (255), (tVal));

		gDPSetCombineMode(glistp++,G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM);
	} else if (isFadingOut) {
		int tVal;
		float t = 1.f - (fadeTimePassed / FADE_OUT_TIME);

		tVal = t * 255;
		gDPSetPrimColor(glistp++, 0, 0, tVal, tVal, tVal, tVal);
		gDPSetCombineMode(glistp++,G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM);
	} else {
		gDPSetCombineMode(glistp++,G_CC_DECALRGBA, G_CC_DECALRGBA);
	}


	gDPSetTextureFilter(glistp++, G_TF_BILERP);
	gDPSetRenderMode(glistp++, G_RM_XLU_SURF, G_RM_XLU_SURF);
	gDPSetTexturePersp(glistp++, G_TP_NONE);
	gDPPipeSync(glistp++);

	for (i = 0; i < (240 / 6); i++) {
		gDPLoadTextureTile(glistp++, titleScreenBackgroundBuffer, G_IM_FMT_RGBA, G_IM_SIZ_16b, 320, 240, 0, (i * 6), 320 - 1, ((i + 1) * 6) - 1, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD );
		gSPScisTextureRectangle(glistp++, 0 << 2, (0 + (i * 6)) << 2, (0 + 320) << 2, (0 + ((i + 1) * 6)) << 2, 0, 0 << 5, (i * 6) << 5, 1 << 10, 1 << 10);
	}

	if (!isFading && !isFadingOut) {
		for (i = 0; i < NUMBER_OF_MENU_ITEMS; i++) {
			if (i == menuIndex) {
				sprintf(testBuf, ">> %s <<", menuStrings[i]);
			} else {
				sprintf(testBuf, "   %s   ", menuStrings[i]);
			}
			
			drawSmallStringCol( (20 - 6) * 8, (20 + i) * 8, testBuf, 0, 0, 0);
		}
		
		drawSmallStringCol( 3 * 8, 25 * 8, "N64Brew Game Jam Submission", 0, 0, 0);
		drawSmallStringCol( 3 * 8, 26 * 8, "https://danbolt.itch.io/", 0, 0, 0);
	}
	

	gDPFullSync(glistp++);
	gSPEndDisplayList(glistp++);

	nuGfxTaskStart(&gfx_glist[gfx_gtask_no][0], (s32)(glistp - gfx_glist[gfx_gtask_no]) * sizeof (Gfx), NU_GFX_UCODE_F3DLP_REJ , NU_SC_SWAPBUFFER);

	// nuDebConClear(0);
	// nuDebConTextColor(0, NU_DEB_CON_TEXT_BLACK);
	// if(contPattern & 0x1)
 //    {
		
	// }
	// else
	// {
	// 	nuDebConTextPos(0,3,30);
	// 	nuDebConCPuts(0, "Connect controller #1, kid!");
	// }

	// nuDebConDisp(NU_SC_SWAPBUFFER);

	gfx_gtask_no = (gfx_gtask_no + 1) % 3;
}

void updateTitleScreen(void) {
	OSTime newTime = OS_CYCLES_TO_USEC(osGetTime());
	float deltaSeconds = 0.f;

	nuContDataGetEx(contdata,0);

	delta = newTime - time;
	time = newTime;
	deltaSeconds = delta * 0.000001f;

	if (isFading) {
		fadeTimePassed += deltaSeconds;

		if (fadeTimePassed > FADE_IN_TIME) {
			fadeTimePassed = FADE_IN_TIME;
			isFading = 0;
		}
	} else if (isFadingOut) {
		fadeTimePassed += deltaSeconds;

		if (fadeTimePassed > FADE_OUT_TIME) {
			nuAuSeqPlayerStop(0);
			changeScreensFlag = 1;

			if (menuIndex == 0) {
				screenType = IntroCardScreen;
				currentLevel = 0;
			} else if (menuIndex == 1) {
				screenType = CreditsScreen;
			}
		}
	} else {

		if ((contdata->trigger & D_JPAD)) {
			menuIndex = (menuIndex + 1) % NUMBER_OF_MENU_ITEMS;
			nuAuSndPlayerPlay(SOUND_PLAYER_BIP);
		} else if ((contdata->trigger & U_JPAD)) {
			menuIndex = (menuIndex - 1 + NUMBER_OF_MENU_ITEMS) % NUMBER_OF_MENU_ITEMS;
			nuAuSndPlayerPlay(SOUND_PLAYER_BIP);
		}

		if (stickPressed) {
			if ((contdata->stick_y < STICK_Y_DEADZONE) && (contdata->stick_y > -STICK_Y_DEADZONE)) {
				stickPressed = 0;
			}
		} else {
			if (contdata->stick_y > STICK_Y_DEADZONE) {
				menuIndex = (menuIndex + 1) % NUMBER_OF_MENU_ITEMS;
				nuAuSndPlayerPlay(SOUND_PLAYER_BIP);
				stickPressed = 1;
			} else if (contdata->stick_y < -STICK_Y_DEADZONE) {
				menuIndex = (menuIndex - 1 + NUMBER_OF_MENU_ITEMS) % NUMBER_OF_MENU_ITEMS;
				nuAuSndPlayerPlay(SOUND_PLAYER_BIP);
				stickPressed = 1;
			}
		}

		if ((contdata->trigger & A_BUTTON) || ((contdata->trigger & START_BUTTON))) {
			

			isFadingOut = 1;
			fadeTimePassed = 0.f;

			nuAuSndPlayerPlay(SOUND_LASER1);
		}
	}
}
