
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

static Vtx kaiju1_verts[] = {
  { 67, -62, 17, 0, 0, 0, 114, 27, 105, 255 },
  { 98, -88, -34, 0, 0, 0, 29, 11, 15, 255 },
  { 84, 99, 74, 0, 0, 0, 72, 14, 52, 255 },
  { 96, 117, -31, 0, 0, 0, 54, 29, 209, 255 },
  { 104, -1, -35, 0, 0, 0, 28, 9, 14, 255 },
  { 69, 8, 54, 0, 0, 0, 114, 44, 110, 255 },
  { 98, 86, -34, 0, 0, 0, 54, 29, 209, 255 },
  { 78, 86, 103, 0, 0, 0, 199, 190, 209, 255 },
  { 68, 128, -21, 0, 0, 0, 71, 10, 51, 255 },
  { 73, 118, 1, 0, 0, 0, 71, 10, 51, 255 },
  { 49, 96, 15, 0, 0, 0, 36, 5, 26, 255 },
  { 18, 123, 70, 0, 0, 0, 71, 11, 51, 255 },
  { 12, 96, 123, 0, 0, 0, 71, 10, 51, 255 },
  { 78, 16, 85, 0, 0, 0, 37, 24, 102, 255 },
  { 91, 57, -39, 0, 0, 0, 19, 13, 48, 255 },
  { 68, 67, 58, 0, 0, 0, 37, 24, 102, 255 },
  { 74, 67, -28, 0, 0, 0, 18, 11, 48, 255 },
  { 50, 93, 75, 0, 0, 0, 41, 6, 29, 255 },
  { 20, 93, 72, 0, 0, 0, 37, 5, 26, 255 },
  { 12, 80, 100, 0, 0, 0, 161, 45, 56, 255 },
  { -67, -62, 17, 0, 0, 0, 114, 27, 105, 255 },
  { -98, -88, -34, 0, 0, 0, 29, 11, 15, 255 },
  { -84, 99, 74, 0, 0, 0, 72, 14, 52, 255 },
  { -96, 117, -31, 0, 0, 0, 54, 29, 209, 255 },
  { 0, -112, -38, 0, 0, 0, 28, 9, 14, 255 },
  { 0, 119, 96, 0, 0, 0, 54, 29, 209, 255 },
  { 0, 131, -37, 0, 0, 0, 163, 33, 62, 255 },
  { 0, -72, 49, 0, 0, 0, 190, 26, 176, 255 },
  { -104, -1, -35, 0, 0, 0, 28, 9, 14, 255 },
  { -69, 8, 54, 0, 0, 0, 114, 44, 110, 255 },
  { 0, -3, -52, 0, 0, 0, 28, 10, 14, 255 },
  { 0, 17, 76, 0, 0, 0, 196, 27, 181, 255 },
  { -98, 86, -34, 0, 0, 0, 54, 29, 209, 255 },
  { 0, 86, -50, 0, 0, 0, 64, 9, 59, 255 },
  { 0, 86, 146, 0, 0, 0, 55, 30, 209, 255 },
  { -78, 86, 103, 0, 0, 0, 199, 190, 209, 255 },
  { 0, 133, -1, 0, 0, 0, 71, 10, 51, 255 },
  { -68, 128, -21, 0, 0, 0, 71, 10, 51, 255 },
  { -73, 118, 1, 0, 0, 0, 71, 10, 51, 255 },
  { 0, 131, 34, 0, 0, 0, 161, 44, 57, 255 },
  { 0, 96, 29, 0, 0, 0, 36, 5, 26, 255 },
  { -49, 96, 15, 0, 0, 0, 36, 5, 26, 255 },
  { -18, 123, 70, 0, 0, 0, 71, 11, 51, 255 },
  { -12, 96, 123, 0, 0, 0, 71, 10, 51, 255 },
  { -78, 16, 85, 0, 0, 0, 37, 24, 102, 255 },
  { -91, 57, -39, 0, 0, 0, 19, 13, 48, 255 },
  { 0, 57, -61, 0, 0, 0, 22, 17, 50, 255 },
  { 0, 27, 116, 0, 0, 0, 122, 115, 219, 255 },
  { -68, 67, 58, 0, 0, 0, 37, 24, 102, 255 },
  { -74, 67, -28, 0, 0, 0, 18, 11, 48, 255 },
  { 0, 67, -49, 0, 0, 0, 46, 37, 38, 255 },
  { 0, 67, 89, 0, 0, 0, 21, 16, 49, 255 },
  { -50, 93, 75, 0, 0, 0, 41, 6, 29, 255 },
  { -20, 93, 72, 0, 0, 0, 37, 5, 26, 255 },
  { -12, 80, 100, 0, 0, 0, 161, 45, 56, 255 },
  { 0, -116, 32, 0, 0, 0, 113, 23, 104, 255 },
};
static Mtx kaiju1Scale;

static Gfx kaiju1_commands[] = {
  gsSPMatrix(OS_K0_TO_PHYSICAL(&(kaiju1Scale)), G_MTX_MODELVIEW | G_MTX_PUSH),
  gsSPVertex(&kaiju1_verts, 56, 0),
  gsSP2Triangles(34, 12, 25, 0, 36, 8, 26, 0),
  gsSP2Triangles(4, 0, 1, 0, 1, 27, 55, 0),
  gsSP2Triangles(6, 26, 3, 0, 1, 30, 4, 0),
  gsSP2Triangles(16, 5, 4, 0, 27, 5, 31, 0),
  gsSP2Triangles(3, 7, 6, 0, 14, 33, 6, 0),
  gsSP2Triangles(47, 7, 34, 0, 26, 8, 3, 0),
  gsSP2Triangles(8, 9, 3, 0, 2, 3, 9, 0),
  gsSP2Triangles(9, 39, 11, 0, 39, 10, 40, 0),
  gsSP2Triangles(9, 8, 10, 0, 36, 40, 10, 0),
  gsSP2Triangles(2, 9, 11, 0, 11, 39, 25, 0),
  gsSP2Triangles(34, 7, 12, 0, 12, 11, 25, 0),
  gsSP2Triangles(2, 12, 7, 0, 51, 13, 47, 0),
  gsSP2Triangles(16, 46, 14, 0, 6, 13, 14, 0),
  gsSP2Triangles(4, 50, 16, 0, 31, 15, 51, 0),
  gsSP2Triangles(14, 15, 16, 0, 11, 18, 17, 0),
  gsSP2Triangles(12, 17, 19, 0, 11, 19, 18, 0),
  gsSP2Triangles(19, 17, 18, 0, 1, 0, 27, 0),
  gsSP2Triangles(24, 1, 55, 0, 4, 5, 0, 0),
  gsSP2Triangles(6, 33, 26, 0, 1, 24, 30, 0),
  gsSP2Triangles(16, 15, 5, 0, 27, 0, 5, 0),
  gsSP2Triangles(3, 2, 7, 0, 14, 46, 33, 0),
  gsSP2Triangles(47, 13, 7, 0, 39, 9, 10, 0),
  gsSP2Triangles(36, 10, 8, 0, 51, 15, 13, 0),
  gsSP2Triangles(16, 50, 46, 0, 6, 7, 13, 0),
  gsSP2Triangles(4, 30, 50, 0, 31, 5, 15, 0),
  gsSP2Triangles(14, 13, 15, 0, 11, 17, 2, 0),
  gsSP2Triangles(12, 2, 17, 0, 11, 12, 19, 0),
  gsSP2Triangles(34, 25, 43, 0, 36, 26, 37, 0),
  gsSP2Triangles(28, 21, 20, 0, 21, 55, 27, 0),
  gsSP2Triangles(32, 23, 26, 0, 21, 28, 30, 0),
  gsSP2Triangles(49, 28, 29, 0, 27, 31, 29, 0),
  gsSP2Triangles(23, 32, 35, 0, 45, 32, 33, 0),
  gsSP2Triangles(47, 34, 35, 0, 26, 23, 37, 0),
  gsSP2Triangles(37, 23, 38, 0, 22, 38, 23, 0),
  gsSP2Triangles(38, 42, 39, 0, 39, 40, 41, 0),
  gsSP2Triangles(38, 41, 37, 0, 36, 41, 40, 0),
  gsSP2Triangles(22, 42, 38, 0, 42, 25, 39, 0),
  gsSP2Triangles(34, 43, 35, 0, 43, 25, 42, 0),
  gsSP2Triangles(22, 35, 43, 0, 51, 47, 44, 0),
  gsSP2Triangles(49, 45, 46, 0, 32, 45, 44, 0),
  gsSP2Triangles(28, 49, 50, 0, 31, 51, 48, 0),
  gsSP2Triangles(45, 49, 48, 0, 42, 52, 53, 0),
  gsSP2Triangles(43, 54, 52, 0, 42, 53, 54, 0),
  gsSP2Triangles(54, 53, 52, 0, 21, 27, 20, 0),
  gsSP2Triangles(24, 55, 21, 0, 28, 20, 29, 0),
  gsSP2Triangles(32, 26, 33, 0, 21, 30, 24, 0),
  gsSP2Triangles(49, 29, 48, 0, 27, 29, 20, 0),
  gsSP2Triangles(23, 35, 22, 0, 45, 33, 46, 0),
  gsSP2Triangles(47, 35, 44, 0, 39, 41, 38, 0),
  gsSP2Triangles(36, 37, 41, 0, 51, 44, 48, 0),
  gsSP2Triangles(49, 46, 50, 0, 32, 44, 35, 0),
  gsSP2Triangles(28, 50, 30, 0, 31, 48, 29, 0),
  gsSP2Triangles(45, 48, 44, 0, 42, 22, 52, 0),
  gsSP2Triangles(43, 52, 22, 0, 42, 54, 43, 0),
  gsSPPopMatrix(G_MTX_MODELVIEW),
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

  guScale(&kaiju1Scale, 0.01f, 0.01f, 0.01f);

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
  hitboxes[0].displayCommands = kaiju1_commands;
  hitboxes[0].check = sdSphere;
  hitboxes[0].numberOfChildren = 0;
  for (i = 0; i < MAX_CHILDREN_PER_HITBOX; i++) {
    hitboxes[0].children[i] = NULL;
  }
  hitboxes[0].parent = NULL;

  // The legs
  hitboxes[2].alive = 1;
  hitboxes[2].destroyable = 0;
  hitboxes[2].isTransformDirty = 1;
  hitboxes[2].position = (vec3){ 0.5f, 0.5f, -1.9f };
  hitboxes[2].rotation = (vec3){ 7.f, 0.f, 0.f };
  hitboxes[2].scale = (vec3){ 0.125f, 0.125f, 2.5f };
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
  hitboxes[3].position = (vec3){ -0.5f, 0.5f, -1.9f };
  hitboxes[3].rotation = (vec3){ 7.f, 0.f, 0.f };
  hitboxes[3].scale = (vec3){ 0.125f, 0.125f, 2.5f };
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
  hitboxes[6].position = (vec3){ 0.5f, -0.5f, -1.9f };
  hitboxes[6].rotation = (vec3){ -7.f, 0.f, 0.f };
  hitboxes[6].scale = (vec3){ 0.125f, 0.125f, 2.5f };
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
  hitboxes[7].position = (vec3){ -0.5f, -0.5f, -1.9f };
  hitboxes[7].rotation = (vec3){ -7.f, 0.f, 0.f };
  hitboxes[7].scale = (vec3){ 0.125f, 0.125f, 2.5f };
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

  // body
  {
    hitboxes[0].position.x = catmullRom(tVal, walkPoints[LOOPED_WALK_POINT(pointIndex - 1)].x, walkPoints[LOOPED_WALK_POINT(pointIndex)].x, walkPoints[LOOPED_WALK_POINT(pointIndex + 1)].x, walkPoints[LOOPED_WALK_POINT(pointIndex + 2)].x);
    hitboxes[0].position.y = catmullRom(tVal, walkPoints[LOOPED_WALK_POINT(pointIndex - 1)].y, walkPoints[LOOPED_WALK_POINT(pointIndex)].y, walkPoints[LOOPED_WALK_POINT(pointIndex + 1)].y, walkPoints[LOOPED_WALK_POINT(pointIndex + 2)].y);
    hitboxes[0].position.z = getHeight(hitboxes[0].position.x, hitboxes[0].position.y) + 25.f;
    
    hitboxes[0].rotation.x = RAD_TO_DEG * sinf(kaijuTotalTime * 0.5f) * 0.147272f;
  
    oldPosAngle = nu_atan2(oldPos.y - hitboxes[0].position.y, oldPos.x - hitboxes[0].position.x);
    hitboxes[0].rotation.z = RAD_TO_DEG * (oldPosAngle + (M_PI * 0.5f));
  
    hitboxes[0].isTransformDirty = 1;
  }

  // legs
  {
    hitboxes[2].position.z = -1.9f + (sinf(kaijuTotalTime * 1.4f + M_PI)) * 0.615243f;
    hitboxes[2].rotation.x = 3.f + (sinf(kaijuTotalTime * 1.4f + M_PI) * 3.f);
    hitboxes[2].isTransformDirty = 1;
    hitboxes[3].position.z = -1.9f + (sinf(kaijuTotalTime * 1.4f + M_PI)) * 0.615243f;
    hitboxes[3].rotation.x = 3.f + (sinf(kaijuTotalTime * 1.4f + M_PI) * 3.f);
    hitboxes[3].isTransformDirty = 1;
    hitboxes[6].position.z = -1.9f + (sinf(kaijuTotalTime * 1.4f)) * 0.615243f;
    hitboxes[6].rotation.x = 3.f + (sinf(kaijuTotalTime * 1.4f) * 3.f);
    hitboxes[6].isTransformDirty = 1;
    hitboxes[7].position.z = -1.9f + (sinf(kaijuTotalTime * 1.4f)) * 0.615243f;
    hitboxes[7].rotation.x = 3.f + (sinf(kaijuTotalTime * 1.4f) * 3.f);
    hitboxes[7].isTransformDirty = 1;
  }
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
