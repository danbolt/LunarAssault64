#include "dialoguestage.h"

#include "main.h"
#include "font.h"
#include "graphic.h"

static Vtx test_geo[] = {
  { 100,  100,  5, 0, 32 << 6,  0 << 6, 0xFF, 0x21, 0x04, 0xff },
  { 208,  100,  5, 0,  0 << 6,  0 << 6, 0xFF, 0x21, 0, 0xff },
  { 208,  200,  5, 0,  0 << 6, 32 << 6, 0xFF, 0x21, 0, 0xff },
  { 100,  200,  5, 0, 32 << 6, 32 << 6, 0xFF, 0x21, 0x04, 0xff },
};

static Gfx test_cmd[] = {
  gsSPVertex(&test_geo, 4, 0),
  gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
  gsSPEndDisplayList()
};

void initDialogue(void) {
	//
}

void makeDLDialogue(void) {
	DisplayData* dynamicp;
	char conbuf[20]; 

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
    gDPLoadTextureBlock_4b(glistp++, font_bin, G_IM_FMT_IA, 128, 64, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    gSPScisTextureRectangle(glistp++, 0 << 2, 0 << 2, 128 << 2, 64 << 2, 0, 0 << 5, 0 << 5, 1 << 10, 1 << 10);


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
