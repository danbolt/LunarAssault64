
#include "introcardstage.h"

#include <nusys.h>
#include <nualsgi_n.h>
#include "main.h"
#include "soundid.h"

extern unsigned char doc_gimp_chapter_images_bin[];
extern unsigned int doc_gimp_chapter_images_bin_len;

#define FADE_TIME 0.6f
#define HOLD_TIME 2.84765f

static OSTime time = 0;
static OSTime delta = 0;
static float timepassed = 0;
static u8 firedLaser = 0;

void initIntroScreen(void) {
    time = OS_CYCLES_TO_USEC(osGetTime());
	delta = 0;
	timepassed = 0;
	firedLaser = 0;
}


void makeDLIntroScreen(void) {
	DisplayData* dynamicp;
	char conbuf[20];
	int i;
	int x = 0;
	int y = 96;
	int t = currentLevel * 64;

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
	if (timepassed < FADE_TIME) {
		int fadeVal = (int)(timepassed / FADE_TIME * 255.f);
		gDPSetPrimColor(glistp++, 0, 0, fadeVal, fadeVal, fadeVal, fadeVal);
	} else if (timepassed > (FADE_TIME + HOLD_TIME)) {
		int fadeVal = (int)((1.f - ((timepassed - (FADE_TIME + HOLD_TIME)) / FADE_TIME)) * 255.f);
		gDPSetPrimColor(glistp++, 0, 0, fadeVal, fadeVal, fadeVal, fadeVal);
	} else {
		gDPSetPrimColor(glistp++, 0, 0, 255, 255, 255, 255);
	}
	gDPSetCombineMode(glistp++,G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM);
	gDPSetTexturePersp(glistp++, G_TP_NONE);

	for (i = 0; i < (64 / 4); i++) {
		gDPLoadTextureTile(glistp++, doc_gimp_chapter_images_bin, G_IM_FMT_IA, G_IM_SIZ_16b, 320, 64, 0, t + (i * 4), 320 - 1, t + ((i + 1) * 4) - 1, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD );
		gSPScisTextureRectangle(glistp++, x << 2, (y + (i * 4)) << 2, (x + 320) << 2, (y + ((i + 1) * 4)) << 2, 0, 0 << 5, (t + (i * 4)) << 5, 1 << 10, 1 << 10);
	}

	gDPFullSync(glistp++);
	gSPEndDisplayList(glistp++);

	nuGfxTaskStart(&gfx_glist[gfx_gtask_no][0], (s32)(glistp - gfx_glist[gfx_gtask_no]) * sizeof (Gfx), NU_GFX_UCODE_F3DLP_REJ , NU_SC_SWAPBUFFER);

	gfx_gtask_no = (gfx_gtask_no + 1) % 3;
}

void updateIntroScreen(void) {
	OSTime newTime = OS_CYCLES_TO_USEC(osGetTime());
	float deltaSeconds = 0.f;

	delta = newTime - time;
	time = newTime;
	deltaSeconds = delta * 0.000001f;

	timepassed += deltaSeconds;

	nuContDataGetEx(contdata,0);

	if (!firedLaser && timepassed > 0.2f) {
		firedLaser = 1;

		nuAuSeqPlayerStop(0);
		nuAuSeqPlayerSetNo(0, 5 + currentLevel);
		nuAuSeqPlayerPlay(0);
	}

	if (timepassed > ( HOLD_TIME + (FADE_TIME * 2))) {
		changeScreensFlag = 1;
		screenType = DialogueScreen;

		nuAuSeqPlayerStop(0);
		return;
	}
}
