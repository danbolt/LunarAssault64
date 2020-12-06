
#include "kaiju3.h"

#include "hitboxes.h"
#include "gamemath.h"
#include "map.h"

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

static float isRunning;

static float kaijuTime;
static float legTime;
static int pointIndex;
static float timeBetweenPoints;

static int bufferTickCount;

#define FIRST_STATE 0
#define SECOND_STATE 1

static u8 k3State;

vec2 firstStateWalkPoints[] = {
  {  12.f,  12.f },
  {  12.f,  32.f },
  {  32.f,  32.f },
  {  32.f,  12.f },
};

vec2 secondStateJumpPoints[] = {
  {          32.f,          32.f },
  {          32.f,  128.f - 32.f },
  {  128.f - 32.f,  128.f - 32.f },
  {  128.f - 32.f,          32.f },
};


#define NUMBER_OF_WALK_POINTS(pts) (sizeof(pts)/sizeof(pts[0]))
#define LOOPED_WALK_POINT(pts, index) ((index + NUMBER_OF_WALK_POINTS(pts)) % NUMBER_OF_WALK_POINTS(pts))

extern vec3 playerPos;
extern vec3 cameraRotation;

void initKaiju3() {
  int i;

  kaijuTime = 0.f;
  legTime = 0.f;
  pointIndex = 0;
  timeBetweenPoints = 2.f;

  isRunning = 1;

  k3State = FIRST_STATE;

  bufferTickCount = 0;

  playerPos = (vec3){96, 48, 0};
  cameraRotation = (vec3){0, -0.2f, 170};

	 for (i = 0; i < NUMBER_OF_KAIJU_HITBOXES; i++) {
    hitboxes[0].alive = 0;
  }

  // The body
  hitboxes[0].alive = 1;
  hitboxes[0].destroyable = 0;
  hitboxes[0].isTransformDirty = 1;
  hitboxes[0].position = (vec3){ 64.f, 64.f, 10.5f };
  hitboxes[0].rotation = (vec3){ 22.5f, 0.f, 0.f };
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

  // hitboxes
  hitboxes[3].alive = 1;
  hitboxes[3].destroyable = 1;
  hitboxes[3].isTransformDirty = 1;
  hitboxes[3].position = (vec3){ 1.2f, 0.95f, 1.1f };
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

  hitboxes[6].alive = 1;
  hitboxes[6].destroyable = 1;
  hitboxes[6].isTransformDirty = 1;
  hitboxes[6].position = (vec3){ -1.2f, 0.95f, 1.1f };
  hitboxes[6].rotation = (vec3){ 0.f, 0.f, 0.f };
  hitboxes[6].scale = (vec3){ 1.4f * (1.f / hitboxes[0].scale.x), 1.4f * (1.f / hitboxes[0].scale.y), 1.4f * (1.f / hitboxes[0].scale.z) };
  guMtxIdentF(hitboxes[6].computedTransform.data);
  hitboxes[6].displayCommands = red_octahedron_commands;
  hitboxes[6].check = sdOctahedron;
  hitboxes[6].numberOfChildren = 0;
  for (i = 0; i < MAX_CHILDREN_PER_HITBOX; i++) {
    hitboxes[6].children[i] = NULL;
  }
  hitboxes[6].parent = NULL;
  parentHitboxes(&(hitboxes[6]), &(hitboxes[0]));

  hitboxes[7].alive = 1;
  hitboxes[7].destroyable = 1;
  hitboxes[7].isTransformDirty = 1;
  hitboxes[7].position = (vec3){ 0.f, 0.5f, -1.f };
  hitboxes[7].rotation = (vec3){ 0.f, 0.f, 0.f };
  hitboxes[7].scale = (vec3){ 1.4f * (1.f / hitboxes[0].scale.x), 1.4f * (1.f / hitboxes[0].scale.y), 1.4f * (1.f / hitboxes[0].scale.z) };
  guMtxIdentF(hitboxes[7].computedTransform.data);
  hitboxes[7].displayCommands = red_octahedron_commands;
  hitboxes[7].check = sdOctahedron;
  hitboxes[7].numberOfChildren = 0;
  for (i = 0; i < MAX_CHILDREN_PER_HITBOX; i++) {
    hitboxes[7].children[i] = NULL;
  }
  hitboxes[7].parent = NULL;
  parentHitboxes(&(hitboxes[7]), &(hitboxes[0]));
}

void updateKaiju3(float deltaSeconds) {
  float tVal = 0.f;
  const vec3 oldPos = hitboxes[0].position;
  int remainingHitboxes = 0;
  float oldAngle;
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

  if (k3State == FIRST_STATE) {
    if (remainingHitboxes < 3) {
      kaijuTime = 0.f;
      k3State = SECOND_STATE;
      isRunning = 0;
      pointIndex = 1;
      return;
    }

    if (isRunning) {
      if (kaijuTime > timeBetweenPoints) {
        isRunning = 0;
        pointIndex = LOOPED_WALK_POINT(firstStateWalkPoints, pointIndex + 1);
        kaijuTime = 0.f;
      }

      tVal = kaijuTime / timeBetweenPoints;

      legTime += deltaSeconds * 4.f;

      hitboxes[0].position.x = catmullRom(tVal, firstStateWalkPoints[LOOPED_WALK_POINT(firstStateWalkPoints, pointIndex - 1)].x, firstStateWalkPoints[LOOPED_WALK_POINT(firstStateWalkPoints, pointIndex)].x, firstStateWalkPoints[LOOPED_WALK_POINT(firstStateWalkPoints, pointIndex + 1)].x, firstStateWalkPoints[LOOPED_WALK_POINT(firstStateWalkPoints, pointIndex + 2)].x);
      hitboxes[0].position.y = catmullRom(tVal, firstStateWalkPoints[LOOPED_WALK_POINT(firstStateWalkPoints, pointIndex - 1)].y, firstStateWalkPoints[LOOPED_WALK_POINT(firstStateWalkPoints, pointIndex)].y, firstStateWalkPoints[LOOPED_WALK_POINT(firstStateWalkPoints, pointIndex + 1)].y, firstStateWalkPoints[LOOPED_WALK_POINT(firstStateWalkPoints, pointIndex + 2)].y);
      hitboxes[0].position.z = getHeight(hitboxes[0].position.x, hitboxes[0].position.y) + 2.f;
      oldAngle = nu_atan2(oldPos.y - hitboxes[0].position.y, oldPos.x - hitboxes[0].position.x);
      hitboxes[0].rotation.z = RAD_TO_DEG * (oldAngle + (M_PI * 0.5f));
      hitboxes[0].isTransformDirty = 1;

      hitboxes[1].rotation.y = sinf(legTime) * 20.f + 5.f;
      hitboxes[1].isTransformDirty = 1;
      hitboxes[2].rotation.y = 30.f + sinf(2.f * legTime + sinf(legTime)) * 4.f;
      hitboxes[2].isTransformDirty = 1;

      hitboxes[1 + 3].rotation.y = sinf(legTime) * 20.f + 5.f;
      hitboxes[1 + 3].isTransformDirty = 1;
      hitboxes[2 + 3].rotation.y = 30.f + sinf(2.f * legTime + sinf(legTime)) * 4.f;
      hitboxes[2 + 3].isTransformDirty = 1;
    } else {
      if (kaijuTime > 7.f) {
        kaijuTime = 0.f;
        isRunning = 1;
        hitboxes[0].rotation.x = 12.5f;
        hitboxes[0].isTransformDirty = 1;
      } else {
        hitboxes[0].rotation.x = 12.5f + (sinf(M_PI * (kaijuTime / 7.f)) * 8.f);
        hitboxes[0].isTransformDirty = 1;
      }
    }
  } else if (k3State = SECOND_STATE) {
    float dx = hitboxes[0].position.x - secondStateJumpPoints[pointIndex].x;
    float dy = hitboxes[0].position.y - secondStateJumpPoints[pointIndex].y;

    dx = dx * dx;
    dy = dy * dy;

    if (dx + dy < 0.8f) {
      pointIndex = (pointIndex + 1) % 4;
    }

    hitboxes[0].position.x = lerp(hitboxes[0].position.x, secondStateJumpPoints[pointIndex].x, deltaSeconds * 0.5f);
    hitboxes[0].position.y = lerp(hitboxes[0].position.y, secondStateJumpPoints[pointIndex].y, deltaSeconds * 0.5f);
    hitboxes[0].position.z = MAX(getHeight(hitboxes[0].position.x, hitboxes[0].position.y) + 2.f, sinf((dx + dy) / 4096 * M_PI) * 34.f);
    hitboxes[0].rotation.x = 30.f;
    hitboxes[0].rotation.z = RAD_TO_DEG * (nu_atan2(64 - hitboxes[0].position.y, 64 - hitboxes[0].position.x) - (M_PI * 0.5f));
    hitboxes[0].isTransformDirty = 1;
  }
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
