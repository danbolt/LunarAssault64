
#include "retryscreen.h"

#include <nusys.h>
#include <nualsgi_n.h>
#include "main.h"
#include "soundid.h"
#include "smallfont.h"

#define NUMBER_OF_MENU_ITEMS 2
#define STICK_Y_DEADZONE 20

#define FADE_WHITE_TIME 1.5f

static OSTime time = 0;
static OSTime delta = 0;

static float timePassed = 0.f;

static int menuIndex = 0;
static int stickPressed = 0;

static float titleScreenFadeTime;
static int isFadingToTitle;

static char formattingBuffer[128];

void initRetryScreen(void) {
    time = OS_CYCLES_TO_USEC(osGetTime());
	delta = 0;

	timePassed = 0.f;

	menuIndex = 0;
	stickPressed = 0;

	isFadingToTitle = 0;
}


void makeDLRetryScreen(void) {
	DisplayData* dynamicp;
	char conbuf[20];
	int i;
	u32 len;
	u8 col = 255;

	dynamicp = &gfx_dynamic[gfx_gtask_no];
	glistp = &gfx_glist[gfx_gtask_no][0];

	gfxRCPInit();
	gfxClearCfb();

	if (isFadingToTitle) {
		u8 fadeVal = (titleScreenFadeTime / FADE_WHITE_TIME) * 255;
		gDPSetFillColor(glistp++, (GPACK_RGBA5551(fadeVal, fadeVal, fadeVal, 1) << 16 |  GPACK_RGBA5551(fadeVal, fadeVal, fadeVal, 1)));
  		gDPFillRectangle(glistp++, 0, 0, SCREEN_WD-1, SCREEN_HT-1);
	}

	gDPSetCycleType(glistp++, G_CYC_1CYCLE);
    gDPSetCombineMode(glistp++, G_CC_SHADE, G_CC_SHADE);
    gDPSetScissor(glistp++, G_SC_NON_INTERLACE, 0, 0, SCREEN_WD - 1, SCREEN_HT - 1);
    gDPSetTextureFilter(glistp++, G_TF_BILERP);
	gDPSetRenderMode(glistp++, G_RM_XLU_SURF, G_RM_XLU_SURF);
	gDPSetTexturePersp(glistp++, G_TP_NONE);
	gDPPipeSync(glistp++);
	gSPTexture(glistp++, 0xffff, 0xffff, 0, G_TX_RENDERTILE, G_ON);
    gSPClearGeometryMode(glistp++,0xFFFFFFFF);
    gSPSetGeometryMode(glistp++, G_SHADE | G_SHADING_SMOOTH | G_CULL_BACK);

    if (!isFadingToTitle) {
		if (timePassed > 1.2f) {
			sprintf(formattingBuffer, "You too too long!");
			drawSmallStringCol(12 * 8, 8 * 8, formattingBuffer, col, col, col);
		}

		if (timePassed > 2.4f) {
			sprintf(formattingBuffer, "You're likely fired.");
			drawSmallStringCol(10 * 8, 10 * 8, formattingBuffer, col, col, col);
		}

		if (timePassed > 3.8f) {
			sprintf(formattingBuffer, "Give it another try?");
			drawSmallStringCol(10 * 8, 12 * 8, formattingBuffer, col, col, col);
		}


		if (timePassed > 4.2f) {
			if (menuIndex == 0) {
				sprintf(formattingBuffer, "> Of course! < ");
				drawSmallStringCol(13 * 8, 20 * 8, formattingBuffer, col, col, col);
			}
			else {
				sprintf(formattingBuffer, "Of course!");
				drawSmallStringCol(15 * 8, 20 * 8, formattingBuffer, col, col, col);
			}
			nuDebConCPuts(0, conbuf);

			if (menuIndex == 0) {
				sprintf(formattingBuffer, "Ugh, another time.");
				drawSmallStringCol(11 * 8, 21 * 8, formattingBuffer, col, col, col);
			} else {
				sprintf(formattingBuffer, "> Ugh, another time. <");
				drawSmallStringCol(9 * 8, 21 * 8, formattingBuffer, col, col, col);
			}
		}
	}

	gDPFullSync(glistp++);
	gSPEndDisplayList(glistp++);

	nuGfxTaskStart(&gfx_glist[gfx_gtask_no][0], (s32)(glistp - gfx_glist[gfx_gtask_no]) * sizeof (Gfx), NU_GFX_UCODE_F3DLP_REJ , NU_SC_SWAPBUFFER);

	gfx_gtask_no = (gfx_gtask_no + 1) % 3;
}

void updateRetryScreen(void) {
	OSTime newTime = OS_CYCLES_TO_USEC(osGetTime());
	float deltaSeconds = 0.f;

	nuContDataGetEx(contdata,0);

	delta = newTime - time;
	time = newTime;
	deltaSeconds = delta * 0.000001f;

	timePassed += deltaSeconds;

	if (isFadingToTitle) {
		titleScreenFadeTime += deltaSeconds;

		if (titleScreenFadeTime > FADE_WHITE_TIME) {
			changeScreensFlag = 1;
			screenType = TitleScreen;
		}
	}

	if (!isFadingToTitle) {
		if ((contdata->trigger & A_BUTTON) || ((contdata->trigger & START_BUTTON))) {
			if (menuIndex == 0) {
				nuAuSndPlayerPlay(SOUND_PLAYER_BIP);
				changeScreensFlag = 1;
				screenType = StageScreen;
			} else {
				nuAuSndPlayerPlay(SOUND_BOSS_BIP);
				titleScreenFadeTime = 0.f;
				isFadingToTitle = 1;
			}
			return;
		}

		if ((contdata->trigger & D_JPAD)) {
			menuIndex = (menuIndex + 1) % NUMBER_OF_MENU_ITEMS;
			nuAuSndPlayerPlay(SOUND_NOBODY_BIP);
		} else if ((contdata->trigger & U_JPAD)) {
			menuIndex = (menuIndex - 1 + NUMBER_OF_MENU_ITEMS) % NUMBER_OF_MENU_ITEMS;
			nuAuSndPlayerPlay(SOUND_NOBODY_BIP);
		}

		if (stickPressed) {
			if ((contdata->stick_y < STICK_Y_DEADZONE) && (contdata->stick_y > -STICK_Y_DEADZONE)) {
				stickPressed = 0;
			}
		} else {
			if (contdata->stick_y > STICK_Y_DEADZONE) {
				menuIndex = (menuIndex + 1) % NUMBER_OF_MENU_ITEMS;
				nuAuSndPlayerPlay(SOUND_NOBODY_BIP);
				stickPressed = 1;
			} else if (contdata->stick_y < -STICK_Y_DEADZONE) {
				menuIndex = (menuIndex - 1 + NUMBER_OF_MENU_ITEMS) % NUMBER_OF_MENU_ITEMS;
				nuAuSndPlayerPlay(SOUND_NOBODY_BIP);
				stickPressed = 1;
			}
		}
	}
}
