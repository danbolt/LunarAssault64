
#include "retryscreen.h"

#include <nusys.h>
#include <nualsgi_n.h>
#include "main.h"
#include "soundid.h"

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

void initRetryScreen(void) {
	currentLevel = 0;

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

	dynamicp = &gfx_dynamic[gfx_gtask_no];
	glistp = &gfx_glist[gfx_gtask_no][0];

	gfxRCPInit();
	gfxClearCfb();

	if (isFadingToTitle) {
		u8 fadeVal = (titleScreenFadeTime / FADE_WHITE_TIME) * 255;
		gDPSetFillColor(glistp++, (GPACK_RGBA5551(fadeVal, fadeVal, fadeVal, 1) << 16 |  GPACK_RGBA5551(fadeVal, fadeVal, fadeVal, 1)));
  		gDPFillRectangle(glistp++, 0, 0, SCREEN_WD-1, SCREEN_HT-1);
	}

	gDPFullSync(glistp++);
	gSPEndDisplayList(glistp++);

	nuGfxTaskStart(&gfx_glist[gfx_gtask_no][0], (s32)(glistp - gfx_glist[gfx_gtask_no]) * sizeof (Gfx), NU_GFX_UCODE_F3DLP_REJ , NU_SC_NOSWAPBUFFER);

	nuDebConClear(0);
	if (!isFadingToTitle) {
		nuDebConTextColor(0, NU_DEB_CON_TEXT_WHITE);
		
		if (timePassed > 1.2f) {
			nuDebConTextPos(0, 12, 8);
			sprintf(conbuf, "You too too long!");
			nuDebConCPuts(0, conbuf);
		}

		if (timePassed > 2.4f) {
			nuDebConTextPos(0, 10, 10);
			sprintf(conbuf, "You're likely fired.");
			nuDebConCPuts(0, conbuf);
		}

		if (timePassed > 3.8f) {
			nuDebConTextPos(0, 10, 12);
			sprintf(conbuf, "Give it another try?");
			nuDebConCPuts(0, conbuf);
		}


		if (timePassed > 4.2f) {
			if (menuIndex == 0) {
				nuDebConTextPos(0, 13, 20);
				sprintf(conbuf, "> Of course! < ");
			}
			else {
				nuDebConTextPos(0, 15, 20);
				sprintf(conbuf, "Of course!");
			}
			nuDebConCPuts(0, conbuf);

			if (menuIndex == 0) {
				nuDebConTextPos(0, 11, 21);
				sprintf(conbuf, "Ugh, another time.");
			} else {
				nuDebConTextPos(0, 9, 21);
				sprintf(conbuf, "> Ugh, another time. <");
			}
			nuDebConCPuts(0, conbuf);
		}
	}

	nuDebConDisp(NU_SC_SWAPBUFFER);

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
