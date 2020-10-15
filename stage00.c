#include <assert.h>
#include <nusys.h>
#include "main.h"

#include "map.h"
#include "graphic.h"
#include "gamemath.h"
#include "terraintex.h"
#include "hitboxes.h"

#define SCISSOR_HIGH 64
#define SCISSOR_LOW 24
#define SCISSOR_SIDES 22

#define CAMERA_FOV 60
#define CAMERA_MOVE_SPEED 10.f
#define CAMERA_TURN_SPEED_X 2.373f
#define CAMERA_TURN_SPEED_Y 2.373f
#define CAMERA_DISTANCE 3.f
#define CAMERA_LIFT 1.f

#define FAR_PLANE_DETAIL_CUTOFF 80.f
#define FAR_PLANE_DETAIL_CUTOFF_SQ (FAR_PLANE_DETAIL_CUTOFF * FAR_PLANE_DETAIL_CUTOFF)

#define FOCUS_HIGH_DETAIL_CUTOFF 29.f
#define FOCUS_HIGH_DETAIL_CUTOFF_SQ (FOCUS_HIGH_DETAIL_CUTOFF * FOCUS_HIGH_DETAIL_CUTOFF)

#define HIGH_DETAIL_CUTOFF 20.f
#define HIGH_DETAIL_CUTOFF_SQ (HIGH_DETAIL_CUTOFF * HIGH_DETAIL_CUTOFF)

#define PLAYER_MOVE_SPEED 10.f
#define GRAVITY -3.2f
#define JUMP_VELOCITY 3.f
#define STEP_HEIGHT_CUTOFF 0.15f

static Vtx player_geo[] = {
  {  1,  1,  2, 0, 0, 0, 0xff, 0, 0xff, 0xff },
  { -1,  1,  2, 0, 0, 0, 0, 0, 0xff, 0xff },
  { -1, -1,  2, 0, 0, 0, 0, 0, 0xff, 0xff },
  {  1, -1,  2, 0, 0, 0, 0, 0, 0xff, 0xff },
  {  1,  1,  0, 0, 0, 0, 0, 0, 0x33, 0xff },
  { -1,  1,  0, 0, 0, 0, 0, 0, 0x33, 0xff },
  { -1, -1,  0, 0, 0, 0, 0, 0, 0xf3, 0xff },
  {  1, -1,  0, 0, 0, 0, 0, 0, 0x33, 0xff },
};

static Vtx red_octahedron_geo[] = {
  {  1,  0,  0, 0, 0, 0, 0xaa, 0, 0x00, 0xff },
  { -1,  0,  0, 0, 0, 0, 0xaa, 0, 0x00, 0xff },

  {  0,  1,  0, 0, 0, 0, 0xaa, 0, 0x00, 0xff },
  {  0, -1,  0, 0, 0, 0, 0xaa, 0, 0x00, 0xff },

  {  0,  0,  1, 0, 0, 0, 0xff, 0, 0x00, 0xff },
  {  0,  0, -1, 0, 0, 0, 0x55, 0, 0x00, 0xff },
};

static Gfx red_octahedron_commands[] = {
  gsSPVertex(&red_octahedron_geo, 6, 0),
  gsSP2Triangles(0, 2, 4, 0, 0, 5, 2, 0),
  gsSP2Triangles(0, 4, 3, 0, 0, 3, 5, 0),
  gsSP2Triangles(1, 3, 4, 0, 1, 5, 3, 0),
  gsSP2Triangles(1, 4, 2, 0, 1, 2, 5, 0),
  gsSPEndDisplayList()
};

static Gfx player_commands[] = {
  gsSPVertex(&player_geo, 8, 0),
  gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
  gsSP2Triangles(3, 2, 6, 0, 3, 6, 7, 0),
  gsSP2Triangles(1, 0, 4, 0, 1, 4, 5, 0),
  gsSPEndDisplayList()
};

static Gfx mapSetionsPreable[] = {
  gsSPTexture(0x8000, 0x8000, 0, 0, G_ON),
  gsDPPipeSync(),
  gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
  gsDPSetTextureFilter(G_TF_POINT),
  gsDPLoadTextureBlock(tex_terrain_bin, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
  gsSPEndDisplayList(),
};

static Gfx mapSectionsPostamble[] = {
  gsSPTexture(0x8000, 0x8000, 0, 0, G_OFF),
  gsSPEndDisplayList(),
};

static Vtx hud_geo[] = {
  // Gradient background left
  {  SCISSOR_SIDES, SCREEN_HT - 1,  5, 0, 8 << 6, 0 << 6, 0x00, 0, 0x33, 0xff },
  {   0, SCREEN_HT - 1,  5, 0, 0 << 6, 0 << 6, 0x00, 0, 0x33, 0xff },
  {   0,             0,  5, 0, 0 << 6, 8 << 6, 0x2D, 0, 0x33, 0xff },
  {  SCISSOR_SIDES,             0,  5, 0, 8 << 6, 8 << 6, 0x2D, 0, 0x33, 0xff },

  // Gradient background right
  {  SCREEN_WD, SCREEN_HT - 1,  5, 0, 8 << 6, 0 << 6, 0x00, 0, 0x33, 0xff },
  {   SCREEN_WD - SCISSOR_SIDES - 1, SCREEN_HT - 1,  5, 0, 0 << 6, 0 << 6, 0x00, 0, 0x33, 0xff },
  {   SCREEN_WD - SCISSOR_SIDES - 1,             0,  5, 0, 0 << 6, 8 << 6, 0x2D, 0, 0x33, 0xff },
  {  SCREEN_WD,             0,  5, 0, 8 << 6, 8 << 6, 0x2D, 0, 0x33, 0xff },

  // Gradient background top
  {  SCREEN_WD - SCISSOR_SIDES - 1,               SCREEN_HT - 1,  5, 0, 8 << 6, 0 << 6, 0x00, 0, 0x33, 0xff },
  {                  SCISSOR_SIDES,               SCREEN_HT - 1,  5, 0, 0 << 6, 0 << 6, 0x00, 0, 0x33, 0xff },
  {                  SCISSOR_SIDES, SCREEN_HT - SCISSOR_LOW,  5, 0, 0 << 6, 8 << 6,     0x00, 0, 0x33, 0xff },
  {  SCREEN_WD - SCISSOR_SIDES - 1, SCREEN_HT - SCISSOR_LOW,  5, 0, 8 << 6, 8 << 6,     0x00, 0, 0x33, 0xff },

  // Gradient background bottom
  {  SCREEN_WD - SCISSOR_SIDES - 1,               SCISSOR_HIGH,  5, 0, 8 << 6, 0 << 6, 0x24, 0, 0x33, 0xff },
  {                  SCISSOR_SIDES,               SCISSOR_HIGH,  5, 0, 0 << 6, 0 << 6, 0x24, 0, 0x33, 0xff },
  {                  SCISSOR_SIDES,                          0,  5, 0, 0 << 6, 8 << 6, 0x2D, 0, 0x33, 0xff },
  {  SCREEN_WD - SCISSOR_SIDES - 1,                          0,  5, 0, 8 << 6, 8 << 6, 0x2D, 0, 0x33, 0xff },
};

static Gfx hud_dl[] = {
  gsSPVertex(&hud_geo, 16, 0),
  gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
  gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
  gsSP2Triangles(8, 9, 10, 0, 8, 10, 11, 0),
  gsSP2Triangles(12, 13, 14, 0, 12, 14, 15, 0),
  gsSPEndDisplayList()
};

static vec3 cameraPos;
static vec3 cameraTarget;
static vec3 cameraRotation;

static vec3 playerPos;
static vec3 playerVelocity;
static int playerIsOnTheGround;

static vec3 cameraPos;
static vec3 cameraTarget;
static vec3 cameraRotation;
static const s8 cameraYInvert = 1;

static OSTime time;
static OSTime delta;

KaijuHitbox testHitbox;

void initStage00(void) {
  int i;

  cameraPos = (vec3){4, 0, 20};
  cameraTarget = (vec3){10.f, 10.f, 0.f};
  cameraRotation = (vec3){0.f, 0.f, 0.f};

  testHitbox.alive = 1;
  testHitbox.position = (vec3){ 8.f, 2.f, 8.f };
  testHitbox.rotation = (vec3){ 45.f, 0.f, 0.f };
  testHitbox.scale = (vec3){ 2.f, 0.4f, 2.f };
  guMtxIdentF(testHitbox.computedTransform.data);
  testHitbox.displayCommands = red_octahedron_commands;

  // Fill the map with sinewave-based data
  // TODO: make this load from a file
  for (i = 0; i < MAP_LENGTH; i++) {
    int x = (i % MAP_WIDTH);
    int y = (i / MAP_WIDTH);
    float funcVal = MAX(0.f, sinf((i % MAP_WIDTH) * 0.1f) - cosf((i / MAP_WIDTH) * 0.1f) + (cosf((i / MAP_WIDTH * 10)) * 0.01f));

    GroundMapping[i] = (u8)((funcVal * 0.5f + 0.5f) * 170);
    if (funcVal > 0.25f) {
      GroundTexIndex[i] = 5;
    } else {
      GroundTexIndex[i] = RAND(4);
    }
  }

  GroundMapping[(MAP_WIDTH * 2) + 2] = 200;
  GroundMapping[(MAP_WIDTH * 3) + 2] = 200;
  GroundMapping[(MAP_WIDTH * 4) + 2] = 200;
  GroundMapping[(MAP_WIDTH * 5) + 2] = 200;
  GroundMapping[(MAP_WIDTH * 6) + 2] = 200;
  GroundMapping[(MAP_WIDTH * 2) + 3] = 200;
  GroundMapping[(MAP_WIDTH * 3) + 3] = 200;
  GroundMapping[(MAP_WIDTH * 5) + 3] = 200;
  GroundMapping[(MAP_WIDTH * 6) + 3] = 200;
  GroundMapping[(MAP_WIDTH * 4) + 3] = 200;
  GroundMapping[(MAP_WIDTH * 2) + 4] = 200;
  GroundMapping[(MAP_WIDTH * 3) + 4] = 200;
  GroundMapping[(MAP_WIDTH * 4) + 4] = 255;
  GroundMapping[(MAP_WIDTH * 5) + 4] = 200;
  GroundMapping[(MAP_WIDTH * 6) + 4] = 200;
  GroundMapping[(MAP_WIDTH * 2) + 5] = 200;
  GroundMapping[(MAP_WIDTH * 3) + 5] = 200;
  GroundMapping[(MAP_WIDTH * 4) + 5] = 200;
  GroundMapping[(MAP_WIDTH * 5) + 5] = 200;
  GroundMapping[(MAP_WIDTH * 6) + 5] = 200;
  GroundMapping[(MAP_WIDTH * 2) + 6] = 200;
  GroundMapping[(MAP_WIDTH * 3) + 6] = 200;
  GroundMapping[(MAP_WIDTH * 4) + 6] = 200;
  GroundMapping[(MAP_WIDTH * 5) + 6] = 200;
  GroundMapping[(MAP_WIDTH * 6) + 6] = 200;

  generateSectionDLs();
}

void makeDL00(void) {
  DisplayData* dynamicp;
  char conbuf[20]; 
  u16 perspNorm;
  int i;

  /* Specify the display list buffer */
  dynamicp = &gfx_dynamic[gfx_gtask_no];
  glistp = &gfx_glist[gfx_gtask_no][0];

  /* Initialize RCP */
  gfxRCPInit();

  /* Clear the frame and Z-buffer */
  gfxClearCfb();

  // Initial setup
  gDPPipeSync(glistp++);
  gDPSetScissor(glistp++, G_SC_NON_INTERLACE, SCISSOR_SIDES, SCISSOR_LOW, SCREEN_WD - SCISSOR_SIDES, SCREEN_HT - SCISSOR_HIGH);
  gDPSetCycleType(glistp++, G_CYC_1CYCLE);
  gDPSetRenderMode(glistp++, G_RM_ZB_OPA_SURF, G_RM_ZB_OPA_SURF2);
  gSPClearGeometryMode(glistp++, 0xFFFFFFFF);
  gSPSetGeometryMode(glistp++, G_SHADE | G_SHADING_SMOOTH | G_CULL_BACK | G_ZBUFFER);

  // The camera
  {
    guPerspective(&dynamicp->projection, &perspNorm, CAMERA_FOV, (float)SCREEN_WD/(float)SCREEN_HT, 1.0f, 1000.0f, 1.0f);
    gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->projection)), G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    guLookAt(&dynamicp->camera, cameraPos.x ,cameraPos.y, cameraPos.z, cameraTarget.x, cameraTarget.y, cameraTarget.z, 0, 0, 1);
    gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(dynamicp->camera)), G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
  
  }

  // The player
  {
    guTranslate(&(dynamicp->playerTranslation), playerPos.x, playerPos.y, playerPos.z);
    gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(dynamicp->playerTranslation)), G_MTX_MODELVIEW | G_MTX_PUSH);

    guRotate(&(dynamicp->playerRotation), (cameraRotation.z + M_PI * 0.5f) * RAD_TO_DEG, 0.f, 0.f, 1.f);
    gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(dynamicp->playerRotation)), G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);

    guScale(&(dynamicp->playerScale), 0.25f, 0.25f, 0.25f);
    gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(dynamicp->playerScale)), G_MTX_MODELVIEW | G_MTX_NOPUSH);
    gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(player_commands));
    gSPPopMatrix(glistp++, G_MTX_MODELVIEW);
  }

  // The kaiju
  {
    guMtxF2L(testHitbox.computedTransform.data, &(dynamicp->octaTransform));
    gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(dynamicp->octaTransform)), G_MTX_MODELVIEW | G_MTX_PUSH);

    gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(red_octahedron_commands));

    gSPPopMatrix(glistp++, G_MTX_MODELVIEW);
  }

  // The map
  {
    guScale(&dynamicp->mapScale, 0.01f, 0.01f, 0.01f);
    gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(dynamicp->mapScale)), G_MTX_MODELVIEW | G_MTX_PUSH);
    gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(mapSetionsPreable));

    for (i = 0; i < SECTIONS_PER_MAP; i++) {
      float distanceToSectionSq = distanceSq(&(sections[i].centroid), &cameraPos);
      const vec3 centroidDirection = { sections[i].centroid.x - cameraPos.x, sections[i].centroid.y - cameraPos.y, sections[i].centroid.z - cameraPos.z };
      const vec3 cameraDirection = { cameraTarget.x - cameraPos.x, cameraTarget.y - cameraPos.y, cameraTarget.z - cameraPos.z };
      float dotProductFromCamera = dotProduct(&cameraDirection, &centroidDirection);
      if (dotProductFromCamera < 0.4f) {
        continue;
      }

      if (distanceToSectionSq > FAR_PLANE_DETAIL_CUTOFF_SQ) {
        continue;
      }

      if ((distanceToSectionSq < HIGH_DETAIL_CUTOFF_SQ) || ((distanceToSectionSq < FOCUS_HIGH_DETAIL_CUTOFF_SQ) && (dotProductFromCamera > 0.8975f))) {
        gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(sections[i].commands));
      } else {
        gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(lowDetailSections[i].commands));
      }
    }

    gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(mapSectionsPostamble));
    gSPPopMatrix(glistp++, G_MTX_MODELVIEW);
  }

  // HUD
  {
    gDPPipeSync(glistp++);
    gDPSetScissor(glistp++, G_SC_NON_INTERLACE, 0, 0, SCREEN_WD - 1, SCREEN_HT - 1);
    gDPSetRenderMode(glistp++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetCombineMode(glistp++, G_CC_SHADE, G_CC_SHADE);
    gSPClearGeometryMode(glistp++,0xFFFFFFFF);
    gSPSetGeometryMode(glistp++, G_SHADE | G_SHADING_SMOOTH | G_CULL_BACK);

    guOrtho(&(dynamicp->orthoHudProjection), 0, SCREEN_WD - 1, 0, SCREEN_HT - 1, 0.f, 10.f, 1.f);
    gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->orthoHudProjection)), G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    guMtxIdent(&(dynamicp->orthoHudModelling));
    gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->orthoHudModelling)), G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPDisplayList(glistp++, hud_dl);
  }

  gDPFullSync(glistp++);
  gSPEndDisplayList(glistp++);

  assert((glistp - gfx_glist[gfx_gtask_no]) < GFX_GLIST_LEN);

  /* Activate the task and 
     switch display buffers */
  nuGfxTaskStart(&gfx_glist[gfx_gtask_no][0],
		 (s32)(glistp - gfx_glist[gfx_gtask_no]) * sizeof (Gfx),
		 NU_GFX_UCODE_F3DLP_REJ , NU_SC_NOSWAPBUFFER);

  if(contPattern & 0x1)
    {
      
    }
  else
    {
      nuDebConTextPos(0,4,4);
      nuDebConCPuts(0, "Connect controller #1, kid!");
    }

    nuDebTaskPerfBar1(1, 200, NU_SC_NOSWAPBUFFER);
    
  /* Display characters on the frame buffer */
  nuDebConDisp(NU_SC_SWAPBUFFER);

  /* Switch display list buffers */
  gfx_gtask_no ^= 1;
}

float getHeight(float x, float y) {
  const float fractionX = x - ((int)x);
  const float fractionY = y - ((int)y);

  const int mid = sampleHeight(x, y);

  const int n = sampleHeight(x, y-1);
  const int s = sampleHeight(x, y+1);
  const int e = sampleHeight(x+1, y);
  const int w = sampleHeight(x-1, y);

  const int ne = sampleHeight(x+1, y-1);
  const int se = sampleHeight(x+1, y+1);
  const int nw = sampleHeight(x-1, y-1);
  const int sw = sampleHeight(x-1, y+1);

  const float heightA = (((nw + n + mid + w) * 0.25f) / 256.f) * 20.f;
  const float heightB = (((n + ne + e + mid) * 0.25f) / 256.f) * 20.f;
  const float heightC = (((mid + e + se + s) * 0.25f) / 256.f) * 20.f;
  const float heightD = (((w + mid + s + sw) * 0.25f) / 256.f) * 20.f;

  return bilinear(heightA, heightB, heightC, heightD, fractionX,  fractionY);
}

void updatePlayer(float deltaSeconds) {
  float stepX;
  float stepY;
  float stepZ;
  float groundHeightStepX;
  float groundHeightStepY;
  float currentAltitude;
  float groundHeight;
  float inputDirectionX = contdata->stick_x / 255.f;
  float inputDirectionY = contdata->stick_y / 255.f;
  float inputDirectionXRotated;
  float inputDirectionYRotated;
  float cosCameraRot, sinCameraRot;

  if (contdata->button & L_CBUTTONS) {
    cameraRotation.z += CAMERA_TURN_SPEED_X * deltaSeconds;
  }
  else if (contdata->button & R_CBUTTONS) {
    cameraRotation.z -= CAMERA_TURN_SPEED_X * deltaSeconds;
  }

  if (contdata->button & D_CBUTTONS) {
    cameraRotation.y = MAX(-0.3f, cameraRotation.y - (CAMERA_TURN_SPEED_Y * deltaSeconds * cameraYInvert));
  }
  else if (contdata->button & U_CBUTTONS) {
    cameraRotation.y = MIN(1.3f, cameraRotation.y + (CAMERA_TURN_SPEED_Y * deltaSeconds * cameraYInvert));
  }

  cosCameraRot = cosf(cameraRotation.z + M_PI * 0.5f);
  sinCameraRot = sinf(cameraRotation.z + M_PI * 0.5f);
  inputDirectionXRotated = (cosCameraRot * inputDirectionX) - (sinCameraRot * inputDirectionY);
  inputDirectionYRotated = (sinCameraRot * inputDirectionX) + (cosCameraRot * inputDirectionY);

  playerVelocity.x = inputDirectionXRotated * PLAYER_MOVE_SPEED;
  playerVelocity.y = inputDirectionYRotated * PLAYER_MOVE_SPEED;
  playerVelocity.z += GRAVITY * deltaSeconds;

  if (playerIsOnTheGround && (contdata->button & A_BUTTON)) {
    playerIsOnTheGround = 0;
    playerVelocity.z = JUMP_VELOCITY;
  }

  stepX = playerPos.x + (playerVelocity.x * deltaSeconds);
  stepY = playerPos.y + (playerVelocity.y * deltaSeconds);
  stepZ = playerPos.z + (playerVelocity.z * deltaSeconds);

  groundHeight = MAX(stepZ, getHeight(stepX, stepY));
  currentAltitude = MAX(stepZ, getHeight(playerPos.x, playerPos.y));

  if ((groundHeight - currentAltitude) > STEP_HEIGHT_CUTOFF) {
    stepX = playerPos.x;
    stepY = playerPos.y;
  }

  playerPos.x = stepX;
  playerPos.y = stepY;
  playerPos.z = stepZ;

  if (playerPos.z < currentAltitude) {
    playerPos.z = currentAltitude;
    playerVelocity.z = 0.f;

    if ((groundHeight - currentAltitude) < STEP_HEIGHT_CUTOFF) {
      playerIsOnTheGround = 1;
    }
  } else if (playerPos.z > currentAltitude + 0.1f) {
    playerIsOnTheGround = 0;
  }

  cameraTarget.x = lerp( cameraTarget.x, playerPos.x, 0.12f);
  cameraTarget.y = lerp( cameraTarget.y, playerPos.y, 0.12f);
  cameraTarget.z = lerp( cameraTarget.z, playerPos.z + (CAMERA_LIFT * 0.5f) + cameraRotation.y, 0.12f);
  cameraPos.x = cameraTarget.x + (cosf(cameraRotation.z) * CAMERA_DISTANCE);
  cameraPos.y = cameraTarget.y + (sinf(cameraRotation.z) * CAMERA_DISTANCE);
  cameraPos.z = cameraTarget.z + CAMERA_LIFT - cameraRotation.y;
}

// TODO: make this an array of boxes
void updateKaijuHitboxes(float delta) {
  mat4 positionMatrix;
  mat4 rotationMatrix;
  mat4 scaleMatrix;

  guTranslateF(positionMatrix.data, testHitbox.position.x, testHitbox.position.y, testHitbox.position.z);
  guRotateRPYF(rotationMatrix.data, testHitbox.rotation.x, testHitbox.rotation.y, testHitbox.rotation.z);
  guScaleF(scaleMatrix.data, testHitbox.scale.x, testHitbox.scale.y, testHitbox.scale.z);
  
  guMtxCatF(scaleMatrix.data, rotationMatrix.data, testHitbox.computedTransform.data);
  guMtxCatF(testHitbox.computedTransform.data, positionMatrix.data, testHitbox.computedTransform.data);
}

void updateGame00(void)
{ 
  float deltaInSeconds = 0.f;
  OSTime newTime = OS_CYCLES_TO_USEC(osGetTime());

  delta = (newTime - time);
  time = newTime;
  deltaInSeconds = delta * 0.000001f;

  nuDebPerfMarkSet(0);

  /* Data reading of controller 1 */
  nuContDataGetEx(contdata,0);

  testHitbox.rotation.x += 0.1f;

  updatePlayer(deltaInSeconds);
  nuDebPerfMarkSet(1);
  updateKaijuHitboxes(deltaInSeconds);
  nuDebPerfMarkSet(2);
}
