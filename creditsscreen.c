
#include "creditsscreen.h"

#include <nusys.h>
#include <nualsgi_n.h>
#include "main.h"
#include "soundid.h"

static OSTime time = 0;
static OSTime delta = 0;

void initCreditscreen(void) {
	// nuAuSeqPlayerStop(0);
	// nuAuSeqPlayerSetNo(0, 0);
	// nuAuSeqPlayerPlay(0);

    time = OS_CYCLES_TO_USEC(osGetTime());
	delta = 0;
}


void makeDLCreditsScreen(void) {
	DisplayData* dynamicp;
	char conbuf[20];
	int i;

	dynamicp = &gfx_dynamic[gfx_gtask_no];
	glistp = &gfx_glist[gfx_gtask_no][0];

	gfxRCPInit();
	gfxClearCfb();

	gDPFullSync(glistp++);
	gSPEndDisplayList(glistp++);

	nuGfxTaskStart(&gfx_glist[gfx_gtask_no][0], (s32)(glistp - gfx_glist[gfx_gtask_no]) * sizeof (Gfx), NU_GFX_UCODE_F3DLP_REJ , NU_SC_NOSWAPBUFFER);

	nuDebConClear(0);
	nuDebConTextColor(0, NU_DEB_CON_TEXT_WHITE);
	
	nuDebConTextPos(0,2,0);
	sprintf(conbuf, "Dreams Come True");
	nuDebConCPuts(0, conbuf);

	nuDebConDisp(NU_SC_SWAPBUFFER);

	gfx_gtask_no = (gfx_gtask_no + 1) % 3;
}

void updateCreditsScreen(void) {
	OSTime newTime = OS_CYCLES_TO_USEC(osGetTime());
	float deltaSeconds = 0.f;

	nuContDataGetEx(contdata,0);

	delta = newTime - time;
	time = newTime;
	deltaSeconds = delta * 0.000001f;

	//
}
