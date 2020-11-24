
#include "kaiju1.h"

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

float kaijuTime;
float kaijuTotalTime;

void initKaiju0() {
  int i;

  kaijuTime = 0.f;
  kaijuTotalTime = 0.f;

	 for (i = 0; i < NUMBER_OF_KAIJU_HITBOXES; i++) {
    hitboxes[0].alive = 0;
  }

  // Create a "body piece"
  hitboxes[0].alive = 1;
  hitboxes[0].destroyable = 1;
  hitboxes[0].isTransformDirty = 1;
  hitboxes[0].position = (vec3){ 64.5f ,64.5f , 10.5f };
  hitboxes[0].rotation = (vec3){ 0.f, 0.f, 0.f };
  hitboxes[0].scale = (vec3){ 1.f, 1.f, 1.f  };
  guMtxIdentF(hitboxes[0].computedTransform.data);
  hitboxes[0].displayCommands = red_octahedron_commands;
  hitboxes[0].check = sdOctahedron;
  hitboxes[0].numberOfChildren = 0;
  for (i = 0; i < MAX_CHILDREN_PER_HITBOX; i++) {
    hitboxes[0].children[i] = NULL;
  }
  hitboxes[0].parent = NULL;

}

void updateKaiju0(float deltaSeconds) {
  kaijuTime += deltaSeconds;
  kaijuTotalTime += deltaSeconds;
}

void renderKaiju0(DisplayData* dynamicp) {
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
