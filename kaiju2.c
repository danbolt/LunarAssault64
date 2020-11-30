
#include "kaiju2.h"

#include "hitboxes.h"
#include "gamemath.h"
#include "map.h"

#define ORBIT_ROTATION_SPEED 14.4f

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

static Gfx hidden_joint_commands[] = {
  gsSPEndDisplayList()
};

static Vtx blue_octahedron_geo[] = {
  {  1,  0,  0, 0, 15 << 6, 0 << 6, 0x1C, 0x78, 0x99, 0xfc },
  { -1,  0,  0, 0, 15 << 6, 0 << 6, 0x1C, 0x88, 0xA3, 0xfc },
  {  0,  1,  0, 0, 15 << 6, 15 << 6, 0x1C, 0x68, 0x99, 0xfc },
  {  0, -1,  0, 0, 15 << 6, 15 << 6, 0x1C, 0x88, 0x99, 0xfc },
  {  0,  0,  1, 0, 0 << 6, 0 << 6, 0xFF, 0xFF, 0xFF, 0xfc },
  {  0,  0, -1, 0, 0 << 6, 0 << 6, 0x00, 0x00, 0x00, 0xfc },
};

static Gfx eva_inspired_commands[] = {
  gsDPPipeSync(),
  gsDPSetCycleType(G_CYC_2CYCLE),
  gsDPSetRenderMode(G_RM_PASS, G_RM_ZB_OPA_SURF2),
  gsDPSetCombineLERP(NOISE, 0, TEXEL0, SHADE, 0, 0, 0, SHADE, NOISE, 0, ENVIRONMENT, COMBINED, 0, 0, 0, SHADE),

  gsSPVertex(&blue_octahedron_geo, 6, 0),
  gsSP2Triangles(0, 2, 4, 0, 0, 5, 2, 0),
  gsSP2Triangles(0, 4, 3, 0, 0, 3, 5, 0),
  gsSP2Triangles(1, 3, 4, 0, 1, 5, 3, 0),
  gsSP2Triangles(1, 4, 2, 0, 1, 2, 5, 0),

  gsDPPipeSync(),
  gsDPSetCycleType(G_CYC_1CYCLE),
  gsDPSetRenderMode(G_RM_ZB_OPA_SURF, G_RM_ZB_OPA_SURF2),
  gsDPSetCombineLERP(NOISE, LOD_FRACTION, ENVIRONMENT, SHADE, 0, 0, 0, SHADE, NOISE, 0, ENVIRONMENT, SHADE, 0, 0, 0, SHADE),
  gsSPEndDisplayList()
};

float kaijuTime;
int pointIndex;
float timeBetweenPoints;

int bufferTickCount;

vec2 walkPoints2[] = {
  { 50.f, 16.f },
  { 50.f, 64.f },
  { 40.f, 70.f },
  { 0.f, 70.f },
  { 10.f, 70.f },
  { 30.f, 64.f },
  { 50.f, 16.f },
};
#define NUMBER_OF_WALK_POINTS (sizeof(walkPoints2)/sizeof(walkPoints2[0]))
#define LOOPED_WALK_POINT(index) ((index + NUMBER_OF_WALK_POINTS) % NUMBER_OF_WALK_POINTS)

extern vec3 playerPos;
extern vec3 cameraRotation;

void initKaiju2() {
  int i;

  kaijuTime = 0.f;
  pointIndex = 0;
  timeBetweenPoints = 10.f;

  bufferTickCount = 0;

  playerPos = (vec3){125, 125, 0};
  cameraRotation = (vec3){0, -0.2f, 170};

	 for (i = 0; i < NUMBER_OF_KAIJU_HITBOXES; i++) {
    hitboxes[0].alive = 0;
  }

  // Create a "body piece"
  hitboxes[0].alive = 1;
  hitboxes[0].destroyable = 0;
  hitboxes[0].isTransformDirty = 1;
  hitboxes[0].position = (vec3){ walkPoints2[0].x, walkPoints2[0].y, 45.5f };
  hitboxes[0].rotation = (vec3){ 0.f, 0.f, 0.f };
  hitboxes[0].scale = (vec3){ 12.f, 12.f, 12.f };
  guMtxIdentF(hitboxes[0].computedTransform.data);
  hitboxes[0].displayCommands = eva_inspired_commands;
  hitboxes[0].check = sdOctahedron;
  hitboxes[0].numberOfChildren = 0;
  for (i = 0; i < MAX_CHILDREN_PER_HITBOX; i++) {
    hitboxes[0].children[i] = NULL;
  }
  hitboxes[0].parent = NULL;

  hitboxes[1].alive = 1;
  hitboxes[1].destroyable = 0;
  hitboxes[1].isTransformDirty = 1;
  hitboxes[1].position = (vec3){ 0.f, 0.f, 0.f };
  hitboxes[1].rotation = (vec3){ 0.f, 0.f, 0.f };
  hitboxes[1].scale = (vec3){ 1.f / 8.f, 1.f / 8.f, 1.f / 8.f };
  guMtxIdentF(hitboxes[1].computedTransform.data);
  hitboxes[1].displayCommands = hidden_joint_commands;
  hitboxes[1].check = sdOctahedron;
  hitboxes[1].numberOfChildren = 0;
  for (i = 0; i < MAX_CHILDREN_PER_HITBOX; i++) {
    hitboxes[1].children[i] = NULL;
  }
  hitboxes[1].parent = NULL;
  parentHitboxes(&(hitboxes[1]), &(hitboxes[0]));

  hitboxes[9].alive = 1;
  hitboxes[9].destroyable = 0;
  hitboxes[9].isTransformDirty = 1;
  hitboxes[9].position = (vec3){ 0.f, 0.f, 0.f };
  hitboxes[9].rotation = (vec3){ 0.f, 0.f, 0.f };
  hitboxes[9].scale = (vec3){ 1.f / 8.f, 1.f / 8.f, 1.f / 8.f };
  guMtxIdentF(hitboxes[9].computedTransform.data);
  hitboxes[9].displayCommands = hidden_joint_commands;
  hitboxes[9].check = sdOctahedron;
  hitboxes[9].numberOfChildren = 0;
  for (i = 0; i < MAX_CHILDREN_PER_HITBOX; i++) {
    hitboxes[9].children[i] = NULL;
  }
  hitboxes[9].parent = NULL;
  parentHitboxes(&(hitboxes[9]), &(hitboxes[0]));

  hitboxes[3].alive = 1;
  hitboxes[3].destroyable = 1;
  hitboxes[3].isTransformDirty = 1;
  hitboxes[3].position = (vec3){ -9.5f, 0.f, 0.f };
  hitboxes[3].rotation = (vec3){ 0.f, 0.f, 0.f };
  hitboxes[3].scale = (vec3){ 1.4f, 1.4f, 1.4f };
  guMtxIdentF(hitboxes[3].computedTransform.data);
  hitboxes[3].displayCommands = red_octahedron_commands;
  hitboxes[3].check = sdOctahedron;
  hitboxes[3].numberOfChildren = 0;
  for (i = 0; i < MAX_CHILDREN_PER_HITBOX; i++) {
    hitboxes[3].children[i] = NULL;
  }
  hitboxes[3].parent = NULL;
  parentHitboxes(&(hitboxes[3]), &(hitboxes[1]));

  hitboxes[4].alive = 1;
  hitboxes[4].destroyable = 1;
  hitboxes[4].isTransformDirty = 1;
  hitboxes[4].position = (vec3){ 9.5f, 0.f, 0.f };
  hitboxes[4].rotation = (vec3){ 0.f, 0.f, 0.f };
  hitboxes[4].scale = (vec3){ 1.4f, 1.4f, 1.4f };
  guMtxIdentF(hitboxes[4].computedTransform.data);
  hitboxes[4].displayCommands = red_octahedron_commands;
  hitboxes[4].check = sdOctahedron;
  hitboxes[4].numberOfChildren = 0;
  for (i = 0; i < MAX_CHILDREN_PER_HITBOX; i++) {
    hitboxes[4].children[i] = NULL;
  }
  hitboxes[4].parent = NULL;
  parentHitboxes(&(hitboxes[4]), &(hitboxes[1]));

  hitboxes[5].alive = 1;
  hitboxes[5].destroyable = 1;
  hitboxes[5].isTransformDirty = 1;
  hitboxes[5].position = (vec3){ 0.f, 9.5f, 0.f };
  hitboxes[5].rotation = (vec3){ 0.f, 0.f, 0.f };
  hitboxes[5].scale = (vec3){ 1.4f, 1.4f, 1.4f };
  guMtxIdentF(hitboxes[5].computedTransform.data);
  hitboxes[5].displayCommands = red_octahedron_commands;
  hitboxes[5].check = sdOctahedron;
  hitboxes[5].numberOfChildren = 0;
  for (i = 0; i < MAX_CHILDREN_PER_HITBOX; i++) {
    hitboxes[5].children[i] = NULL;
  }
  hitboxes[5].parent = NULL;
  parentHitboxes(&(hitboxes[5]), &(hitboxes[1]));

  hitboxes[6].alive = 1;
  hitboxes[6].destroyable = 1;
  hitboxes[6].isTransformDirty = 1;
  hitboxes[6].position = (vec3){ 0.f, -9.5f, 0.f };
  hitboxes[6].rotation = (vec3){ 0.f, 0.f, 0.f };
  hitboxes[6].scale = (vec3){ 1.4f, 1.4f, 1.4f };
  guMtxIdentF(hitboxes[6].computedTransform.data);
  hitboxes[6].displayCommands = red_octahedron_commands;
  hitboxes[6].check = sdOctahedron;
  hitboxes[6].numberOfChildren = 0;
  for (i = 0; i < MAX_CHILDREN_PER_HITBOX; i++) {
    hitboxes[6].children[i] = NULL;
  }
  hitboxes[6].parent = NULL;
  parentHitboxes(&(hitboxes[6]), &(hitboxes[1]));

  hitboxes[7].alive = 1;
  hitboxes[7].destroyable = 1;
  hitboxes[7].isTransformDirty = 1;
  hitboxes[7].position = (vec3){ 0.f, 0.f, 9.5f };
  hitboxes[7].rotation = (vec3){ 0.f, 0.f, 0.f };
  hitboxes[7].scale = (vec3){ 1.4f, 1.4f, 1.4f };
  guMtxIdentF(hitboxes[7].computedTransform.data);
  hitboxes[7].displayCommands = red_octahedron_commands;
  hitboxes[7].check = sdOctahedron;
  hitboxes[7].numberOfChildren = 0;
  for (i = 0; i < MAX_CHILDREN_PER_HITBOX; i++) {
    hitboxes[7].children[i] = NULL;
  }
  hitboxes[7].parent = NULL;
  parentHitboxes(&(hitboxes[7]), &(hitboxes[9]));

  hitboxes[8].alive = 1;
  hitboxes[8].destroyable = 1;
  hitboxes[8].isTransformDirty = 1;
  hitboxes[8].position = (vec3){ 0.f, 0.f, -9.5f };
  hitboxes[8].rotation = (vec3){ 0.f, 0.f, 0.f };
  hitboxes[8].scale = (vec3){ 1.4f, 1.4f, 1.4f };
  guMtxIdentF(hitboxes[8].computedTransform.data);
  hitboxes[8].displayCommands = red_octahedron_commands;
  hitboxes[8].check = sdOctahedron;
  hitboxes[8].numberOfChildren = 0;
  for (i = 0; i < MAX_CHILDREN_PER_HITBOX; i++) {
    hitboxes[8].children[i] = NULL;
  }
  hitboxes[8].parent = NULL;
  parentHitboxes(&(hitboxes[8]), &(hitboxes[9]));
}

void updateKaiju2(float deltaSeconds) {
  float tVal = 0.f;
  int remainingHitboxes = 0;
  int i;

  if (bufferTickCount < 5) {
    bufferTickCount++;
    return;
  }

  for (i = 0; i < NUMBER_OF_KAIJU_HITBOXES; i++) {
    if (hitboxes[i].alive && hitboxes[i].destroyable) {
      remainingHitboxes++;
    }
  }


  kaijuTime += deltaSeconds;
  if (kaijuTime > timeBetweenPoints) {
    pointIndex = LOOPED_WALK_POINT(pointIndex + 1);
    kaijuTime = 0.f;
  }

  tVal = kaijuTime / timeBetweenPoints;

	hitboxes[0].position.x = catmullRom(tVal, walkPoints2[LOOPED_WALK_POINT(pointIndex - 1)].x, walkPoints2[LOOPED_WALK_POINT(pointIndex)].x, walkPoints2[LOOPED_WALK_POINT(pointIndex + 1)].x, walkPoints2[LOOPED_WALK_POINT(pointIndex + 2)].x);
  hitboxes[0].position.y = catmullRom(tVal, walkPoints2[LOOPED_WALK_POINT(pointIndex - 1)].y, walkPoints2[LOOPED_WALK_POINT(pointIndex)].y, walkPoints2[LOOPED_WALK_POINT(pointIndex + 1)].y, walkPoints2[LOOPED_WALK_POINT(pointIndex + 2)].y);
  hitboxes[0].position.z = getHeight(hitboxes[0].position.x, hitboxes[0].position.y) + 20.f;
  hitboxes[0].isTransformDirty = 1;

  hitboxes[1].rotation.y += deltaSeconds * ORBIT_ROTATION_SPEED;
  hitboxes[1].isTransformDirty = 1;

  hitboxes[9].rotation.x += deltaSeconds * ORBIT_ROTATION_SPEED;
  hitboxes[9].isTransformDirty = 1;
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
