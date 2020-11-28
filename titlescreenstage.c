
#include "titlescreenstage.h"

#include <nusys.h>
#include <nualsgi_n.h>
#include "main.h"
#include "soundid.h"
#include "segmentinfo.h"

#define FADE_IN_TIME 0.7f

u8 titleScreenBackgroundBuffer[153600];
float fadeTimePassed;
u32 isFading = 0;

static OSTime time = 0;
static OSTime delta = 0;

void initTitleScreen(void) {
	// nuAuSeqPlayerStop(0);
	// nuAuSeqPlayerSetNo(0, 0);
	// nuAuSeqPlayerPlay(0);

	nuPiReadRom((u32)_title_screen_bgSegmentRomStart, titleScreenBackgroundBuffer, 153600);

	fadeTimePassed = 0.f;
	isFading = 1;


    time = OS_CYCLES_TO_USEC(osGetTime());
	delta = 0;
}


void makeDLTitleScreen(void) {
	DisplayData* dynamicp;
	char conbuf[20];
	int i;

	dynamicp = &gfx_dynamic[gfx_gtask_no];
	glistp = &gfx_glist[gfx_gtask_no][0];

	gfxRCPInit();
	gfxClearCfb();

	gDPSetColorImage(glistp++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_WD, osVirtualToPhysical(nuGfxCfb_ptr));
	gDPSetFillColor(glistp++, (GPACK_RGBA5551(255, 255, 255, 1) << 16 | GPACK_RGBA5551(255, 255, 255, 1)));
	gDPFillRectangle(glistp++, 0, 0, SCREEN_WD-1, SCREEN_HT-1);
	gDPPipeSync(glistp++);

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
	} else {
		gDPSetCombineMode(glistp++,G_CC_DECALRGBA, G_CC_DECALRGBA);
	}


	gDPSetTextureFilter(glistp++, G_TF_BILERP);
	gDPSetRenderMode(glistp++, G_RM_XLU_SURF, G_RM_XLU_SURF);
	gDPSetTexturePersp(glistp++, G_TP_NONE);
	gDPPipeSync(glistp++);

	for (i = 0; i < (240 / 5); i++) {
		gDPLoadTextureTile(glistp++, titleScreenBackgroundBuffer, G_IM_FMT_RGBA, G_IM_SIZ_16b, 320, 240, 0, (i * 5), 320 - 1, ((i + 1) * 5) - 1, 0, G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD );
		gSPScisTextureRectangle(glistp++, 0 << 2, (0 + (i * 5)) << 2, (0 + 320) << 2, (0 + ((i + 1) * 5)) << 2, 0, 0 << 5, (i * 5) << 5, 1 << 10, 1 << 10);
	}

	gDPFullSync(glistp++);
	gSPEndDisplayList(glistp++);

	nuGfxTaskStart(&gfx_glist[gfx_gtask_no][0], (s32)(glistp - gfx_glist[gfx_gtask_no]) * sizeof (Gfx), NU_GFX_UCODE_F3DLP_REJ , NU_SC_NOSWAPBUFFER);

	nuDebConTextColor(0, NU_DEB_CON_TEXT_BLACK);
	if(contPattern & 0x1)
    {
		nuDebConTextPos(0,2,28);
		sprintf(conbuf, "audio heap used %d", nuAuHeapGetUsed());
		nuDebConCPuts(0, conbuf);
		nuDebConTextPos(0,2,29);
		sprintf(conbuf, "audio heap free %d", nuAuHeapGetFree());
		nuDebConCPuts(0, conbuf);

		if (!isFading) {
			nuDebConTextPos(0,20 - 6,20);
			sprintf(conbuf, "press start!");
			nuDebConCPuts(0, conbuf);
		}
	}
	else
	{
		nuDebConTextPos(0,4,4);
		nuDebConCPuts(0, "Connect controller #1, kid!");
	}

	nuDebConDisp(NU_SC_SWAPBUFFER);

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
	}

	if (contdata->trigger & START_BUTTON) {
		nuAuSeqPlayerStop(0);
		changeScreensFlag = 1;
		screenType = IntroCardScreen;
		currentLevel = 0;
		return;
	}
}
