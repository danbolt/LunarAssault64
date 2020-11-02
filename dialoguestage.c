#include "dialoguestage.h"

#include "main.h"
#include "font.h"
#include "graphic.h"

#define FONT_WIDTH 8
#define FONT_HALF_HEIGHT 8
#define FONT_HEIGHT 16

#define ASCII_SPACE 32

static Vtx test_geo[] = {
  { 100,  100,  5, 0, 32 << 6,  0 << 6, 0xFF, 0x21, 0x04, 0xff },
  { 208,  100,  5, 0,  0 << 6,  0 << 6, 0xFF, 0x21, 0, 0xff },
  { 208,  128,  5, 0,  0 << 6, 32 << 6, 0xFF, 0x21, 0, 0xff },
  { 100,  128,  5, 0, 32 << 6, 32 << 6, 0xFF, 0x21, 0x04, 0xff },
};

static Gfx test_cmd[] = {
  gsSPVertex(&test_geo, 4, 0),
  gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
  gsSPEndDisplayList()
};

void initDialogue(void) {
	//
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


    gSPDisplayList(glistp++, test_cmd);

	gSPTexture(glistp++, 0xffff, 0xffff, 0, G_TX_RENDERTILE, G_ON);
	gDPPipeSync(glistp++);
	gDPSetTextureFilter(glistp++, G_TF_AVERAGE);
	gDPSetRenderMode(glistp++, G_RM_TEX_EDGE, G_RM_TEX_EDGE);
	gDPSetCombineMode(glistp++,G_CC_DECALRGBA, G_CC_DECALRGBA);
	gDPSetTexturePersp(glistp++, G_TP_NONE);

    drawString(72, 128, "Welcome to the\ndialogue screen test!\nHow does it look?\nI hope it's okay.", -1);

    gSPTexture(glistp++, 0xffff, 0xffff, 0, G_TX_RENDERTILE, G_OFF);

	gDPFullSync(glistp++);
	gSPEndDisplayList(glistp++);

	nuGfxTaskStart(&gfx_glist[gfx_gtask_no][0],
		 (s32)(glistp - gfx_glist[gfx_gtask_no]) * sizeof (Gfx),
		 NU_GFX_UCODE_F3DLP_REJ , NU_SC_SWAPBUFFER);

	gfx_gtask_no = (gfx_gtask_no + 1) % 3;
}

void updateDialogue(void) {
	nuContDataGetEx(contdata,0);

	if (contdata->trigger & A_BUTTON) {
		changeScreensFlag = 1;
		screenType = StageScreen;
		return;
	}
}