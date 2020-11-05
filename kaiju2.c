
#include "kaiju2.h"

#include "hitboxes.h"
#include "gamemath.h"
#include "map.h"

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

float kaijuTime;
int pointIndex;
float timeBetweenPoints;

vec2 walkPoints2[] = {
  { 48.f, 2.f },
  { 48.f, 100.f },
  { 100.f, 48.f },
  { 100.f, 100.f },
  { 48.f, 100.f },
};
#define NUMBER_OF_WALK_POINTS (sizeof(walkPoints2)/sizeof(walkPoints2[0]))
#define LOOPED_WALK_POINT(index) ((index + NUMBER_OF_WALK_POINTS) % NUMBER_OF_WALK_POINTS)

void initKaiju2() {
  int i;

  kaijuTime = 0.f;
  pointIndex = 0;
  timeBetweenPoints = 30.f;

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

  // Create an appendage that's a hitbox
  hitboxes[1].alive = 1;
  hitboxes[1].destroyable = 1;
  hitboxes[1].isTransformDirty = 1;
  hitboxes[1].position = (vec3){ 1.f, 1.7f, -0.2f };
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

  hitboxes[2].alive = 1;
  hitboxes[2].destroyable = 1;
  hitboxes[2].isTransformDirty = 1;
  hitboxes[2].position = (vec3){ -1.f, 1.7f, -0.2f };
  hitboxes[2].rotation = (vec3){ 0.f, 0.f, 0.f };
  hitboxes[2].scale = (vec3){ 1.f / hitboxes[0].scale.x, 1.f / hitboxes[0].scale.y, 1.f / hitboxes[0].scale.z };
  guMtxIdentF(hitboxes[2].computedTransform.data);
  hitboxes[2].displayCommands = red_octahedron_commands;
  hitboxes[2].check = sdOctahedron;
  hitboxes[2].numberOfChildren = 0;
  for (i = 0; i < MAX_CHILDREN_PER_HITBOX; i++) {
    hitboxes[2].children[i] = NULL;
  }
  hitboxes[2].parent = NULL;
  parentHitboxes(&(hitboxes[2]), &(hitboxes[0]));
}

void updateKaiju2(float deltaSeconds) {
  float tVal = 0.f;
  kaijuTime += deltaSeconds;
  if (kaijuTime > timeBetweenPoints) {
    pointIndex = LOOPED_WALK_POINT(pointIndex + 1);
    kaijuTime = 0.f;
  }

  tVal = kaijuTime / timeBetweenPoints;

	hitboxes[0].position.x = catmullRom(tVal, walkPoints2[LOOPED_WALK_POINT(pointIndex - 1)].x, walkPoints2[LOOPED_WALK_POINT(pointIndex)].x, walkPoints2[LOOPED_WALK_POINT(pointIndex + 1)].x, walkPoints2[LOOPED_WALK_POINT(pointIndex + 2)].x);
  hitboxes[0].position.y = catmullRom(tVal, walkPoints2[LOOPED_WALK_POINT(pointIndex - 1)].y, walkPoints2[LOOPED_WALK_POINT(pointIndex)].y, walkPoints2[LOOPED_WALK_POINT(pointIndex + 1)].y, walkPoints2[LOOPED_WALK_POINT(pointIndex + 2)].y);
  hitboxes[0].position.z = getHeight(hitboxes[0].position.x, hitboxes[0].position.y) + 25.f;
  hitboxes[0].isTransformDirty = 1;
}

void renderKaiju2(DisplayData* dynamicp) {
  gSPTexture(glistp++, 0x8000, 0x8000, 0, 0, G_OFF);
  gDPPipeSync(glistp++);
  gDPSetCombineLERP(glistp++, NOISE, 0, ENVIRONMENT, SHADE, 0, 0, 0, SHADE, NOISE, 0, ENVIRONMENT, SHADE, 0, 0, 0, SHADE);
  gDPSetRenderMode(glistp++, G_RM_ZB_OPA_SURF, G_RM_ZB_OPA_SURF2);
  gSPClearGeometryMode(glistp++, 0xFFFFFFFF);
  gSPSetGeometryMode(glistp++, G_SHADE | G_SHADING_SMOOTH | G_CULL_BACK | G_ZBUFFER);
  gSPClipRatio(glistp++, FRUSTRATIO_6);
  appendHitboxDL(&(hitboxes[0]), dynamicp);
  gDPPipeSync(glistp++);
  gDPSetRenderMode(glistp++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
  gDPSetCombineLERP(glistp++, NOISE, 0, ENVIRONMENT, TEXEL0, 0, 0, 0, SHADE, NOISE, 0, ENVIRONMENT, TEXEL0, 0, 0, 0, SHADE);
  gSPClipRatio(glistp++, FRUSTRATIO_2);
  gSPClearGeometryMode(glistp++, 0xFFFFFFFF);
  gSPSetGeometryMode(glistp++, G_CULL_BACK);
  gSPTexture(glistp++, 0x8000, 0x8000, 0, 0, G_ON);
}
