#ifndef _HITBOXES_H_
#define _HITBOXES_H_

#include <nusys.h>

#include "gamemath.h"
#include "graphic.h"

#define MAX_CHILDREN_PER_HITBOX 8

typedef float (*SDFCallback)(const vec3*);

typedef struct KaijuHitbox {
	u8 alive;
	u8 destroyable;

	vec3 position;
	vec3 rotation;
	vec3 scale;

	mat4 computedTransform;
	mat4 computedInverse;
	u8 isTransformDirty;

	Gfx* displayCommands;
	SDFCallback check;

	u8 numberOfChildren;
	struct KaijuHitbox* children[MAX_CHILDREN_PER_HITBOX];
	struct KaijuHitbox* parent;
} KaijuHitbox;
void parentHitboxes(KaijuHitbox* child, KaijuHitbox* newParent);

void appendHitboxDL(KaijuHitbox* hitbox, DisplayData* dynamicp);


extern KaijuHitbox hitboxes[NUMBER_OF_KAIJU_HITBOXES];

#endif