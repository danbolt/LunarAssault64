
#include "kaiju3.h"

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

static Vtx blue_box_geo[] = {
  {  1,  1,  1, 0,  7 << 6, 24 << 6, 0, 0, 0xff, 0xff },
  { -1,  1,  1, 0,  0 << 6, 24 << 6, 0, 0, 0xff, 0xff },
  { -1, -1,  1, 0,  7 << 6, 31 << 6, 0, 0, 0xff, 0xff },
  {  1, -1,  1, 0,  0 << 6, 31 << 6, 0, 0, 0xff, 0xff },
  {  1,  1, -1, 0,  0 << 6, 24 << 6, 0, 0, 0xff, 0xff },
  { -1,  1, -1, 0,  0 << 6, 24 << 6, 0, 0, 0xff, 0xff },
  { -1, -1, -1, 0,  0 << 6, 31 << 6, 0, 0, 0xff, 0xff },
  {  1, -1, -1, 0,  0 << 6, 31 << 6, 0, 0, 0xff, 0xff },
};

static Gfx blue_box_commands[] = {
  gsSPVertex(&blue_box_geo, 8, 0),
  gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
  gsSP2Triangles(4, 5, 0, 0, 5, 1, 0, 0),
  gsSP2Triangles(5, 6, 2, 0, 5, 2, 1, 0),
  gsSP2Triangles(7, 4, 0, 0, 7, 0, 3, 0),
  gsSP2Triangles(6, 4, 7, 0, 6, 5, 4, 0),
  gsSP2Triangles(6, 7, 3, 0, 6, 3, 2, 0),
  gsSPEndDisplayList()
};

static Vtx leg_geo[] = {
  {  1,  1,  1, 0,  7 << 6, 24 << 6, 0, 0, 0xa1, 0xff },
  {  0,  1,  1, 0,  0 << 6, 24 << 6, 0, 0, 0xa1, 0xff },
  {  0, -1,  1, 0,  7 << 6, 31 << 6, 0, 0, 0xa1, 0xff },
  {  1, -1,  1, 0,  0 << 6, 31 << 6, 0, 0, 0xa1, 0xff },
  {  1,  1, -1, 0,  0 << 6, 24 << 6, 0, 0, 0xa1, 0xff },
  {  0,  1, -1, 0,  0 << 6, 24 << 6, 0, 0, 0xa1, 0xff },
  {  0, -1, -1, 0,  0 << 6, 31 << 6, 0, 0, 0xa1, 0xff },
  {  1, -1, -1, 0,  0 << 6, 31 << 6, 0, 0, 0xa1, 0xff },
};

static Gfx leg_commands[] = {
  gsSPVertex(&leg_geo, 8, 0),
  gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
  gsSP2Triangles(4, 5, 0, 0, 5, 1, 0, 0),
  gsSP2Triangles(5, 6, 2, 0, 5, 2, 1, 0),
  gsSP2Triangles(7, 4, 0, 0, 7, 0, 3, 0),
  gsSP2Triangles(6, 4, 7, 0, 6, 5, 4, 0),
  gsSP2Triangles(6, 7, 3, 0, 6, 3, 2, 0),
  gsSPEndDisplayList()
};

float kaijuTime;
float legTime;
int pointIndex;
float timeBetweenPoints;

int bufferTickCount;

#define NUMBER_OF_WALK_POINTS (sizeof(walkPoints3)/sizeof(walkPoints3[0]))
#define LOOPED_WALK_POINT(index) ((index + NUMBER_OF_WALK_POINTS) % NUMBER_OF_WALK_POINTS)

extern vec3 playerPos;
extern vec3 cameraRotation;

void initKaiju3() {
  int i;

  kaijuTime = 0.f;
  legTime = 0.f;
  pointIndex = 0;
  timeBetweenPoints = 10.f;

  bufferTickCount = 0;

  playerPos = (vec3){48, 48, 0};
  cameraRotation = (vec3){0, -0.2f, 170};

	 for (i = 0; i < NUMBER_OF_KAIJU_HITBOXES; i++) {
    hitboxes[0].alive = 0;
  }

  // The body
  hitboxes[0].alive = 1;
  hitboxes[0].destroyable = 0;
  hitboxes[0].isTransformDirty = 1;
  hitboxes[0].position = (vec3){ 64.f, 64.f, 10.5f };
  hitboxes[0].rotation = (vec3){ 0.f, 0.f, 0.f };
  hitboxes[0].scale = (vec3){ 3.f, 7.f, 2.0f };
  guMtxIdentF(hitboxes[0].computedTransform.data);
  hitboxes[0].displayCommands = blue_box_commands;
  hitboxes[0].check = sdOctahedron;
  hitboxes[0].numberOfChildren = 0;
  for (i = 0; i < MAX_CHILDREN_PER_HITBOX; i++) {
    hitboxes[0].children[i] = NULL;
  }
  hitboxes[0].parent = NULL;

  // a leg
  hitboxes[1].alive = 1;
  hitboxes[1].destroyable = 0;
  hitboxes[1].isTransformDirty = 1;
  hitboxes[1].position = (vec3){ 1.f, 0.75f, 0.f };
  hitboxes[1].rotation = (vec3){ 0.f, -20.f, 0.f };
  hitboxes[1].scale = (vec3){ 0.33333f, (1.f / 7.f), 1.f };
  guMtxIdentF(hitboxes[1].computedTransform.data);
  hitboxes[1].displayCommands = leg_commands;
  hitboxes[1].check = sdOctahedron;
  hitboxes[1].numberOfChildren = 0;
  for (i = 0; i < MAX_CHILDREN_PER_HITBOX; i++) {
    hitboxes[1].children[i] = NULL;
  }
  hitboxes[1].parent = NULL;
  parentHitboxes(&(hitboxes[1]), &(hitboxes[0]));

  // a shin
  hitboxes[2].alive = 1;
  hitboxes[2].destroyable = 0;
  hitboxes[2].isTransformDirty = 1;
  hitboxes[2].position = (vec3){ 1.f, 0.f, 0.f };
  hitboxes[2].rotation = (vec3){ 0.f, 20.f, 0.f };
  hitboxes[2].scale = (vec3){ 5.f, 1.f, 1.f };
  guMtxIdentF(hitboxes[2].computedTransform.data);
  hitboxes[2].displayCommands = leg_commands;
  hitboxes[2].check = sdOctahedron;
  hitboxes[2].numberOfChildren = 0;
  for (i = 0; i < MAX_CHILDREN_PER_HITBOX; i++) {
    hitboxes[2].children[i] = NULL;
  }
  hitboxes[2].parent = NULL;
  parentHitboxes(&(hitboxes[2]), &(hitboxes[1]));

  hitboxes[3].alive = 1;
  hitboxes[3].destroyable = 1;
  hitboxes[3].isTransformDirty = 1;
  hitboxes[3].position = (vec3){ 1.f, 0.75f, 1.8f };
  hitboxes[3].rotation = (vec3){ 0.f, 0.f, 0.f };
  hitboxes[3].scale = (vec3){ 1.4f * (1.f / hitboxes[0].scale.x), 1.4f * (1.f / hitboxes[0].scale.y), 1.4f * (1.f / hitboxes[0].scale.z) };
  guMtxIdentF(hitboxes[3].computedTransform.data);
  hitboxes[3].displayCommands = red_octahedron_commands;
  hitboxes[3].check = sdOctahedron;
  hitboxes[3].numberOfChildren = 0;
  for (i = 0; i < MAX_CHILDREN_PER_HITBOX; i++) {
    hitboxes[3].children[i] = NULL;
  }
  hitboxes[3].parent = NULL;
  parentHitboxes(&(hitboxes[3]), &(hitboxes[0]));

  // leg 2
  hitboxes[1 + 3].alive = 1;
  hitboxes[1 + 3].destroyable = 0;
  hitboxes[1 + 3].isTransformDirty = 1;
  hitboxes[1 + 3].position = (vec3){ -1.f, 0.75f, 0.f };
  hitboxes[1 + 3].rotation = (vec3){ 0.f, -20.f, 0.f };
  hitboxes[1 + 3].scale = (vec3){ -0.33333f, (1.f / 7.f), 1.f };
  guMtxIdentF(hitboxes[1 + 3].computedTransform.data);
  hitboxes[1 + 3].displayCommands = leg_commands;
  hitboxes[1 + 3].check = sdOctahedron;
  hitboxes[1 + 3].numberOfChildren = 0;
  for (i = 0; i < MAX_CHILDREN_PER_HITBOX; i++) {
    hitboxes[1 + 3].children[i] = NULL;
  }
  hitboxes[1 + 3].parent = NULL;
  parentHitboxes(&(hitboxes[1 + 3]), &(hitboxes[0]));

  // a shin
  hitboxes[2 + 3].alive = 1;
  hitboxes[2 + 3].destroyable = 0;
  hitboxes[2 + 3].isTransformDirty = 1;
  hitboxes[2 + 3].position = (vec3){ 1.f, 0.f, 0.f };
  hitboxes[2 + 3].rotation = (vec3){ 0.f, 20.f, 0.f };
  hitboxes[2 + 3].scale = (vec3){ 5.f, 1.f, 1.f };
  guMtxIdentF(hitboxes[2 + 3].computedTransform.data);
  hitboxes[2 + 3].displayCommands = leg_commands;
  hitboxes[2 + 3].check = sdOctahedron;
  hitboxes[2 + 3].numberOfChildren = 0;
  for (i = 0; i < MAX_CHILDREN_PER_HITBOX; i++) {
    hitboxes[2 + 3].children[i] = NULL;
  }
  hitboxes[2 + 3].parent = NULL;
  parentHitboxes(&(hitboxes[2 + 3]), &(hitboxes[1 + 3]));
}

void updateKaiju3(float deltaSeconds) {
  float tVal = 0.f;
  int remainingHitboxes = 0;
  int i;

  if (bufferTickCount < 5) {
    bufferTickCount++;
    return;
  }

  kaijuTime += deltaSeconds;
  legTime += deltaSeconds * 4.f;

  hitboxes[0].position.z = getHeight(hitboxes[0].position.x, hitboxes[0].position.y) + 7.f;
  hitboxes[0].isTransformDirty = 1;

  hitboxes[1].rotation.y = sinf(legTime) * 20.f + 5.f;
  hitboxes[1].isTransformDirty = 1;
  hitboxes[2].rotation.y = 30.f + sinf(2.f * legTime + sinf(legTime)) * 4.f;
  hitboxes[2].isTransformDirty = 1;


  hitboxes[1 + 3].rotation.y = sinf(legTime) * 20.f + 5.f;
  hitboxes[1 + 3].isTransformDirty = 1;
  hitboxes[2 + 3].rotation.y = 30.f + sinf(2.f * legTime + sinf(legTime)) * 4.f;
  hitboxes[2 + 3].isTransformDirty = 1;
}

void renderKaiju3(DisplayData* dynamicp) {
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
