#include <assert.h>
#include <nusys.h>
#include "main.h"

#include "map.h"
#include "graphic.h"
#include "gamemath.h"
#include "terraintex.h"

#define CAMERA_FOV 60
#define CAMERA_MOVE_SPEED 10.f
#define CAMERA_TURN_SPEED_X 2.373f
#define CAMERA_TURN_SPEED_Y 2.373f

#define FOCUS_HIGH_DETAIL_CUTOFF 36.f
#define FOCUS_HIGH_DETAIL_CUTOFF_SQ (FOCUS_HIGH_DETAIL_CUTOFF * FOCUS_HIGH_DETAIL_CUTOFF)

#define HIGH_DETAIL_CUTOFF 20.f
#define HIGH_DETAIL_CUTOFF_SQ (HIGH_DETAIL_CUTOFF * HIGH_DETAIL_CUTOFF)

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

static vec3 cameraPos;
static vec3 cameraTarget;
static vec3 cameraRotation;

void initStage00(void) {
  int i;
  
  cameraPos = (vec3){0, 0, 20};
  cameraTarget = (vec3){10.f, 10.f, 0.f};
  cameraRotation = (vec3){0.f, 0.f, 0.f};

  // Fill the map with sinewave-based data
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

  guPerspective(&dynamicp->projection, &perspNorm, CAMERA_FOV, (float)SCREEN_WD/(float)SCREEN_HT, 1.0f, 1000.0f, 1.0f);
  gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->projection)), G_MTX_PROJECTION|G_MTX_LOAD|G_MTX_NOPUSH);

  guLookAt(&dynamicp->camera, cameraPos.x ,cameraPos.y, cameraPos.z, cameraTarget.x, cameraTarget.y, cameraTarget.z, 0, 0, 1);
  gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(dynamicp->camera)), G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);

  gDPPipeSync(glistp++);
  gDPSetScissor(glistp++, G_SC_NON_INTERLACE, 22, 24, SCREEN_WD - 22, SCREEN_HT - 64);
  gDPSetCycleType(glistp++, G_CYC_1CYCLE);
  gDPSetRenderMode(glistp++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
  gSPClearGeometryMode(glistp++, 0xFFFFFFFF);
  gSPSetGeometryMode(glistp++, G_SHADE| G_SHADING_SMOOTH);

  guScale(&dynamicp->mapScale, 0.01f, 0.01f, 0.01f);
  gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(dynamicp->mapScale)), G_MTX_MODELVIEW | G_MTX_PUSH);
  gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(mapSetionsPreable));

  for (i = 0; i < SECTIONS_PER_MAP; i++) {
    float distanceToSectionSq;
    const vec3 centroidDirection = { sections[i].centroid.x - cameraPos.x, sections[i].centroid.y - cameraPos.y, sections[i].centroid.z - cameraPos.z };
    const vec3 cameraDirection = { cameraTarget.x - cameraPos.x, cameraTarget.y - cameraPos.y, cameraTarget.z - cameraPos.z };
    float dotProductFromCamera = dotProduct(&cameraDirection, &centroidDirection);
    if (dotProductFromCamera < 0.4f) {
      continue;
    }

    distanceToSectionSq = distanceSq(&(sections[i].centroid), &cameraPos);

    if ((distanceToSectionSq < HIGH_DETAIL_CUTOFF_SQ) || ((distanceToSectionSq < FOCUS_HIGH_DETAIL_CUTOFF_SQ) && (dotProductFromCamera > 0.8375f))) {
      gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(sections[i].commands));
    } else {
      gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(lowDetailSections[i].commands));
    }
  }

  gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(mapSectionsPostamble));
  gSPPopMatrix(glistp++, G_MTX_MODELVIEW);

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
      /* Change character representation positions */
      nuDebConTextPos(0,12,23);
      sprintf(conbuf,"test");
      nuDebConCPuts(0, conbuf);
    }
  else
    {
      nuDebConTextPos(0,4,4);
      nuDebConCPuts(0, "Connect controller #1, kid!");
    }
    
  /* Display characters on the frame buffer */
  nuDebConDisp(NU_SC_SWAPBUFFER);

  /* Switch display list buffers */
  gfx_gtask_no ^= 1;
}

void updateGame00(void)
{  
  static float vel = 1.0;

  /* Data reading of controller 1 */
  nuContDataGetEx(contdata,0);

}
