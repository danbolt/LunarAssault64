#include "dialoguestage.h"

#include "main.h"
#include "font.h"
#include "graphic.h"
#include "gamemath.h"
#include "segmentinfo.h"

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

DialogueLine closing = { "And the work began.", NULL, NOBODY_THERE };

DialogueLine d2 = { "I'm on it!\nYou can count on me.", &closing, PROTAG_SPEAKING };
DialogueLine d1 = { "Previous operators have\ntaken advantage of\nthe hill in the middle.", &d2, BOSS_SPEAKING };
DialogueLine dBrief = { "A mark IV Lunarbeast\nhas been spotted there.", &d1, BOSS_SPEAKING };
DialogueLine d0 = { "Your first task today\nwill be at Crater Pit.", &dBrief, BOSS_SPEAKING };
DialogueLine dIntro = { "Lunar Fufillment Centre\n13:53 Earth Time, CST", &d0, NOBODY_THERE };

DialogueLine dSecondStage2 = { "Whew, okay!", &closing, PROTAG_SPEAKING };
DialogueLine dSecondStage1 = { "Don't mess it up\nkid.", &dSecondStage2, BOSS_SPEAKING };
DialogueLine dSecondStage0 = { "Okay, this is the\nsecond piece of dia-\nlogue.", &dSecondStage1, BOSS_SPEAKING };
DialogueLine dSecondStageIntro = { "Lunar Fufillment Centre\n08:15 Earth Time, CST", &dSecondStage0, NOBODY_THERE };

DialogueLine* stageDialogues[NUMBER_OF_LEVELS] = {
	&dIntro,
	&dSecondStageIntro
};

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
	}
}

void initDialogue(void) {
	// TODO: add a fadein
	letterIndex = 0;
	currentLine = stageDialogues[currentLevel];
	tickingText = 1;
	textTime = 0.f;

    time = OS_CYCLES_TO_USEC(osGetTime());
	delta = 0;

	portratPositions[0].x = -BOSS_PORTRAIT_WIDTH;
	portratPositions[0].y = 240;
	portratPositions[1].x = 320;
	portratPositions[1].y = 240;
	refreshTargetSpots(currentLine->speakerIndex);
}

void getCharST(const char* character, int* s, int* t) {
	// Use a question mark if we have a strange ASCII character
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
	gDPSetCombineMode(glistp++,G_CC_DECALRGBA, G_CC_DECALRGBA);
	gDPSetTexturePersp(glistp++, G_TP_NONE);

	drawBosss(portratPositions[0].x, portratPositions[0].y);
	drawProtagonist(portratPositions[1].x, portratPositions[1].y);

    drawString(72, 128, currentLine->data, letterIndex);
    if ((!tickingText) && (currentLine->data[letterIndex] == '\0')) {
    	drawString(256, 200, "(A)", -1);
	}

	gDPFullSync(glistp++);
	gSPEndDisplayList(glistp++);

	nuGfxTaskStart(&gfx_glist[gfx_gtask_no][0],
		 (s32)(glistp - gfx_glist[gfx_gtask_no]) * sizeof (Gfx),
		 NU_GFX_UCODE_F3DLP_REJ , NU_SC_SWAPBUFFER);

	gfx_gtask_no = (gfx_gtask_no + 1) % 3;
}

void updateText(float deltaSeconds) {
	if (tickingText) {
		textTime += deltaSeconds;
		if (textTime > TIME_PER_LETTER) {
			textTime = 0;
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
			// TODO: add a fadeout
			changeScreensFlag = 1;
			screenType = StageScreen;
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

	updateText(deltaSeconds);


	portratPositions[0].x = lerp(portratPositions[0].x, portratTargetSpots[0].x, 0.17f);
	portratPositions[0].y = lerp(portratPositions[0].y, portratTargetSpots[0].y, 0.17f);
	portratPositions[1].x = lerp(portratPositions[1].x, portratTargetSpots[1].x, 0.17f);
	portratPositions[1].y = lerp(portratPositions[1].y, portratTargetSpots[1].y, 0.17f);

	if (contdata->trigger & START_BUTTON) {
		changeScreensFlag = 1;
		screenType = StageScreen;
		return;
	}
}
