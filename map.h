#ifndef _MAP_H_
#define _MAP_H_

#include <nusys.h>
#include "gamemath.h"

#define MAP_WIDTH 128
#define MAP_HEIGHT 128
#define MAP_LENGTH (MAP_WIDTH * MAP_HEIGHT)

#define VERTS_PER_SECTION 64
#define LOW_DETAIL_VERTS_PER_SECTION 4
#define VERTS_PER_TILE 4
#define TILES_PER_SECTION (VERTS_PER_SECTION / VERTS_PER_TILE)
#define COMMANDS_PER_SECTION 20 
#define SECTION_SIZE 4

#define SECTIONS_PER_MAP (MAP_LENGTH / (SECTION_SIZE * SECTION_SIZE))

typedef struct {
	vec3 centroid;
	Vtx verts[VERTS_PER_SECTION];
	Gfx commands[COMMANDS_PER_SECTION];
} SectionData;

typedef struct {
	vec3 centroid;
	Vtx verts[LOW_DETAIL_VERTS_PER_SECTION];
	Gfx commands[4];
} LowDetailSectionData;

extern u8 GroundMapping[MAP_LENGTH];
extern u8 GroundTexIndex[MAP_LENGTH];

extern SectionData sections[SECTIONS_PER_MAP];
extern LowDetailSectionData lowDetailSections[SECTIONS_PER_MAP];

u8 sampleHeight(u16 x, u16 y);
u8 sampleTileTex(u16 x, u16 y);

float getHeight(float x, float y);

void generateSectionDLs();

#endif