
#include "kaiju1.h"

#include "hitboxes.h"
#include "gamemath.h"

static Vtx cube_geo[] = {
  {  1,  1,  1, 0,  7 << 6, 24 << 6, 0, 0, 0xff, 0xff },
  { -1,  1,  1, 0,  0 << 6, 24 << 6, 0, 0, 0xff, 0xff },
  { -1, -1,  1, 0,  7 << 6, 31 << 6, 0, 0, 0xff, 0xff },
  {  1, -1,  1, 0,  0 << 6, 31 << 6, 0, 0, 0xff, 0xff },
  {  1,  1, -1, 0,  0 << 6, 24 << 6, 0, 0, 0xff, 0xff },
  { -1,  1, -1, 0,  0 << 6, 24 << 6, 0, 0, 0xff, 0xff },
  { -1, -1, -1, 0,  0 << 6, 31 << 6, 0, 0, 0xff, 0xff },
  {  1, -1, -1, 0,  0 << 6, 31 << 6, 0, 0, 0xff, 0xff },
};

static Gfx blue_cube_commands[] = {
  gsSPVertex(&cube_geo, 8, 0),
  gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
  gsSP2Triangles(4, 5, 0, 0, 5, 1, 0, 0),
  gsSP2Triangles(5, 6, 2, 0, 5, 2, 1, 0),
  gsSP2Triangles(7, 4, 0, 0, 7, 0, 3, 0),
  gsSP2Triangles(6, 4, 7, 0, 6, 5, 4, 0),
  gsSP2Triangles(6, 7, 3, 0, 6, 3, 2, 0),
  gsSPEndDisplayList()
};

static Vtx red_octahedron_geo[] = {
  {  1,  0,  0, 0, 16 << 6, 24 << 6, 0xaa, 0, 0x00, 0xff },
  { -1,  0,  0, 0, 16 << 6, 24 << 6, 0xaa, 0, 0x00, 0xff },
  {  0,  1,  0, 0, 23 << 6, 24 << 6, 0xaa, 0, 0x00, 0xff },
  {  0, -1,  0, 0, 23 << 6, 24 << 6, 0xaa, 0, 0x00, 0xff },
  {  0,  0,  1, 0, 16 << 6, 31 << 6, 0xff, 0, 0x00, 0xff },
  {  0,  0, -1, 0, 16 << 6, 31 << 6, 0x55, 0, 0x00, 0xff },
};

static Gfx red_octahedron_commands[] = {
  gsSPVertex(&red_octahedron_geo, 6, 0),
  gsSP2Triangles(0, 2, 4, 0, 0, 5, 2, 0),
  gsSP2Triangles(0, 4, 3, 0, 0, 3, 5, 0),
  gsSP2Triangles(1, 3, 4, 0, 1, 5, 3, 0),
  gsSP2Triangles(1, 4, 2, 0, 1, 2, 5, 0),
  gsSPEndDisplayList()
};

static Vtx blue_octahedron_geo[] = {
  {  1,  0,  0, 0, 0, 0, 0x77, 0, 0x99, 0xff },
  { -1,  0,  0, 0, 0, 0, 0x77, 0, 0x99, 0xff },
  {  0,  1,  0, 0, 0, 0, 0x77, 0, 0x99, 0xff },
  {  0, -1,  0, 0, 0, 0, 0x77, 0, 0x99, 0xff },
  {  0,  0,  1, 0, 0, 0, 0x77, 0, 0x99, 0xff },
  {  0,  0, -1, 0, 0, 0, 0x77, 0, 0x99, 0xff },
};

static Gfx blue_octahedron_commands[] = {
  gsSPVertex(&blue_octahedron_geo, 6, 0),
  gsSP2Triangles(0, 2, 4, 0, 0, 5, 2, 0),
  gsSP2Triangles(0, 4, 3, 0, 0, 3, 5, 0),
  gsSP2Triangles(1, 3, 4, 0, 1, 5, 3, 0),
  gsSP2Triangles(1, 4, 2, 0, 1, 2, 5, 0),
  gsSPEndDisplayList()
};

void initKaiju1() {
  int i;

	 for (i = 0; i < NUMBER_OF_KAIJU_HITBOXES; i++) {
    hitboxes[0].alive = 0;
  }

  // Create a "body piece"
  hitboxes[0].alive = 1;
  hitboxes[0].destroyable = 0;
  hitboxes[0].isTransformDirty = 1;
  hitboxes[0].position = (vec3){ 48.f, 2.f, 45.5f };
  hitboxes[0].rotation = (vec3){ -10.f, 0.f, 45.f };
  hitboxes[0].scale = (vec3){ 8.f, 6.f, 7.f };
  guMtxIdentF(hitboxes[0].computedTransform.data);
  hitboxes[0].displayCommands = blue_octahedron_commands;
  hitboxes[0].check = sdOctahedron;
  hitboxes[0].numberOfChildren = 0;
  for (i = 0; i < MAX_CHILDREN_PER_HITBOX; i++) {
    hitboxes[0].children[i] = NULL;
  }
  hitboxes[0].parent = NULL;

  hitboxes[4].alive = 1;
  hitboxes[4].destroyable = 0;
  hitboxes[4].isTransformDirty = 1;
  hitboxes[4].position = (vec3){ 0.f, 0.f, 1.f };
  hitboxes[4].rotation = (vec3){ 10.f, 0.f, 0.f };
  hitboxes[4].scale = (vec3){ 0.9f, 0.9f, 0.9f };
  guMtxIdentF(hitboxes[4].computedTransform.data);
  hitboxes[4].displayCommands = blue_cube_commands;
  hitboxes[4].check = sdBox;
  hitboxes[4].numberOfChildren = 0;
  for (i = 0; i < MAX_CHILDREN_PER_HITBOX; i++) {
    hitboxes[4].children[i] = NULL;
  }
  hitboxes[4].parent = NULL;
  parentHitboxes(&(hitboxes[4]), &(hitboxes[0]));

  hitboxes[2].alive = 1;
  hitboxes[2].destroyable = 0;
  hitboxes[2].isTransformDirty = 1;
  hitboxes[2].position = (vec3){ 0.75f, 0.0f, -1.7f };
  hitboxes[2].rotation = (vec3){ 7.f, 0.f, 0.f };
  hitboxes[2].scale = (vec3){ 0.25f, 0.20f, 3.f };
  guMtxIdentF(hitboxes[2].computedTransform.data);
  hitboxes[2].displayCommands = blue_octahedron_commands;
  hitboxes[2].check = sdOctahedron;
  hitboxes[2].numberOfChildren = 0;
  for (i = 0; i < MAX_CHILDREN_PER_HITBOX; i++) {
    hitboxes[2].children[i] = NULL;
  }
  hitboxes[2].parent = NULL;
  parentHitboxes(&(hitboxes[2]), &(hitboxes[0]));

  hitboxes[3].alive = 1;
  hitboxes[3].destroyable = 0;
  hitboxes[3].isTransformDirty = 1;
  hitboxes[3].position = (vec3){ -0.75f, 0.0f, -1.7f };
  hitboxes[3].rotation = (vec3){ 7.f, 0.f, 0.f };
  hitboxes[3].scale = (vec3){ 0.25f, 0.20f, 3.f };
  guMtxIdentF(hitboxes[3].computedTransform.data);
  hitboxes[3].displayCommands = blue_octahedron_commands;
  hitboxes[3].check = sdOctahedron;
  hitboxes[3].numberOfChildren = 0;
  for (i = 0; i < MAX_CHILDREN_PER_HITBOX; i++) {
    hitboxes[3].children[i] = NULL;
  }
  hitboxes[3].parent = NULL;
  parentHitboxes(&(hitboxes[3]), &(hitboxes[0]));

  // Create an appendage that's a hitbox
  hitboxes[1].alive = 1;
  hitboxes[1].destroyable = 1;
  hitboxes[1].isTransformDirty = 1;
  hitboxes[1].position = (vec3){ 0.f, 1.7f, -0.2f };
  hitboxes[1].rotation = (vec3){ 0.f, 0.f, 0.f };
  hitboxes[1].scale = (vec3){ 1.f / hitboxes[0].scale.x, 1.f / hitboxes[0].scale.y, 1.f / hitboxes[0].scale.z };
  guMtxIdentF(hitboxes[1].computedTransform.data);
  hitboxes[1].displayCommands = red_octahedron_commands;
  hitboxes[1].check = sdOctahedron;
  hitboxes[1].numberOfChildren = 0;
  for (i = 0; i < MAX_CHILDREN_PER_HITBOX; i++) {
    hitboxes[1].children[i] = NULL;
  }
  hitboxes[1].parent = NULL;
  parentHitboxes(&(hitboxes[1]), &(hitboxes[0]));
}

void updateKaiju1(float deltaSeconds) {
	//
}

// TODO: move this to its own "parent kaiju" file
void appendHitboxDL(KaijuHitbox* hitbox, DisplayData* dynamicp) {
  int i;

  if (!(hitbox->alive)) {
    return;
  }

  guMtxF2L(hitbox->computedTransform.data, &(dynamicp->hitboxTransform[dynamicp->hitboxTransformCount]));
  gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(dynamicp->hitboxTransform[dynamicp->hitboxTransformCount])), G_MTX_MODELVIEW | G_MTX_PUSH);
  gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(hitbox->displayCommands));
  dynamicp->hitboxTransformCount++;

  for (i = 0; i < hitbox->numberOfChildren; i++) {
    appendHitboxDL(hitbox->children[i], dynamicp);
  }

  gSPPopMatrix(glistp++, G_MTX_MODELVIEW);
}

void renderKaiju1(DisplayData* dynamicp) {
  gSPClipRatio(glistp++, FRUSTRATIO_6);
  appendHitboxDL(&(hitboxes[0]), dynamicp);
  gSPClipRatio(glistp++, FRUSTRATIO_2);
}
