
#ifndef MAIN_H
#define MAIN_H

#include "graphic.h"

#ifdef _LANGUAGE_C

typedef enum {
	StageScreen,
	DialogueScreen,
	TitleScreen,
	FMVScreen,
	IntroCardScreen,
	CreditsScreen,
	RetryScreen
} ScreenSetting;

#define NUMBER_OF_LEVELS 5

extern volatile int changeScreensFlag;
extern volatile ScreenSetting screenType;
extern volatile int currentLevel;
extern volatile u8 finishedLevel;

extern NUContData	contdata[1];
extern u8 contPattern;

extern u32 groundTextureROMAddress;
extern u32 terrainROMAddress;
extern u32 topographyROMAddress;

extern void (*initKaijuCallback)();
extern void (*updateKaijuCallback)(float);
extern void (*renderKaijuCallback)(DisplayData*);

#endif /* _LANGUAGE_C */
#endif /* MAIN_H */




