#ifndef _HITBOXES_H_
#define _HITBOXES_H_

#include "gamemath.h"

#define NUMBER_OF_KAIJU_HITBOXES 16

typedef float (*SDFCallback)(const vec3*);

typedef struct {
	u8 alive;

	vec3 position;
	vec3 rotation;
	vec3 scale;

	mat4 computedTransform;
	mat4 computedInverse;
	u8 isTransformDirty;

	Gfx* displayCommands;
	SDFCallback check;
} KaijuHitbox;

#endif