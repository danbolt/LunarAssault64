#ifndef _DIALOGUE_STAGE_H_
#define _DIALOGUE_STAGE_H_


#define NO_PEOPLE_SPEAKING -2
#define NOBODY_THERE -1
#define BOSS_SPEAKING 0
#define PROTAG_SPEAKING 1
#define JUST_PROTAG_THERE 2
#define JUST_PROTAG_THERE_BUT_NO_PEOPLE_SPEAKING 3
#define JUST_BOSS_THERE 4

typedef struct DialogueLine {
	const char* data;
	struct DialogueLine* next;
	int speakerIndex;
} DialogueLine;

void initDialogue(void);
void makeDLDialogue(void);
void updateDialogue(void);

#endif