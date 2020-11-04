#ifndef _DIALOGUE_STAGE_H_
#define _DIALOGUE_STAGE_H_

typedef struct DialogueLine {
	const char* data;
	struct DialogueLine* next;
	int speakerIndex;
} DialogueLine;

void initDialogue(void);
void makeDLDialogue(void);
void updateDialogue(void);

#endif