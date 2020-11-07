
#include "kaiju1.h"

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

static Vtx yellow_octahedron_geo[] = {
  {  1,  0,  0, 0, 16 << 6, 24 << 6, 0xaa, 0xaa, 0x00, 0xff },
  { -1,  0,  0, 0, 16 << 6, 24 << 6, 0xaa, 0xaa, 0x00, 0xff },
  {  0,  1,  0, 0, 23 << 6, 24 << 6, 0xaa, 0xaa, 0x00, 0xff },
  {  0, -1,  0, 0, 23 << 6, 24 << 6, 0xaa, 0xaa, 0x00, 0xff },
  {  0,  0,  1, 0, 16 << 6, 31 << 6, 0xff, 0xff, 0x00, 0xff },
  {  0,  0, -1, 0, 16 << 6, 31 << 6, 0x55, 0x55, 0x00, 0xff },
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
float kaijuTotalTime;
int pointIndex;
float timeBetweenPoints;

vec2 walkPoints[] = {
  { 48.f, 48.f },
  { 96.f, 48.f },
  { 96.f, 96.f },
  { 48.f, 96.f },
};

#define NUMBER_OF_WALK_POINTS (sizeof(walkPoints)/sizeof(walkPoints[0]))
#define LOOPED_WALK_POINT(index) ((index + NUMBER_OF_WALK_POINTS) % NUMBER_OF_WALK_POINTS)

void initKaiju1() {
  int i;

  kaijuTime = 0.f;
  kaijuTotalTime = 0.f;
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
  hitboxes[0].rotation = (vec3){ 0.f, 0.f, 0.f };
  hitboxes[0].scale = (vec3){ 8.f, 6.f, 7.f  };
  guMtxIdentF(hitboxes[0].computedTransform.data);
  hitboxes[0].displayCommands = blue_cube_commands;
  hitboxes[0].check = sdSphere;
  hitboxes[0].numberOfChildren = 0;
  for (i = 0; i < MAX_CHILDREN_PER_HITBOX; i++) {
    hitboxes[0].children[i] = NULL;
  }
  hitboxes[0].parent = NULL;

  hitboxes[2].alive = 1;
  hitboxes[2].destroyable = 0;
  hitboxes[2].isTransformDirty = 1;
  hitboxes[2].position = (vec3){ 0.3f, 0.3f, -1.9f };
  hitboxes[2].rotation = (vec3){ 7.f, 0.f, 0.f };
  hitboxes[2].scale = (vec3){ 0.125f, 0.125f, 3.f };
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
  hitboxes[3].position = (vec3){ -0.3f, 0.3f, -1.9f };
  hitboxes[3].rotation = (vec3){ 7.f, 0.f, 0.f };
  hitboxes[3].scale = (vec3){ 0.125f, 0.125f, 3.f };
  guMtxIdentF(hitboxes[3].computedTransform.data);
  hitboxes[3].displayCommands = blue_octahedron_commands;
  hitboxes[3].check = sdOctahedron;
  hitboxes[3].numberOfChildren = 0;
  for (i = 0; i < MAX_CHILDREN_PER_HITBOX; i++) {
    hitboxes[3].children[i] = NULL;
  }
  hitboxes[3].parent = NULL;
  parentHitboxes(&(hitboxes[3]), &(hitboxes[0]));

  hitboxes[6].alive = 1;
  hitboxes[6].destroyable = 0;
  hitboxes[6].isTransformDirty = 1;
  hitboxes[6].position = (vec3){ 0.3f, -0.3f, -1.9f };
  hitboxes[6].rotation = (vec3){ -7.f, 0.f, 0.f };
  hitboxes[6].scale = (vec3){ 0.125f, 0.125f, 3.f };
  guMtxIdentF(hitboxes[6].computedTransform.data);
  hitboxes[6].displayCommands = blue_octahedron_commands;
  hitboxes[6].check = sdOctahedron;
  hitboxes[6].numberOfChildren = 0;
  for (i = 0; i < MAX_CHILDREN_PER_HITBOX; i++) {
    hitboxes[6].children[i] = NULL;
  }
  hitboxes[6].parent = NULL;
  parentHitboxes(&(hitboxes[6]), &(hitboxes[0]));

  hitboxes[7].alive = 1;
  hitboxes[7].destroyable = 0;
  hitboxes[7].isTransformDirty = 1;
  hitboxes[7].position = (vec3){ -0.3f, -0.3f, -1.9f };
  hitboxes[7].rotation = (vec3){ -7.f, 0.f, 0.f };
  hitboxes[7].scale = (vec3){ 0.125f, 0.125f, 3.f };
  guMtxIdentF(hitboxes[7].computedTransform.data);
  hitboxes[7].displayCommands = blue_octahedron_commands;
  hitboxes[7].check = sdOctahedron;
  hitboxes[7].numberOfChildren = 0;
  for (i = 0; i < MAX_CHILDREN_PER_HITBOX; i++) {
    hitboxes[7].children[i] = NULL;
  }
  hitboxes[7].parent = NULL;
  parentHitboxes(&(hitboxes[7]), &(hitboxes[0]));

  // Create an appendage that's a hitbox
  hitboxes[1].alive = 1;
  hitboxes[1].destroyable = 1;
  hitboxes[1].isTransformDirty = 1;
  hitboxes[1].position = (vec3){ 0, 1.5f, 0.f };
  hitboxes[1].rotation = (vec3){ 0.f, 1.f, 0.f };
  hitboxes[1].scale = (vec3){ 1.78f / hitboxes[0].scale.x, 1.78f / hitboxes[0].scale.y, 1.78f / hitboxes[0].scale.z };
  guMtxIdentF(hitboxes[1].computedTransform.data);
  hitboxes[1].displayCommands = red_octahedron_commands;
  hitboxes[1].check = sdOctahedron;
  hitboxes[1].numberOfChildren = 0;
  for (i = 0; i < MAX_CHILDREN_PER_HITBOX; i++) {
    hitboxes[1].children[i] = NULL;
  }
  hitboxes[1].parent = NULL;
  parentHitboxes(&(hitboxes[1]), &(hitboxes[0]));

  hitboxes[4].alive = 1;
  hitboxes[4].destroyable = 1;
  hitboxes[4].isTransformDirty = 1;
  hitboxes[4].position = (vec3){ 0, -1.5f, 0.f };
  hitboxes[4].rotation = (vec3){ 0.f, 0.f, 0.f };
  hitboxes[4].scale = (vec3){ 1.7f / hitboxes[0].scale.x, 1.7f / hitboxes[0].scale.y, 1.7f / hitboxes[0].scale.z };
  guMtxIdentF(hitboxes[4].computedTransform.data);
  hitboxes[4].displayCommands = red_octahedron_commands;
  hitboxes[4].check = sdOctahedron;
  hitboxes[4].numberOfChildren = 0;
  for (i = 0; i < MAX_CHILDREN_PER_HITBOX; i++) {
    hitboxes[4].children[i] = NULL;
  }
  hitboxes[4].parent = NULL;
  parentHitboxes(&(hitboxes[4]), &(hitboxes[0]));

  hitboxes[5].alive = 1;
  hitboxes[5].destroyable = 1;
  hitboxes[5].isTransformDirty = 1;
  hitboxes[5].position = (vec3){ 0.f, 0.0f, 1.7f };
  hitboxes[5].rotation = (vec3){ 0.f, 0.f, 0.f };
  hitboxes[5].scale = (vec3){ 1.f / hitboxes[0].scale.x, 1.2f / hitboxes[0].scale.y, 1.2f / hitboxes[0].scale.z };
  guMtxIdentF(hitboxes[5].computedTransform.data);
  hitboxes[5].displayCommands = red_octahedron_commands;
  hitboxes[5].check = sdOctahedron;
  hitboxes[5].numberOfChildren = 0;
  for (i = 0; i < MAX_CHILDREN_PER_HITBOX; i++) {
    hitboxes[5].children[i] = NULL;
  }
  hitboxes[5].parent = NULL;
  parentHitboxes(&(hitboxes[5]), &(hitboxes[0]));
}

void updateKaiju1(float deltaSeconds) {
  float tVal = 0.f;
  const vec3 oldPos = hitboxes[0].position;
  float oldPosAngle;
  kaijuTime += deltaSeconds;
  kaijuTotalTime += deltaSeconds;
  if (kaijuTime > timeBetweenPoints) {
    pointIndex = LOOPED_WALK_POINT(pointIndex + 1);
    kaijuTime = 0.f;
  }

  tVal = kaijuTime / timeBetweenPoints;

	hitboxes[0].position.x = catmullRom(tVal, walkPoints[LOOPED_WALK_POINT(pointIndex - 1)].x, walkPoints[LOOPED_WALK_POINT(pointIndex)].x, walkPoints[LOOPED_WALK_POINT(pointIndex + 1)].x, walkPoints[LOOPED_WALK_POINT(pointIndex + 2)].x);
  hitboxes[0].position.y = catmullRom(tVal, walkPoints[LOOPED_WALK_POINT(pointIndex - 1)].y, walkPoints[LOOPED_WALK_POINT(pointIndex)].y, walkPoints[LOOPED_WALK_POINT(pointIndex + 1)].y, walkPoints[LOOPED_WALK_POINT(pointIndex + 2)].y);
  hitboxes[0].position.z = getHeight(hitboxes[0].position.x, hitboxes[0].position.y) + 25.f;
  
  hitboxes[0].rotation.x = RAD_TO_DEG * sinf(kaijuTotalTime * 0.5f) * 0.147272f;

  oldPosAngle = nu_atan2(oldPos.y - hitboxes[0].position.y, oldPos.x - hitboxes[0].position.x);
  hitboxes[0].rotation.z = RAD_TO_DEG * (oldPosAngle + (M_PI * 0.5f));

  hitboxes[0].isTransformDirty = 1;
}

void renderKaiju1(DisplayData* dynamicp) {
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
