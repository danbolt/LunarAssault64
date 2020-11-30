
#include <nusys.h>
#include <nualsgi_n.h>

#include "dialoguestage.h"

#include "main.h"
#include "font.h"
#include "graphic.h"
#include "gamemath.h"
#include "segmentinfo.h"
#include "soundid.h"

#include "doc/protag_portrait.h"
#include "doc/boss_portrait.h"

#define FONT_WIDTH 8
#define FONT_HALF_HEIGHT 8
#define FONT_HEIGHT 16

#define ASCII_SPACE 32

#define TIME_PER_LETTER 0.055f

#define PROTAG_PORTRAIT_WIDTH 98
#define PROTAG_PORTRAIT_HEIGHT 211

#define BOSS_PORTRAIT_WIDTH 132
#define BOSS_PORTRAIT_HEIGHT 206

extern unsigned char lab_bg_bin[];
extern unsigned int lab_bg_bin_len;

DialogueLine closing = { "And the work began.", NULL, NOBODY_THERE };

DialogueLine prologueIntro14 = { "Whew.\nOkay, let's give it\na go!", NULL, JUST_PROTAG_THERE };
DialogueLine prologueIntro13 = { "8. DON'T SCREW UP.", &prologueIntro14, JUST_PROTAG_THERE_BUT_NO_PEOPLE_SPEAKING };
DialogueLine prologueIntro12 = { "7. DON'T RUN OUT OF\nTIME.", &prologueIntro13, JUST_PROTAG_THERE_BUT_NO_PEOPLE_SPEAKING };
DialogueLine prologueIntro11 = { "6. HAVE THE SATELLITE\nSTRIKE THE RED WEAK\nPOINTS.", &prologueIntro12, JUST_PROTAG_THERE_BUT_NO_PEOPLE_SPEAKING };
DialogueLine prologueIntro10 = { "5. WHILE HOLDING L,\nHOLD Z TO CHARGE THE\nSATELLITE", &prologueIntro11, JUST_PROTAG_THERE_BUT_NO_PEOPLE_SPEAKING };
DialogueLine prologueIntro9 = { "4. LOOK THROUGH YOUR\nSPOTTER SCOPE WITH\nTHE L BUTTON.", &prologueIntro10, JUST_PROTAG_THERE_BUT_NO_PEOPLE_SPEAKING };
DialogueLine prologueIntro8 = { "3. JUMP WITH THE BLUE\nA BUTTON. THIS CAN\nHELP YOU CLIMB STUFF.", &prologueIntro9, JUST_PROTAG_THERE_BUT_NO_PEOPLE_SPEAKING };
DialogueLine prologueIntro7 = { "3. TURN WITH THE\nJOYSTICK. LIKE A\nMODERN SHOOTER GAME.", &prologueIntro8, JUST_PROTAG_THERE_BUT_NO_PEOPLE_SPEAKING };
DialogueLine prologueIntro6 = { "2. MOVE WITH THE\nDIRECTIONAL PAD.", &prologueIntro7, JUST_PROTAG_THERE_BUT_NO_PEOPLE_SPEAKING };
DialogueLine prologueIntro5 = { "1. HOLD THE LEFT AND\nMIDDLE HANDLES OF THE\nN64 CONTROLLER.", &prologueIntro6, JUST_PROTAG_THERE_BUT_NO_PEOPLE_SPEAKING };
DialogueLine prologueIntro4 = { "I'm just going to\nreview the controls\nfirst:", &prologueIntro5, JUST_PROTAG_THERE };
DialogueLine prologueIntro3 = { "How about a round on\nthe simulator to\npractice?", &prologueIntro4, JUST_PROTAG_THERE };
DialogueLine prologueIntro2 = { "I'm so excited, but\nI'm a little nervous\nas well.", &prologueIntro3, JUST_PROTAG_THERE };
DialogueLine prologueIntro1 = { "Alright, I start my\nfirst day tomorrow\nas a lunar spotter!", &prologueIntro2, JUST_PROTAG_THERE };
DialogueLine prologueIntro0 = { "June 10\nLunar Residence 14", &prologueIntro1, NOBODY_THERE };

DialogueLine level01Intro18 = { "Okay, then go get it.", &closing, BOSS_SPEAKING };
DialogueLine level01Intro17 = { "Of course!", &level01Intro18, PROTAG_SPEAKING };
DialogueLine level01Intro16 = { "Are you passionate?", &level01Intro17, BOSS_SPEAKING };
DialogueLine level01Intro15 = { "Yeah, I think I\nshould be able to hit\nits weak spots.", &level01Intro16, PROTAG_SPEAKING };
DialogueLine level01Intro14 = { "Anyway, we need you\nto kill it. Can you\ndo that?", &level01Intro15, BOSS_SPEAKING };
DialogueLine level01Intro13 = { "Uh, yeah, sure...", &level01Intro14, BOSS_SPEAKING };
DialogueLine level01Intro12 = { "Wow, that's a lot for\na Mark VI. Especially\nduring this equinox.", &level01Intro13, PROTAG_SPEAKING };
DialogueLine level01Intro11 = { "Intel indicates that\nit wields the despair\nof 99 remote workers.", &level01Intro12, BOSS_SPEAKING };
DialogueLine level01Intro10 = { "Listen, a Mark VI\nLunarbeast appeared\nnear the 19th suburb.", &level01Intro11, BOSS_SPEAKING };
DialogueLine level01Intro9 = { "Oh, okay! Sorry!", &level01Intro10, PROTAG_SPEAKING };
DialogueLine level01Intro8 = { "Uhhh yeah, I'm goona\nstop you there, kid.", &level01Intro9, BOSS_SPEAKING };
DialogueLine level01Intro7 = { "Or how the weak spots\nare shadows of a\n5-dimensional heart--", &level01Intro8, PROTAG_SPEAKING };
DialogueLine level01Intro6 = { "Like how the laser\nionizes quantum light\nto stun the beasts.", &level01Intro7, PROTAG_SPEAKING };
DialogueLine level01Intro5 = { "I love learning\nhow the lunar array\nworks. It's so cool!", &level01Intro6, PROTAG_SPEAKING };
DialogueLine level01Intro4 = { "Yep! That's me! I'm\nhere to be a Lunar\nSpotter.", &level01Intro5, PROTAG_SPEAKING };
DialogueLine level01Intro3 = { "You're Wren, right?\nWren Buster?", &level01Intro4, BOSS_SPEAKING };
DialogueLine level01Intro2 = { "...whew,\nalright.\n...okay.", &level01Intro3, BOSS_SPEAKING };
DialogueLine level01Intro1 = { "...", &level01Intro2, JUST_PROTAG_THERE };
DialogueLine level01Intro0 = { "June 11\nLunar Fufillment Site\n(post-orientation)", &level01Intro1, NOBODY_THERE };

DialogueLine level02Intro20 = { "Got it.", &closing, PROTAG_SPEAKING };
DialogueLine level02Intro19 = { "Then please go kill\nit.", &level02Intro20, BOSS_SPEAKING };
DialogueLine level02Intro18 = { "Yeah, um, for sure.", &level02Intro19, PROTAG_SPEAKING };
DialogueLine level02Intro17 = { "Okay, whatever. Are\nyou passionate about\nthe job?", &level02Intro18, BOSS_SPEAKING };
DialogueLine level02Intro16 = { "I read that their\ntrue form exists only\nin infrared shadows.", &level02Intro17, PROTAG_SPEAKING };
DialogueLine level02Intro15 = { "Uhh...\nI guess not?", &level02Intro16, BOSS_SPEAKING };
DialogueLine level02Intro14 = { "A Mark II!? I thought\nthose were all gone\nnow.", &level02Intro15, PROTAG_SPEAKING };
DialogueLine level02Intro13 = { "Anyway, intel found\nthat it's caused by\na Mark II's rampage.", &level02Intro14, BOSS_SPEAKING };
DialogueLine level02Intro12 = { "Huh.", &level02Intro13, PROTAG_SPEAKING };
DialogueLine level02Intro11 = { "I guess. Gotta get\nasses back in theatre\nseats on Earth.", &level02Intro12, BOSS_SPEAKING };
DialogueLine level02Intro10 = { "Another one? Like for\nmovie production?", &level02Intro11, PROTAG_SPEAKING };
DialogueLine level02Intro9 = { "I think they neeeded\nthe power for another\nCG render farm.", &level02Intro10, BOSS_SPEAKING };
DialogueLine level02Intro8 = { "I thought the plant\nwas retired to stop\nthe Moonquakes.", &level02Intro9, PROTAG_SPEAKING };
DialogueLine level02Intro7 = { "Power's gone down at\nthe geothermal energy\nplant in the south.", &level02Intro8, BOSS_SPEAKING };
DialogueLine level02Intro6 = { "Okay, good. Let's get\ndown to it.", &level02Intro7, BOSS_SPEAKING };
DialogueLine level02Intro5 = { "Umm... it's fine!", &level02Intro6, PROTAG_SPEAKING };
DialogueLine level02Intro4 = { "Where you waiting\nlong?", &level02Intro5, BOSS_SPEAKING };
DialogueLine level02Intro3 = { "Hey, sorry I'm late.\nThe last meeting went\novertime.", &level02Intro4, BOSS_SPEAKING };
DialogueLine level02Intro2 = { "I'm still sore from\nthe array's positron\nannealing.", &level02Intro3, JUST_PROTAG_THERE };
DialogueLine level02Intro1 = { "Ugh, that last\nLunarbeast was a\ndoozy!", &level02Intro2, JUST_PROTAG_THERE };
DialogueLine level02Intro0 = { "September 26\nLunar Fufillment Site\n(a long Tuesday)", &level02Intro1, NOBODY_THERE };

DialogueLine* stageDialogues[NUMBER_OF_LEVELS] = {
	&prologueIntro0,
	&level01Intro0,
	&level02Intro0
};


DialogueLine prologueOutro2 = { "I feel like I'm in a\ngood spot for my\nfirst day tomorrow.", NULL, JUST_PROTAG_THERE };
DialogueLine prologueOutro1 = { "Whew!\nThat was tricky,\nbut fun!", &prologueOutro2, JUST_PROTAG_THERE };

DialogueLine level01Outro17 = { "(everyone went home)", NULL, NOBODY_THERE };
DialogueLine level01Outro16 = { "Oh, yeah, sure thing!", &level01Outro17, JUST_PROTAG_THERE };
DialogueLine level01Outro15 = { "From here on out, be\nback here 9AM every\nmorning. Earth time.", &level01Outro16, BOSS_SPEAKING };
DialogueLine level01Outro14 = { "I dunno.", &level01Outro15, BOSS_SPEAKING };
DialogueLine level01Outro13 = { "...", &level01Outro14, BOSS_SPEAKING };
DialogueLine level01Outro12 = { "Do you know how it\ncan do that?", &level01Outro13, PROTAG_SPEAKING };
DialogueLine level01Outro11 = { "Especially given how\nit ionizes the weak\nspot's outer shell.", &level01Outro12, PROTAG_SPEAKING };
DialogueLine level01Outro10 = { "The Satellite Array\nis so precise!", &level01Outro11, PROTAG_SPEAKING };
DialogueLine level01Outro9 = { "Thanks! It was inter-\nesting!", &level01Outro10, PROTAG_SPEAKING };
DialogueLine level01Outro8 = { "Good work kid.", &level01Outro9, BOSS_SPEAKING };
DialogueLine level01Outro7 = { "DLC revenue should\ngo up by 0.3% if\nestimates are right.", &level01Outro8, BOSS_SPEAKING };
DialogueLine level01Outro6 = { "That should keep the\nhousing supply up for\nwhen we hit Q3.", &level01Outro7, BOSS_SPEAKING };
DialogueLine level01Outro5 = { "...it felt a little\nunnerving at times.", &level01Outro6, PROTAG_SPEAKING };
DialogueLine level01Outro4 = { "I've never seen a\nLunarbeast so close\nbefore!", &level01Outro5, PROTAG_SPEAKING };
DialogueLine level01Outro3a = { "Okay, nice. Looks\nlike the thing's been\nkilled.", &level01Outro4, BOSS_SPEAKING };
DialogueLine level01Outro3 = { "(..yeah, I'll call\nyou back)\n*CLICK*", &level01Outro3a, BOSS_SPEAKING };
DialogueLine level01Outro2 = { "You could really feel\nthe agony of a video\ncall over bad Wi-Fi.", &level01Outro3, JUST_PROTAG_THERE };
DialogueLine level01Outro1 = { "Wow. That was\nsomething else. I'm\nglad it's over with.", &level01Outro2, JUST_PROTAG_THERE };
DialogueLine level01Outro0 = { "June 11\nLunar Fufillment Site\n(post-slaying)", &level01Outro1, NOBODY_THERE };


DialogueLine* stagePostDialogues[NUMBER_OF_LEVELS] = {
	&prologueOutro1,
	&level01Outro0,
	&closing
};

#define FADE_IN_OUT_DURATION 1.3f

static int isFading = 0;
static float fadeTime = 0.f;
static int isFadingOut = 0;

static OSTime time = 0;
static OSTime delta = 0;

static int letterIndex = 0;
static DialogueLine* currentLine = NULL;
static int tickingText = 0;
static float textTime;

vec2 portratPositions[2];
vec2 portratTargetSpots[2];

void refreshTargetSpots(int speakerIndex) {
	if (speakerIndex == NOBODY_THERE) {
		portratTargetSpots[0].x = -BOSS_PORTRAIT_WIDTH;
		portratTargetSpots[0].y = 240;
		portratTargetSpots[1].x = 320;
		portratTargetSpots[1].y = 240;
	} else if (speakerIndex == BOSS_SPEAKING) {
		portratTargetSpots[0].x = 0;
		portratTargetSpots[0].y = (240 - (BOSS_PORTRAIT_HEIGHT - 32));
		portratTargetSpots[1].x = (320 - (PROTAG_PORTRAIT_WIDTH - 32));
		portratTargetSpots[1].y = (240 - (PROTAG_PORTRAIT_HEIGHT - 64));
	} else if (speakerIndex == PROTAG_SPEAKING) {
		portratTargetSpots[0].x = -32;
		portratTargetSpots[0].y = (240 - (BOSS_PORTRAIT_HEIGHT - 64));
		portratTargetSpots[1].x = (320 - (PROTAG_PORTRAIT_WIDTH - 0));
		portratTargetSpots[1].y = (240 - (PROTAG_PORTRAIT_HEIGHT - 32));
	} else if (speakerIndex == JUST_PROTAG_THERE) {
		portratTargetSpots[0].x = -BOSS_PORTRAIT_WIDTH;
		portratTargetSpots[0].y = 240;
		portratTargetSpots[1].x = (320 - (PROTAG_PORTRAIT_WIDTH - 0));
		portratTargetSpots[1].y = (240 - (PROTAG_PORTRAIT_HEIGHT - 32));
	} else if (speakerIndex == JUST_PROTAG_THERE_BUT_NO_PEOPLE_SPEAKING) {
		portratTargetSpots[0].x = -BOSS_PORTRAIT_WIDTH;
		portratTargetSpots[0].y = 240;
		portratTargetSpots[1].x = (320 - (PROTAG_PORTRAIT_WIDTH - 0));
		portratTargetSpots[1].y = (240 - (PROTAG_PORTRAIT_HEIGHT - 32));
	}


}

void initDialogue(void) {
	letterIndex = 0;
	currentLine = finishedLevel ? stagePostDialogues[currentLevel] : stageDialogues[currentLevel];
	tickingText = 1;
	textTime = 0.f;

	isFading = 1;
	fadeTime = 0.f;
	isFadingOut = 0;

    time = OS_CYCLES_TO_USEC(osGetTime());
	delta = 0;

	if (currentLevel == 0) {
		nuPiReadRom((u32)_player_room_bgSegmentRomStart, lab_bg_bin, lab_bg_bin_len);
	} else {
		nuPiReadRom((u32)_base_bgSegmentRomStart, lab_bg_bin, lab_bg_bin_len);
	}

	portratPositions[0].x = -BOSS_PORTRAIT_WIDTH;
	portratPositions[0].y = 240;
	portratPositions[1].x = 320;
	portratPositions[1].y = 240;
	refreshTargetSpots(currentLine->speakerIndex);

	nuAuSeqPlayerStop(0);
	nuAuSeqPlayerSetNo(0, 4);
	nuAuSeqPlayerPlay(0);
}

void getCharST(const char* character, int* s, int* t) {
	if (*character < ASCII_SPACE) {
		*s = 120;
		*t = 8;
		return;
	}

	*s = (((int)((*character) - ASCII_SPACE)) % 16) * FONT_HALF_HEIGHT;
	*t = (((int)((*character) - ASCII_SPACE)) / 16) * FONT_HALF_HEIGHT;
}

void drawString_impl(int x, int y, const char* str, int cutoff) {
	int i = 0;
	int xPos = x;
	int yPos = y;

	gDPLoadTextureBlock_4b(glistp++, font_bin, G_IM_FMT_IA, 128, 64, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
	while (str[i] != '\0') {
		int s;
		int t;

		if ((cutoff > -1) && (i == cutoff)) {
			break;
		}

		if (str[i] == ' ') {
			xPos += FONT_WIDTH;
			i++;
			continue;
		}

		if (str[i] == '\n') {
			xPos = x;
			yPos += FONT_HEIGHT;
			i++;
			continue;
		}

		getCharST(&(str[i]), &s, &t);
		gSPScisTextureRectangle(glistp++, (xPos + 0) << 2, (yPos + 0) << 2, (xPos + FONT_WIDTH) << 2, (yPos + FONT_HALF_HEIGHT) << 2, 0, s << 5, t << 5, 1 << 10, 1 << 10);
		xPos += FONT_WIDTH;
		i++;
	}

	xPos = x;
	yPos = y;
	i = 0;
    gDPLoadTextureBlock_4b(glistp++, font_bin + 4096u, G_IM_FMT_IA, 128, 64, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
	while (str[i] != '\0') {
		int s;
		int t;

		if ((cutoff > -1) && (i == cutoff)) {
			break;
		}

		if (str[i] == ' ') {
			xPos += FONT_WIDTH;
			i++;
			continue;
		}

		if (str[i] == '\n') {
			xPos = x;
			yPos += FONT_HEIGHT;
			i++;
			continue;
		}

		getCharST(&(str[i]), &s, &t);
		gSPScisTextureRectangle(glistp++, (xPos + 0) << 2, (yPos + FONT_HALF_HEIGHT) << 2, (xPos + FONT_WIDTH) << 2, (yPos + FONT_HEIGHT) << 2, 0, s << 5, t << 5, 1 << 10, 1 << 10);
		xPos += FONT_WIDTH;
		i++;
	}
}

void drawString(int x, int y, const char* str, int cutoff) {
	drawString_impl(x, y, str, cutoff);
}

void drawLabBackground(int x, int y) {
	int i;

	for (i = 0; i < (240 / 5); i++) {
		gDPLoadTextureTile(glistp++, lab_bg_bin, G_IM_FMT_RGBA, G_IM_SIZ_16b, 320, 240, 0, (i * 5), 320 - 1, ((i + 1) * 5) - 1, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD );
		gSPScisTextureRectangle(glistp++, x << 2, (y + (i * 5)) << 2, (x + 320) << 2, (y + ((i + 1) * 5)) << 2, 0, 0 << 5, (i * 5) << 5, 1 << 10, 1 << 10);
	}
}

void drawProtagonist(int x, int y) {
	int i;

	for (i = 0; i < (PROTAG_PORTRAIT_HEIGHT / 10); i++) {
		gDPLoadTextureTile(glistp++, doc_protag_portrait_bin, G_IM_FMT_RGBA, G_IM_SIZ_16b, PROTAG_PORTRAIT_WIDTH, PROTAG_PORTRAIT_HEIGHT, 0, (i * 10), PROTAG_PORTRAIT_WIDTH - 1, ((i + 1) * 10) - 1, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD );
		gSPScisTextureRectangle(glistp++, x << 2, (y + (i * 10)) << 2, (x + PROTAG_PORTRAIT_WIDTH) << 2, (y + ((i + 1) * 10)) << 2, 0, 0 << 5, (i * 10) << 5, 1 << 10, 1 << 10);
	}
}

void drawBosss(int x, int y) {
	int i;

	for (i = 0; i < (BOSS_PORTRAIT_HEIGHT / 15); i++) {
		gDPLoadTextureTile(glistp++, doc_boss_portrait_bin, G_IM_FMT_RGBA, G_IM_SIZ_16b, BOSS_PORTRAIT_WIDTH, BOSS_PORTRAIT_HEIGHT, 0, (i * 15), BOSS_PORTRAIT_WIDTH - 1, ((i + 1) * 15) - 1, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD );
		gSPScisTextureRectangle(glistp++, x << 2, (y + (i * 15)) << 2, (x + BOSS_PORTRAIT_WIDTH) << 2, (y + ((i + 1) * 15)) << 2, 0, 0 << 5, (i * 15) << 5, 1 << 10, 1 << 10);
	}
}

void makeDLDialogue(void) {
	DisplayData* dynamicp;

	dynamicp = &gfx_dynamic[gfx_gtask_no];
	glistp = &gfx_glist[gfx_gtask_no][0];

	gfxRCPInit();
	gfxClearCfb();

	gDPPipeSync(glistp++);
	gDPSetCycleType(glistp++, G_CYC_1CYCLE);
    gDPSetCombineMode(glistp++, G_CC_SHADE, G_CC_SHADE);
    gDPSetScissor(glistp++, G_SC_NON_INTERLACE, 0, 0, SCREEN_WD - 1, SCREEN_HT - 1);
    gDPSetRenderMode(glistp++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gSPClearGeometryMode(glistp++,0xFFFFFFFF);
    gSPSetGeometryMode(glistp++, G_SHADE | G_SHADING_SMOOTH | G_CULL_BACK);

    guOrtho(&(dynamicp->orthoHudProjection), 0, SCREEN_WD - 1, 0, SCREEN_HT - 1, 0.f, 10.f, 1.f);
    gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->orthoHudProjection)), G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    guMtxIdent(&(dynamicp->orthoHudModelling));
    gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->orthoHudModelling)), G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);

	gSPTexture(glistp++, 0xffff, 0xffff, 0, G_TX_RENDERTILE, G_ON);
	gDPPipeSync(glistp++);
	gDPSetTextureFilter(glistp++, G_TF_BILERP);
	gDPSetRenderMode(glistp++, G_RM_AA_TEX_EDGE, G_RM_AA_TEX_EDGE);
	if (isFading) {
		int tVal;
		float t = fadeTime / FADE_IN_OUT_DURATION;
		if (isFadingOut) {
			t = 1.f - t;
		}
		tVal = t * 255;
		gDPSetPrimColor(glistp++, 0, 0, tVal, tVal, tVal, 255);
		gDPSetCombineMode(glistp++,G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM);
	} else {
		gDPSetCombineMode(glistp++,G_CC_DECALRGBA, G_CC_DECALRGBA);
	}
	gDPSetTexturePersp(glistp++, G_TP_NONE);

	drawLabBackground(0, 0);
	drawBosss(portratPositions[0].x, portratPositions[0].y);
	drawProtagonist(portratPositions[1].x, portratPositions[1].y);

	if (!isFading) {
	    drawString(72, 128, currentLine->data, letterIndex);
	    if ((!tickingText) && (currentLine->data[letterIndex] == '\0')) {
	    	drawString(256, 200, "(A)", -1);
		}
	}

	gDPFullSync(glistp++);
	gSPEndDisplayList(glistp++);

	nuGfxTaskStart(&gfx_glist[gfx_gtask_no][0],
		 (s32)(glistp - gfx_glist[gfx_gtask_no]) * sizeof (Gfx),
		 NU_GFX_UCODE_F3DLP_REJ , NU_SC_SWAPBUFFER);

	gfx_gtask_no = (gfx_gtask_no + 1) % 3;
}

void updateText(float deltaSeconds) {
	u32 currentSpeakerSound = SOUND_NOBODY_BIP;
	if (currentLine->speakerIndex == BOSS_SPEAKING) {
		currentSpeakerSound = SOUND_BOSS_BIP;
	} else if (currentLine->speakerIndex == PROTAG_SPEAKING) {
		currentSpeakerSound = SOUND_PLAYER_BIP;
	} else if (currentLine->speakerIndex == JUST_PROTAG_THERE) {
		currentSpeakerSound = SOUND_PLAYER_BIP;
	}

	if (tickingText) {
		textTime += deltaSeconds;
		if (textTime > TIME_PER_LETTER) {
			textTime = 0;
			if ((currentLine->data[letterIndex] != ' ') && (currentLine->data[letterIndex] != '\n')) {
				nuAuSndPlayerPlay(currentSpeakerSound);
			}
			letterIndex++;

			if (currentLine->data[letterIndex] == '\0') {
				tickingText = 0;
			}
		}
	}

	if (tickingText && (currentLine->data[letterIndex] != '\0') && (contdata->trigger & A_BUTTON)) {
		while ((currentLine->data[letterIndex] != '\0')) {
			letterIndex++;
		}
		tickingText = 0;
		textTime = 0;

		return;
	}

	if ((!tickingText) && (currentLine->data[letterIndex] == '\0') && (contdata->trigger & A_BUTTON)) {
		if (currentLine->next) {
			currentLine = currentLine->next;
			refreshTargetSpots(currentLine->speakerIndex);
			letterIndex = 0;
			textTime = 0;
			tickingText = 1;
		} else {
			isFading = 1;
			isFadingOut = 1;
			fadeTime = 0.f;
		}
	}
}

void updateDialogue(void) {
	OSTime newTime = OS_CYCLES_TO_USEC(osGetTime());
	float deltaSeconds = 0.f;

	nuContDataGetEx(contdata,0);

	delta = newTime - time;
	time = newTime;
	deltaSeconds = delta * 0.000001f;

	if (isFading) {
		fadeTime += deltaSeconds;
		if (fadeTime > FADE_IN_OUT_DURATION) {
			isFading = 0;

			if (isFadingOut) {
				if (finishedLevel) {
					finishedLevel = 0;
					screenType = IntroCardScreen;
	      			currentLevel++;
				} else {
					screenType = StageScreen;
				}
				
				changeScreensFlag = 1;
				nuAuSeqPlayerStop(0);
			}
		}
	} else {
		updateText(deltaSeconds);

		portratPositions[0].x = lerp(portratPositions[0].x, portratTargetSpots[0].x, 0.17f);
		portratPositions[0].y = lerp(portratPositions[0].y, portratTargetSpots[0].y, 0.17f);
		portratPositions[1].x = lerp(portratPositions[1].x, portratTargetSpots[1].x, 0.17f);
		portratPositions[1].y = lerp(portratPositions[1].y, portratTargetSpots[1].y, 0.17f);
	}
}
