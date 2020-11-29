
#include "creditsscreen.h"

#include <nusys.h>
#include <nualsgi_n.h>
#include "main.h"
#include "soundid.h"

static OSTime time = 0;
static OSTime delta = 0;

static float timePassed = 0.f;

#define BLACK_TIME 1.21626f
#define MINIMUM_ON_TIME 4.f
#define WHITE_FADE_OUT_TIME 5.f

static int showingCard = 0;
static int currentCardIndex = 0;

// Naive strlen copied from
// https://stackoverflow.com/questions/22520413/c-strlen-implementation-in-one-line-of-code
void my_strlen(const char *str, u32 *len)
{
    for (*len = 0; str[*len]; (*len)++);
}

typedef struct {
	const char* data;
} CreditItem;

typedef struct {
	int quantity;

	CreditItem items[16];
} CreditCard;


CreditCard Closer = {
	3,
	{
		{
			"http://danbolt.itch.io/"
		},
		{ "" },
		{
			"Thanks for playing! <3"
		}
	}
};
CreditCard extraspecialthanks = {
	7,
	{
		{
			"Extra Special Thanks"
		},
		{
			""
		},
		{
			"Natsha Miner"
		},
		{
			"Alyssa Savage"
		},
		{
			"Rose-Lynne Savage"
		},
		{
			"My Mom and Dad!"
		},
		{
			"The N64Brew Jam Organizers!"
		}
	}
};
CreditCard specialThanksContd = {
	8,
	{
		{
			"Special Thanks (cont.d)"
		},
		{
			""
		},
		{
			"Vanadium"
		},
		{
			"joeldipops"
		},
		{
			"Vatuu"
		},
		{
			"lambertjamesd"
		},
		{
			"Zest"
		},
		{
			"kivan117"
		},
		{
			"devwizard"
		}
	}
};
CreditCard specialThanks = {
	8,
	{
		{
			"Special Thanks"
		},
		{
			""
		},
		{
			"Hazematman"
		},
		{
			"WadeMalone"
		},
		{
			"Allie"
		},
		{
			"Nabile"
		},
		{
			"gamemasterplc"
		},
		{
			"streams"
		},
		{
			"manfried"
		}
	}
};
CreditCard hqvmSupport = {
	3,
	{
		{
			"Answered my HQVM questions"
		},
		{
			""
		},
		{
			"CrashOveride"
		},
	}
};
CreditCard UNFLoader = {
	3,
	{
		{
			"UNFLoader Development"
		},
		{
			""
		},
		{
			"Buu342"
		},
	}
};
CreditCard VoiceActing = {
	5,
	{
		{
			"Voices"
		},
		{
			""
		},
		{
			"Natasha Miner"
		},
		{
			"Clayton Savage"
		},
		{
			"William Beltran"
		},
	}
};
CreditCard AudioTracks = {
	6,
	{
		{
			"Music"
		},
		{
			""
		},
		{
			"Clair de Lune - Claude Debussy"
		},
		{
			"Cello Suite No. 1 - J. S. Bach"
		},
		{
			"harbour - Morusque"
		},
		{
			"ARENE01 - Morusque"
		},
	}
};
CreditCard CutsceneAssets = {
	7,
	{
		{
			"Intro FMV Assets"
		},
		{
			""
		},
		{
			"Earth Texture - Solar System Scope"
		},
		{
			"Moon Texture - Solar System Scope"
		},
		{
			"Stars Texture - Solar System Scope"
		},
		{
			"Clair de Lune - Claude Debussy"
		},
		{
			"Satellite Model - Daniel Savage"
		},
	}
};
CreditCard Development = {
	3,
	{
		{
			"Development Work"
		},
		{
			""
		},
		{
			"Daniel Savage"
		},
	}
};
CreditCard Intro = {
	1,
	{
		{
			"Lunar Assault 64"
		}
	}
};

CreditCard* cards[] = {
	&Intro,
	&Development,
	&VoiceActing,
	&AudioTracks,
	&hqvmSupport,
	&UNFLoader,
	&CutsceneAssets,
	&specialThanks,
	&specialThanksContd,
	&extraspecialthanks,
	&Closer,
	NULL,
};

void initCreditscreen(void) {
	nuAuSeqPlayerStop(0);
	nuAuSeqPlayerSetNo(0, 0);
	nuAuSeqPlayerPlay(0);

    time = OS_CYCLES_TO_USEC(osGetTime());
	delta = 0;

	timePassed = 0.f;
	showingCard = 0;
	currentCardIndex = 0;
}


void makeDLCreditsScreen(void) {
	DisplayData* dynamicp;
	char conbuf[20];
	int i;
	u32 len;

	dynamicp = &gfx_dynamic[gfx_gtask_no];
	glistp = &gfx_glist[gfx_gtask_no][0];

	gfxRCPInit();
	gfxClearCfb();

	if (!showingCard && (cards[currentCardIndex] == NULL)) {
		int fadeVal = (timePassed / WHITE_FADE_OUT_TIME) * 255;

		gDPSetFillColor(glistp++, (GPACK_RGBA5551(fadeVal, fadeVal, fadeVal, 1) << 16 | GPACK_RGBA5551(fadeVal, fadeVal, fadeVal, 1)));
		gDPFillRectangle(glistp++, 0, 0, SCREEN_WD-1, SCREEN_HT-1);
		gDPPipeSync(glistp++);
	}

	gDPFullSync(glistp++);
	gSPEndDisplayList(glistp++);

	nuGfxTaskStart(&gfx_glist[gfx_gtask_no][0], (s32)(glistp - gfx_glist[gfx_gtask_no]) * sizeof (Gfx), NU_GFX_UCODE_F3DLP_REJ , NU_SC_NOSWAPBUFFER);

	nuDebConClear(0);
	if (showingCard) {
		nuDebConTextColor(0, NU_DEB_CON_TEXT_WHITE);
		
		for (i = 0; i < cards[currentCardIndex]->quantity; i++) {
			my_strlen(cards[currentCardIndex]->items[i].data, &len);

			nuDebConTextPos(0, (40 - len) / 2, ((30 - (cards[currentCardIndex]->quantity * 2)) / 2) + (i * 2));
			sprintf(conbuf, cards[currentCardIndex]->items[i].data);
			nuDebConCPuts(0, conbuf);
		}
	}
	

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

	timePassed += deltaSeconds;

	if (showingCard) {
		if (timePassed > MINIMUM_ON_TIME) {
			currentCardIndex++;
			showingCard = 0;
			timePassed = 0.f;
		}
	} else {
		if (cards[currentCardIndex] == NULL) {
			nuAuSeqPlayerSetVol(0, (1.f - (timePassed / WHITE_FADE_OUT_TIME)) * 0x7fff);

			if (timePassed > WHITE_FADE_OUT_TIME) {
				screenType = TitleScreen;
				changeScreensFlag = 1;
				nuAuSeqPlayerStop(0);
				nuAuSeqPlayerSetVol(0, 0x7fff);
				return;
			}
		}

		if ((cards[currentCardIndex] != NULL) && (timePassed > BLACK_TIME)) {
			showingCard = 1;
			timePassed = 0.f;
		}
	}
}
