
#include "titlescreenstage.h"

#include "main.h"

void initTitleScreen(void) {
	//
}

void makeDLTitleScreen(void) {
	DisplayData* dynamicp;
	char conbuf[20];

	dynamicp = &gfx_dynamic[gfx_gtask_no];
	glistp = &gfx_glist[gfx_gtask_no][0];

	gfxRCPInit();
	gfxClearCfb();

	//

	gDPFullSync(glistp++);
	gSPEndDisplayList(glistp++);

	nuGfxTaskStart(&gfx_glist[gfx_gtask_no][0], (s32)(glistp - gfx_glist[gfx_gtask_no]) * sizeof (Gfx), NU_GFX_UCODE_F3DLP_REJ , NU_SC_NOSWAPBUFFER);

	if(contPattern & 0x1)
    {
		nuDebConTextPos(0,4,4);
		sprintf(conbuf, "LASERSCOPE MOON SPOTTER");
		nuDebConCPuts(0, conbuf);

		nuDebConTextPos(0,4,20);
		sprintf(conbuf, "press start");
		nuDebConCPuts(0, conbuf);
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
	nuContDataGetEx(contdata,0);

	if (contdata->trigger & START_BUTTON) {
		changeScreensFlag = 1;
		screenType = DialogueScreen;
		return;
	}
}
