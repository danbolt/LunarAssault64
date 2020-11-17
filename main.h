
#ifndef MAIN_H
#define MAIN_H

#include "graphic.h"

#ifdef _LANGUAGE_C

typedef enum {
	StageScreen,
	DialogueScreen,
	TitleScreen
} ScreenSetting;

#define NUMBER_OF_LEVELS 2

extern volatile int changeScreensFlag;
extern volatile ScreenSetting screenType;
extern volatile int currentLevel;

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




