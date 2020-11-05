#include <assert.h>
#include <nusys.h>
#include "stage00.h"
#include "main.h"
#include "segmentinfo.h"

#include "map.h"
#include "graphic.h"
#include "gamemath.h"
#include "terraintex.h"
#include "hitboxes.h"
#include "portraittex.h"
#include "protaggeo.h"
#include "kaiju1.h"

#define SCISSOR_HIGH 64
#define SCISSOR_LOW 24
#define SCISSOR_SIDES 22

#define CAMERA_BASE_FOV 60
#define CAMERA_ZOOM_FOV_CHANGE -35
#define CAMERA_MOVE_SPEED 10.f
#define CAMERA_TURN_SPEED_X 2.373f
#define CAMERA_TURN_SPEED_Y 2.373f
#define CAMERA_TURN_SPEED_ADJUSTMENT_WHILE_ZOOMED -2.2f
#define CAMERA_DISTANCE 3.f
#define CAMERA_LIFT_FRONT 0.7f
#define CAMERA_LIFT_BACK 1.2f

#define FAR_PLANE_DETAIL_CUTOFF 128.f
#define FAR_PLANE_DETAIL_CUTOFF_SQ (FAR_PLANE_DETAIL_CUTOFF * FAR_PLANE_DETAIL_CUTOFF)

#define FOCUS_HIGH_DETAIL_CUTOFF 34.f
#define FOCUS_HIGH_DETAIL_CUTOFF_SQ (FOCUS_HIGH_DETAIL_CUTOFF * FOCUS_HIGH_DETAIL_CUTOFF)

#define HIGH_DETAIL_CUTOFF 45.f
#define HIGH_DETAIL_CUTOFF_SQ (HIGH_DETAIL_CUTOFF * HIGH_DETAIL_CUTOFF)

#define PLAYER_MOVE_SPEED 10.f
#define GRAVITY -3.2f
#define JUMP_VELOCITY 3.f
#define STEP_HEIGHT_CUTOFF 0.15f

#define NOT_ZOOMED_IN -1
#define ZOOMED_IN 1
#define ZOOM_IN_OUT_SPEED 4.f

#define LASER_CHARGE_SPEED 0.49151f

#define MAX_STEP_COUNT 64
#define INITIAL_STEP_DISTANCE 4.0f

#define DEFAULT_NOISE_LEVEL 0.035f

#define DIVINE_LINE_OFF 0
#define DIVINE_LINE_ON 1
#define DIVINE_LINE_DURATION 100.4f
#define INV_DIVINE_LINE_DURATION (1.f / DIVINE_LINE_DURATION)
#define DIVINE_LINE_START_HEIGHT 70

static Vtx divine_line_geo[] = {
  {  0,   2,     0, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  1,  -1,     0, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  { -1,  -1,     0, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  0,   2,    -4, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  1,  -1,    -4, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  { -1,  -1,    -4, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  0,   2,    -8, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  1,  -1,    -8, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  { -1,  -1,    -8, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  0,   2,   -16, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  1,  -1,   -16, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  { -1,  -1,   -16, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  0,   2,   -20, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  1,  -1,   -20, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  { -1,  -1,   -20, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  0,   2,   -24, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  1,  -1,   -24, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  { -1,  -1,   -24, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  0,   2,   -28, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  1,  -1,   -28, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  { -1,  -1,   -28, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  0,   2,   -32, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  1,  -1,   -32, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  { -1,  -1,   -32, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  0,   2,   -36, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  1,  -1,   -36, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  { -1,  -1,   -36, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  0,   2,   -40, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  1,  -1,   -40, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  { -1,  -1,   -40, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  0,   2,   -44, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  1,  -1,   -44, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  { -1,  -1,   -44, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  0,   2,   -48, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  1,  -1,   -48, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  { -1,  -1,   -48, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  0,   2,   -52, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  1,  -1,   -52, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  { -1,  -1,   -52, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  0,   2,   -56, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  1,  -1,   -56, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  { -1,  -1,   -56, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  0,   2,   -60, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  1,  -1,   -60, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  { -1,  -1,   -60, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  0,   2,   -64, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  1,  -1,   -64, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  { -1,  -1,   -64, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  0,   2,   -68, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  1,  -1,   -68, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  { -1,  -1,   -68, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  0,   2,   -72, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  1,  -1,   -72, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  { -1,  -1,   -72, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  0,   2,   -74, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  1,  -1,   -74, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  { -1,  -1,   -74, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  0,   2,   -78, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  1,  -1,   -78, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  { -1,  -1,   -78, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  0,   2,   -82, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  {  1,  -1,   -82, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
  { -1,  -1,   -82, 0, 0, 0, 0xDD, 0xDD, 0x00, 0xff },
};

static Gfx divine_line_commands[] = {
  gsSPVertex(&divine_line_geo, 63, 0),
  gsSPClipRatio(FRUSTRATIO_6),
  gsSP2Triangles(0, 1, 4, 0, 0, 4, 3, 0),
  gsSP2Triangles(1, 2, 4, 0, 2, 5, 4, 0),
  gsSP2Triangles(2, 0, 5, 0, 5, 0, 3, 0),

  gsSP2Triangles(0 + 3, 1 + 3, 4 + 3, 0, 0 + 3, 4 + 3, 3 + 3, 0),
  gsSP2Triangles(1 + 3, 2 + 3, 4 + 3, 0, 2 + 3, 5 + 3, 4 + 3, 0),
  gsSP2Triangles(2 + 3, 0 + 3, 5 + 3, 0, 5 + 3, 0 + 3, 3 + 3, 0),

  gsSP2Triangles(0 + 6, 1 + 6, 4 + 6, 0, 0 + 6, 4 + 6, 3 + 6, 0),
  gsSP2Triangles(1 + 6, 2 + 6, 4 + 6, 0, 2 + 6, 5 + 6, 4 + 6, 0),
  gsSP2Triangles(2 + 6, 0 + 3, 5 + 6, 0, 5 + 6, 0 + 6, 3 + 6, 0),

  gsSP2Triangles(0 + 9, 1 + 9, 4 + 9, 0, 0 + 9, 4 + 9, 3 + 9, 0),
  gsSP2Triangles(1 + 9, 2 + 9, 4 + 9, 0, 2 + 9, 5 + 9, 4 + 9, 0),
  gsSP2Triangles(2 + 9, 0 + 9, 5 + 9, 0, 5 + 9, 0 + 9, 3 + 9, 0),

  gsSP2Triangles(0 + 12, 1 + 12, 4 + 12, 0, 0 + 12, 4 + 12, 3 + 12, 0),
  gsSP2Triangles(1 + 12, 2 + 12, 4 + 12, 0, 2 + 12, 5 + 12, 4 + 12, 0),
  gsSP2Triangles(2 + 12, 0 + 12, 5 + 12, 0, 5 + 12, 0 + 12, 3 + 12, 0),
  gsSP2Triangles(0 + 15, 1 + 15, 4 + 15, 0, 0 + 15, 4 + 15, 3 + 15, 0),
  gsSP2Triangles(1 + 15, 2 + 15, 4 + 15, 0, 2 + 15, 5 + 15, 4 + 15, 0),
  gsSP2Triangles(2 + 15, 0 + 15, 5 + 15, 0, 5 + 15, 0 + 15, 3 + 15, 0),
  gsSP2Triangles(0 + 18, 1 + 18, 4 + 18, 0, 0 + 18, 4 + 18, 3 + 18, 0),
  gsSP2Triangles(1 + 18, 2 + 18, 4 + 18, 0, 2 + 18, 5 + 18, 4 + 18, 0),
  gsSP2Triangles(2 + 18, 0 + 18, 5 + 18, 0, 5 + 18, 0 + 18, 3 + 18, 0),
  gsSP2Triangles(0 + 21, 1 + 21, 4 + 21, 0, 0 + 21, 4 + 21, 3 + 21, 0),
  gsSP2Triangles(1 + 21, 2 + 21, 4 + 21, 0, 2 + 21, 5 + 21, 4 + 21, 0),
  gsSP2Triangles(2 + 21, 0 + 21, 5 + 21, 0, 5 + 21, 0 + 21, 3 + 21, 0),
  gsSP2Triangles(0 + 24, 1 + 24, 4 + 24, 0, 0 + 24, 4 + 24, 3 + 24, 0),
  gsSP2Triangles(1 + 24, 2 + 24, 4 + 24, 0, 2 + 24, 5 + 24, 4 + 24, 0),
  gsSP2Triangles(2 + 24, 0 + 24, 5 + 24, 0, 5 + 24, 0 + 24, 3 + 24, 0),
  gsSP2Triangles(0 + 27, 1 + 27, 4 + 27, 0, 0 + 27, 4 + 27, 3 + 27, 0),
  gsSP2Triangles(1 + 27, 2 + 27, 4 + 27, 0, 2 + 27, 5 + 27, 4 + 27, 0),
  gsSP2Triangles(2 + 27, 0 + 27, 5 + 27, 0, 5 + 27, 0 + 27, 3 + 27, 0),
  gsSP2Triangles(0 + 30, 1 + 30, 4 + 30, 0, 0 + 30, 4 + 30, 3 + 30, 0),
  gsSP2Triangles(1 + 30, 2 + 30, 4 + 30, 0, 2 + 30, 5 + 30, 4 + 30, 0),
  gsSP2Triangles(2 + 30, 0 + 30, 5 + 30, 0, 5 + 30, 0 + 30, 3 + 30, 0),
  gsSP2Triangles(0 + 33, 1 + 33, 4 + 33, 0, 0 + 33, 4 + 33, 3 + 33, 0),
  gsSP2Triangles(1 + 33, 2 + 33, 4 + 33, 0, 2 + 33, 5 + 33, 4 + 33, 0),
  gsSP2Triangles(2 + 33, 0 + 33, 5 + 33, 0, 5 + 33, 0 + 33, 3 + 33, 0),
  gsSP2Triangles(0 + 36, 1 + 36, 4 + 36, 0, 0 + 36, 4 + 36, 3 + 36, 0),
  gsSP2Triangles(1 + 36, 2 + 36, 4 + 36, 0, 2 + 36, 5 + 36, 4 + 36, 0),
  gsSP2Triangles(2 + 36, 0 + 36, 5 + 36, 0, 5 + 36, 0 + 36, 3 + 36, 0),
  gsSP2Triangles(0 + 39, 1 + 39, 4 + 39, 0, 0 + 39, 4 + 39, 3 + 39, 0),
  gsSP2Triangles(1 + 39, 2 + 39, 4 + 39, 0, 2 + 39, 5 + 39, 4 + 39, 0),
  gsSP2Triangles(2 + 39, 0 + 39, 5 + 39, 0, 5 + 39, 0 + 39, 3 + 39, 0),
  gsSP2Triangles(0 + 42, 1 + 42, 4 + 42, 0, 0 + 42, 4 + 42, 3 + 42, 0),
  gsSP2Triangles(1 + 42, 2 + 42, 4 + 42, 0, 2 + 42, 5 + 42, 4 + 42, 0),
  gsSP2Triangles(2 + 42, 0 + 42, 5 + 42, 0, 5 + 42, 0 + 42, 3 + 42, 0),
  gsSP2Triangles(0 + 45, 1 + 45, 4 + 45, 0, 0 + 45, 4 + 45, 3 + 45, 0),
  gsSP2Triangles(1 + 45, 2 + 45, 4 + 45, 0, 2 + 45, 5 + 45, 4 + 45, 0),
  gsSP2Triangles(2 + 45, 0 + 45, 5 + 45, 0, 5 + 45, 0 + 45, 3 + 45, 0),
  gsSP2Triangles(0 + 48, 1 + 48, 4 + 48, 0, 0 + 48, 4 + 48, 3 + 48, 0),
  gsSP2Triangles(1 + 48, 2 + 48, 4 + 48, 0, 2 + 48, 5 + 48, 4 + 48, 0),
  gsSP2Triangles(2 + 48, 0 + 48, 5 + 48, 0, 5 + 48, 0 + 48, 3 + 48, 0),
  gsSP2Triangles(0 + 51, 1 + 51, 4 + 51, 0, 0 + 51, 4 + 51, 3 + 51, 0),
  gsSP2Triangles(1 + 51, 2 + 51, 4 + 51, 0, 2 + 51, 5 + 51, 4 + 51, 0),
  gsSP2Triangles(2 + 51, 0 + 51, 5 + 51, 0, 5 + 51, 0 + 51, 3 + 51, 0),
  gsSP2Triangles(0 + 54, 1 + 54, 4 + 54, 0, 0 + 54, 4 + 54, 3 + 54, 0),
  gsSP2Triangles(1 + 54, 2 + 54, 4 + 54, 0, 2 + 54, 5 + 54, 4 + 54, 0),
  gsSP2Triangles(2 + 54, 0 + 54, 5 + 54, 0, 5 + 54, 0 + 54, 3 + 54, 0),
  gsSP2Triangles(0 + 57, 1 + 57, 4 + 57, 0, 0 + 57, 4 + 57, 3 + 57, 0),
  gsSP2Triangles(1 + 57, 2 + 57, 4 + 57, 0, 2 + 57, 5 + 57, 4 + 57, 0),
  gsSP2Triangles(2 + 57, 0 + 57, 5 + 57, 0, 5 + 57, 0 + 57, 3 + 57, 0),
  gsSPClipRatio(FRUSTRATIO_2),
  gsSPEndDisplayList()
};

static Gfx mapSetionsPreable[] = {
  gsSPSetGeometryMode(G_CULL_BACK),
  gsSPTexture(0x8000, 0x8000, 0, 0, G_ON),
  gsDPPipeSync(),
  gsDPSetCombineLERP(NOISE, 0, ENVIRONMENT, TEXEL0, 0, 0, 0, SHADE, NOISE, 0, ENVIRONMENT, TEXEL0, 0, 0, 0, SHADE),
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

  // Player avatar
  { SCREEN_WD - SCISSOR_SIDES - 1 -  0 -  6 + 4,  SCISSOR_HIGH - 2,  5, 0, 32 << 6,  0 << 6, 0xCC, 0xCC, 0, 0xff },
  { SCREEN_WD - SCISSOR_SIDES - 1 - 40 - 10 + 4,  SCISSOR_HIGH - 2,  5, 0,  0 << 6,  0 << 6, 0xCC, 0xCC, 0, 0xff },
  { SCREEN_WD - SCISSOR_SIDES - 1 - 40 - 10 + 4,  SCISSOR_HIGH - 4 - 35,  5, 0,  0 << 6, 32 << 6, 0xCC, 0xCC, 0, 0xff },
  { SCREEN_WD - SCISSOR_SIDES - 1 -  0 -  6 + 4,  SCISSOR_HIGH - 4 - 35,  5, 0, 32 << 6, 32 << 6, 0xCC, 0xCC, 0, 0xff },
  { SCREEN_WD - SCISSOR_SIDES - 1 -  0 - 8 + 4,  SCISSOR_HIGH - 3,  5, 0, 48 << 6,  0 << 6, 0x22, 0x88, 0x88, 0xff },
  { SCREEN_WD - SCISSOR_SIDES - 1 - 40 - 8 + 4,  SCISSOR_HIGH - 3,  5, 0,  0 << 6,  0 << 6, 0x22, 0x88, 0x88, 0xff },
  { SCREEN_WD - SCISSOR_SIDES - 1 - 40 - 8 + 4,  SCISSOR_HIGH - 3 - 35,  5, 0,  0 << 6, 42 << 6, 0x00, 0, 0x88, 0xff },
  { SCREEN_WD - SCISSOR_SIDES - 1 -  0 - 8 + 4,  SCISSOR_HIGH - 3 - 35,  5, 0, 48 << 6, 42 << 6, 0x00, 0, 0x88, 0xff },

  // laser charge bar backing
  { SCISSOR_SIDES + 208,  SCISSOR_HIGH - 2,   5, 0, 32 << 6,  0 << 6, 0, 0, 0, 0xff },
  { SCISSOR_SIDES +   0,  SCISSOR_HIGH - 2,   5, 0,  0 << 6,  0 << 6, 0, 0, 0, 0xff },
  { SCISSOR_SIDES +   0,  SCISSOR_HIGH - 2 - 8,  5, 0,  0 << 6, 32 << 6, 0, 0, 0, 0xff },
  { SCISSOR_SIDES + 208,  SCISSOR_HIGH - 2 - 8,  5, 0, 32 << 6, 32 << 6, 0, 0, 0, 0xff },
  { SCISSOR_SIDES + 208 + 2 + 16,  SCISSOR_HIGH - 2,   5, 0, 32 << 6,  0 << 6, 0, 0, 0, 0xff },
  { SCISSOR_SIDES + 208 + 2,  SCISSOR_HIGH - 2,   5, 0,  0 << 6,  0 << 6, 0, 0, 0, 0xff },
  { SCISSOR_SIDES + 208 + 2,  SCISSOR_HIGH - 2 - 8,  5, 0,  0 << 6, 32 << 6, 0, 0, 0, 0xff },
  { SCISSOR_SIDES + 208 + 2 + 16,  SCISSOR_HIGH - 2 - 8,  5, 0, 32 << 6, 32 << 6, 0, 0, 0, 0xff },
};

static Vtx laser_charge_bar_geo[] = {
  { 0,  0,  5, 0, 32 << 6,  0 << 6, 0xCC, 0x21, 0x04, 0xff },
  { 208,  0,  5, 0,  0 << 6,  0 << 6, 0xCC, 0x21, 0, 0xff },
  { 208,  8,  5, 0,  0 << 6, 32 << 6, 0xCC, 0x21, 0, 0xff },
  { 0,  8,  5, 0, 32 << 6, 32 << 6, 0xCC, 0x21, 0x04, 0xff },

  { 208 + 2,  0,  5, 0, 32 << 6,  0 << 6, 0xCC, 0xaD, 0x04, 0xff },
  { 208 + 2 + 16,  0,  5, 0,  0 << 6,  0 << 6, 0xCC, 0xaD, 0, 0xff },
  { 208 + 2 + 16,  8,  5, 0,  0 << 6, 32 << 6, 0xCC, 0xaD, 0, 0xff },
  { 208 + 2,  8,  5, 0, 32 << 6, 32 << 6, 0xCC, 0xaD, 0x04, 0xff },
};

static Gfx hud_dl[] = {
  gsSPVertex(&hud_geo, 32, 0),
  gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
  gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
  gsSP2Triangles(8, 9, 10, 0, 8, 10, 11, 0),
  gsSP2Triangles(12, 13, 14, 0, 12, 14, 15, 0),

  gsSP2Triangles(16, 17, 18, 0, 16, 18, 19, 0),
  gsSP2Triangles(16 + 4, 17 + 4, 18 + 4, 0, 16 + 4, 18 + 4, 19 + 4, 0),

  gsSP2Triangles(24, 25, 26, 0, 24, 26, 27, 0),
  gsSP2Triangles(28, 29, 30, 0, 28, 30, 31, 0),

  gsSPEndDisplayList()
};

static Gfx laser_charge_bar_commands[] = {
  gsSPVertex(&laser_charge_bar_geo, 8, 0),
  gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
  gsSPEndDisplayList()
};

static Gfx laser_charge_almost_max[] = {
  gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
  gsSPEndDisplayList()
};

static Gfx portrait_commands[] = {
  gsSPTexture(0x8000, 0x8000, 0, 0, G_ON),
  gsDPPipeSync(),
  gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
  gsDPSetRenderMode(G_RM_TEX_EDGE, G_RM_TEX_EDGE),
  gsDPSetTextureFilter(G_TF_POINT),
  gsDPLoadTextureBlock(portrait_bin, G_IM_FMT_RGBA, G_IM_SIZ_16b, 48, 48, 0, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),

  gsSP2Triangles(16 + 4, 17 + 4, 18 + 4, 0, 16 + 4, 18 + 4, 19 + 4, 0),
  gsSPEndDisplayList()
};

static Gfx zoomed_in_dl[] = {
  gsDPPipeSync(),
  gsDPSetAlphaCompare(G_AC_NONE),
  gsDPSetCycleType(G_CYC_FILL),
  gsDPSetFillColor((GPACK_RGBA5551(255, 0, 0, 1) << 16 |  GPACK_RGBA5551(255, 0, 0, 1))),
  gsDPFillRectangle((SCREEN_WD >> 1) - 1, (SCREEN_HT >> 1) - 1, (SCREEN_WD >> 1) + 1, (SCREEN_HT >> 1) + 1),
  gsSPEndDisplayList()
};

static vec3 cameraPos = { 0.f, 0.f, 0.f };
static vec3 cameraTarget = { 0.f, 0.f, 0.f };
static vec3 cameraRotation = { 0.f, 0.f, 0.f };
static const s8 cameraYInvert = 1;

static vec3 playerPos = { 0.f, 0.f, 0.f };
static vec3 playerVelocity = { 0.f, 0.f, 0.f };
static float playerDisplayRotation = 0.f;
static int playerIsOnTheGround = 0;

static float playerZoomFactor = 0.f;
static s8 zoomState = NOT_ZOOMED_IN;

static float laserChargeFactor = 0.f;

static u8 divineLineState = DIVINE_LINE_OFF;
static float divineLineTimePassed = 0.f;
static vec3 divineLineStartSpot = { 0.f, 0.f, 0.f };
static vec3 divineLineEndSpot = { 0.f, 0.f, 0.f };

static OSTime time = 0;
static OSTime delta = 0;

static float noiseFactor = 0.f;

void initStage00(void) {
  int i;

  cameraPos = (vec3){4, 0, 20};
  cameraTarget = (vec3){10.f, 10.f, 10.f};
  cameraRotation = (vec3){0.f, 0.f, M_PI};
  playerDisplayRotation = cameraRotation.z;

  playerPos = (vec3){10.f, 10.f, 10.f};

  playerZoomFactor = 0.f;
  zoomState = NOT_ZOOMED_IN;

  laserChargeFactor = 0.f;

  divineLineState = DIVINE_LINE_OFF;
  divineLineTimePassed = 0.f;
  divineLineStartSpot = (vec3){ MAP_WIDTH * 0.5f, MAP_LENGTH * 0.5f, 60.f };
  divineLineEndSpot = (vec3){ 0.f, 0.f, 0.f };

  initKaijuCallback();

  nuPiReadRom(groundTextureROMAddress, tex_terrain_bin, tex_terrain_bin_len);
  nuPiReadRom(terrainROMAddress, GroundTexIndex, MAP_LENGTH);
  nuPiReadRom(topographyROMAddress, GroundMapping, MAP_LENGTH);

  noiseFactor = DEFAULT_NOISE_LEVEL;

  generateSectionDLs();
}

void renderDivineLine(DisplayData* dynamicp) {
  if (divineLineState != DIVINE_LINE_ON) {
    return;
  }

  guScale(&(dynamicp->divineLineScale), 0.32f, 0.32f, 1.f);
  guTranslate(&(dynamicp->divineLineTransform), divineLineStartSpot.x, divineLineStartSpot.y, divineLineStartSpot.z);

  gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(dynamicp->divineLineTransform)), G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
  gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(dynamicp->divineLineScale)), G_MTX_MODELVIEW | G_MTX_NOPUSH); 

  gSPDisplayList(glistp++, divine_line_commands);

  gSPPopMatrix(glistp++, G_MTX_MODELVIEW);
}

void makeDL00(void) {
  int i;
  DisplayData* dynamicp;
  char conbuf[20]; 
  u16 perspNorm;
  u8 envVal = (u8)(noiseFactor * 255);

  /* Specify the display list buffer */
  dynamicp = &gfx_dynamic[gfx_gtask_no];
  glistp = &gfx_glist[gfx_gtask_no][0];

  dynamicp->hitboxTransformCount = 0;

  /* Initialize RCP */
  gfxRCPInit();

  /* Clear the frame and Z-buffer */
  gfxClearCfb();

  // Initial setup
  gDPSetEnvColor(glistp++, envVal, envVal, envVal, 255);
  gDPSetScissor(glistp++, G_SC_NON_INTERLACE, SCISSOR_SIDES, SCISSOR_LOW, SCREEN_WD - SCISSOR_SIDES, SCREEN_HT - SCISSOR_HIGH);
  gDPSetCycleType(glistp++, G_CYC_1CYCLE);

  // The camera
  {
    guPerspective(&dynamicp->projection, &perspNorm, CAMERA_BASE_FOV + (CAMERA_ZOOM_FOV_CHANGE * playerZoomFactor), (float)SCREEN_WD/(float)SCREEN_HT, 0.90f, 1000.0f, 1.0f);
    gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->projection)), G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    guLookAt(&dynamicp->camera, cameraPos.x ,cameraPos.y, cameraPos.z, cameraTarget.x, cameraTarget.y, cameraTarget.z, 0, 0, 1);
    gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(dynamicp->camera)), G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
  }

  // The map
  {
    int x;
    int y;
    int xStart, xEnd;
    int yStart, yEnd;
    int xStep;
    int yStep;
    const vec3 cameraDirection = { cameraTarget.x - cameraPos.x, cameraTarget.y - cameraPos.y, cameraTarget.z - cameraPos.z };
    vec2 flatCamDir = { cameraTarget.x - cameraPos.x, cameraTarget.y - cameraPos.y };
    float invFlatCamDistSq = Q_rsqrt((flatCamDir.x * flatCamDir.x) + (flatCamDir.y * flatCamDir.y));
    flatCamDir.x *= invFlatCamDistSq;
    flatCamDir.y *= invFlatCamDistSq;

    guScale(&dynamicp->mapScale, 0.01f, 0.01f, 0.01f);
    gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(dynamicp->mapScale)), G_MTX_MODELVIEW | G_MTX_PUSH);
    gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(mapSetionsPreable));

    if (flatCamDir.x < 0.f) {
      xStart = 0;
      xEnd = 32;
      xStep = 1;
    } else {
      xStart = 32 - 1;
      xEnd = -1;
      xStep = -1;
    }
    if (flatCamDir.y < 0.f) {
      yStart = 0;
      yEnd = 32;
      yStep = 1;
    } else {
      yStart = 32 - 1;
      yEnd = -1;
      yStep = -1;
    }

    if (fabs_d(flatCamDir.x) > fabs_d(flatCamDir.y)) {
      for (x = xStart; x != xEnd; x += xStep) {
        for (y = yStart; y != yEnd; y += yStep) {
          int sectionIndex = x + (y * 32);
          const vec3 centroidDirection = { sections[sectionIndex].centroid.x - cameraPos.x, sections[sectionIndex].centroid.y - cameraPos.y, sections[sectionIndex].centroid.z - cameraPos.z };
          float dotProductFromCamera = dotProduct(&cameraDirection, &centroidDirection);
          float distanceToSectionSq = distanceSq(&(sections[sectionIndex].centroid), &cameraPos);

          // If we're on the same spot as the kaiju, render it
          if (((int)(hitboxes[0].position.x / 4.f) == x) && ((int)(hitboxes[0].position.y / 4.f) == y)) {
            gSPPopMatrix(glistp++, G_MTX_MODELVIEW);
            renderKaijuCallback(dynamicp);
            gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(dynamicp->mapScale)), G_MTX_MODELVIEW | G_MTX_PUSH);
          }

          if (dotProductFromCamera < 0.5f && (distanceToSectionSq > 16.f)) {
            continue;
          }

          if (distanceToSectionSq > FAR_PLANE_DETAIL_CUTOFF_SQ) {
            continue;
          }

          if ((distanceToSectionSq < HIGH_DETAIL_CUTOFF_SQ)) {
            gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(sections[sectionIndex].commands));
          } else {
            gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(lowDetailSections[sectionIndex].commands));
          }

          if (((int)(divineLineEndSpot.x / 4.f) == x) && ((int)(divineLineEndSpot.y / 4.f) == y)) {
            gSPPopMatrix(glistp++, G_MTX_MODELVIEW);
            renderDivineLine(dynamicp);
            gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(dynamicp->mapScale)), G_MTX_MODELVIEW | G_MTX_PUSH);
            
          }
        }
      }
    } else {
      for (y = yStart; y != yEnd; y += yStep) {
        for (x = xStart; x != xEnd; x += xStep) {
          int sectionIndex = x + (y * 32);
          const vec3 centroidDirection = { sections[sectionIndex].centroid.x - cameraPos.x, sections[sectionIndex].centroid.y - cameraPos.y, sections[sectionIndex].centroid.z - cameraPos.z };
          float dotProductFromCamera = dotProduct(&cameraDirection, &centroidDirection);
          float distanceToSectionSq = distanceSq(&(sections[sectionIndex].centroid), &cameraPos);
 
          // If we're on the same spot as the kaiju, render it
          if (((int)(hitboxes[0].position.x / 4.f) == x) && ((int)(hitboxes[0].position.y / 4.f) == y)) {
            gSPPopMatrix(glistp++, G_MTX_MODELVIEW);
            renderKaijuCallback(dynamicp);
            gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(dynamicp->mapScale)), G_MTX_MODELVIEW | G_MTX_PUSH);
          }

          if (dotProductFromCamera < 0.5f && (distanceToSectionSq > 16.f)) {
            continue;
          }

          if (distanceToSectionSq > FAR_PLANE_DETAIL_CUTOFF_SQ) {
            continue;
          }

          if ((distanceToSectionSq < HIGH_DETAIL_CUTOFF_SQ) || ((distanceToSectionSq < FOCUS_HIGH_DETAIL_CUTOFF_SQ) && (dotProductFromCamera > 0.8975f))) {
            gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(sections[sectionIndex].commands));
          } else {
            gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(lowDetailSections[sectionIndex].commands));
          }

          if (((int)(divineLineEndSpot.x / 4.f) == x) && ((int)(divineLineEndSpot.y / 4.f) == y)) {
            gSPPopMatrix(glistp++, G_MTX_MODELVIEW);
            renderDivineLine(dynamicp);
            gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(dynamicp->mapScale)), G_MTX_MODELVIEW | G_MTX_PUSH);
          }
        }
      }
    }

    gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(mapSectionsPostamble));
    gSPPopMatrix(glistp++, G_MTX_MODELVIEW);
  }

  gDPPipeSync(glistp++);
  gDPSetCombineLERP(glistp++, NOISE, 0, ENVIRONMENT, SHADE, 0, 0, 0, SHADE, NOISE, 0, ENVIRONMENT, SHADE, 0, 0, 0, SHADE);
  gDPSetRenderMode(glistp++, G_RM_ZB_OPA_SURF, G_RM_ZB_OPA_SURF2);
  gSPClearGeometryMode(glistp++, 0xFFFFFFFF);
  gSPSetGeometryMode(glistp++, G_SHADE | G_SHADING_SMOOTH | G_CULL_BACK | G_ZBUFFER);

  // The player
  if (zoomState != ZOOMED_IN) {
    guTranslate(&(dynamicp->playerTranslation), playerPos.x, playerPos.y, playerPos.z);
    gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(dynamicp->playerTranslation)), G_MTX_MODELVIEW | G_MTX_PUSH);

    guRotate(&(dynamicp->playerRotation), (playerDisplayRotation - (M_PI * 1.5f)) * RAD_TO_DEG, 0.f, 0.f, 1.f);
    gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(dynamicp->playerRotation)), G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);

    guScale(&(dynamicp->playerScale), protag_scale, protag_scale, protag_scale);
    gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(dynamicp->playerScale)), G_MTX_MODELVIEW | G_MTX_NOPUSH);
    gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(protag_head_commands));
    gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(protag_body_commands));
    gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(protag_legs_commands));
    gSPPopMatrix(glistp++, G_MTX_MODELVIEW);
  }

  // HUD
  {
    gDPPipeSync(glistp++);
    gDPSetCombineMode(glistp++, G_CC_SHADE, G_CC_SHADE);
    gDPSetScissor(glistp++, G_SC_NON_INTERLACE, 0, 0, SCREEN_WD - 1, SCREEN_HT - 1);
    gDPSetRenderMode(glistp++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gSPClearGeometryMode(glistp++,0xFFFFFFFF);
    gSPSetGeometryMode(glistp++, G_SHADE | G_SHADING_SMOOTH | G_CULL_BACK);

    guOrtho(&(dynamicp->orthoHudProjection), 0, SCREEN_WD - 1, 0, SCREEN_HT - 1, 0.f, 10.f, 1.f);
    gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->orthoHudProjection)), G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    guMtxIdent(&(dynamicp->orthoHudModelling));
    gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->orthoHudModelling)), G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);

    // Hud background
    gSPDisplayList(glistp++, hud_dl);

    guTranslate(&(dynamicp->laserBarTranslation), SCISSOR_SIDES, SCISSOR_HIGH - 10, 0.f);
    if (laserChargeFactor < 0.9f) {
      guScale(&(dynamicp->laserBarScale), cubic(laserChargeFactor / 0.9f), 1.f, 1.f);
    } else {
      guScale(&(dynamicp->laserBarScale), 1.f, 1.f, 1.f);
    }
    gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->laserBarTranslation)), G_MTX_MODELVIEW | G_MTX_PUSH);
    gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->laserBarScale)), G_MTX_MODELVIEW | G_MTX_NOPUSH);
    gSPDisplayList(glistp++, laser_charge_bar_commands);
    if (laserChargeFactor > 0.9f) {
      gSPDisplayList(glistp++, laser_charge_almost_max);
    }
    gSPPopMatrix(glistp++, G_MTX_MODELVIEW);

    gSPDisplayList(glistp++, portrait_commands);

    if (zoomState == ZOOMED_IN) {
      gSPDisplayList(glistp++, zoomed_in_dl);
    }
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
      nuDebConTextPos(0,4,4);
      sprintf(conbuf, "DL: %3d/%3d", (glistp - gfx_glist[gfx_gtask_no]), GFX_GLIST_LEN );
      nuDebConCPuts(0, conbuf);

    }
  else
    {
      nuDebConTextPos(0,4,4);
      nuDebConCPuts(0, "Connect controller #1, kid!");
    }

    nuDebTaskPerfBar1(2, 200, NU_SC_NOSWAPBUFFER);
    
  nuDebConDisp(NU_SC_SWAPBUFFER);

  /* Switch display list buffers */
  gfx_gtask_no = (gfx_gtask_no + 1) % 3;
}

float getDistanceFromGround(const vec3* pos) {
  const float height = getHeight(pos->x, pos->y);
  return pos->z - height;
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
    cameraRotation.z += (CAMERA_TURN_SPEED_X + (CAMERA_TURN_SPEED_ADJUSTMENT_WHILE_ZOOMED * playerZoomFactor)) * deltaSeconds;
  }
  else if (contdata->button & R_CBUTTONS) {
    cameraRotation.z -= (CAMERA_TURN_SPEED_X + (CAMERA_TURN_SPEED_ADJUSTMENT_WHILE_ZOOMED * playerZoomFactor)) * deltaSeconds;
  }

  if (contdata->button & U_CBUTTONS) {
    cameraRotation.y = MAX(-M_PI * 0.35f, cameraRotation.y - ((CAMERA_TURN_SPEED_Y + (CAMERA_TURN_SPEED_ADJUSTMENT_WHILE_ZOOMED * playerZoomFactor)) * deltaSeconds * cameraYInvert));
  }
  else if (contdata->button & D_CBUTTONS) {
    cameraRotation.y = MIN(M_PI * 0.35f, cameraRotation.y + ((CAMERA_TURN_SPEED_Y + (CAMERA_TURN_SPEED_ADJUSTMENT_WHILE_ZOOMED * playerZoomFactor)) * deltaSeconds * cameraYInvert));
  }

  if (contdata->button & Z_TRIG) {
    inputDirectionX = 0.f;
    inputDirectionY = 0.f;
  }

  cosCameraRot = cosf(cameraRotation.z + M_PI * 0.5f);
  sinCameraRot = sinf(cameraRotation.z + M_PI * 0.5f);
  inputDirectionXRotated = (cosCameraRot * inputDirectionX) - (sinCameraRot * inputDirectionY);
  inputDirectionYRotated = (sinCameraRot * inputDirectionX) + (cosCameraRot * inputDirectionY);

  playerVelocity.x = inputDirectionXRotated * PLAYER_MOVE_SPEED;
  playerVelocity.y = inputDirectionYRotated * PLAYER_MOVE_SPEED;
  playerVelocity.z += GRAVITY * deltaSeconds;

  if (fabs_d(playerVelocity.x) > 0.01f || fabs_d(playerVelocity.y) > 0.01f) {
    const float angle = nu_atan2(playerVelocity.y, playerVelocity.x);
    playerDisplayRotation = lerp(playerDisplayRotation, angle, 0.12f);
  }

  if (playerIsOnTheGround && (contdata->trigger & A_BUTTON)) {
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
  cameraTarget.z = lerp( cameraTarget.z, playerPos.z + CAMERA_LIFT_FRONT, 0.12f);
  cameraPos.x = cameraTarget.x + (cosf(cameraRotation.z) * (CAMERA_DISTANCE) * cosf(cameraRotation.y));
  cameraPos.y = cameraTarget.y + (sinf(cameraRotation.z) * (CAMERA_DISTANCE) * cosf(cameraRotation.y));
  cameraPos.z = cameraTarget.z + (CAMERA_DISTANCE * sinf(cameraRotation.y)) + CAMERA_LIFT_BACK;

  if ((zoomState == NOT_ZOOMED_IN) && (contdata->trigger & R_TRIG)) {
    zoomState = ZOOMED_IN;
  }
  if ((zoomState == ZOOMED_IN) && (!(contdata->button & R_TRIG))) {
    zoomState = NOT_ZOOMED_IN;
  }

  playerZoomFactor = clamp(playerZoomFactor + (ZOOM_IN_OUT_SPEED * deltaSeconds * zoomState), 0.f, 1.f);

  if (laserChargeFactor < 0.01f && (zoomState == ZOOMED_IN) && (contdata->trigger & Z_TRIG)) {
    laserChargeFactor = 0.01f;
  }

  if ((laserChargeFactor >= 0.01f) && (contdata->button & Z_TRIG) && (zoomState == ZOOMED_IN)) {
    laserChargeFactor = clamp(laserChargeFactor + (LASER_CHARGE_SPEED * deltaSeconds * 1), 0.f, 1.f);
  } else {
    laserChargeFactor = clamp(laserChargeFactor + (LASER_CHARGE_SPEED * deltaSeconds * -1), 0.f, 1.f);
  }
}

// TODO: move this to its own "parent kaiju" file
void updateKaijuHitboxes(float delta) {
  int i;
  mat4 positionMatrix;
  mat4 rotationMatrix;
  mat4 scaleMatrix;

  for (i = 0; i < NUMBER_OF_KAIJU_HITBOXES; i++) {
    KaijuHitbox* hitbox = &(hitboxes[i]);

    if (!(hitbox->alive)) {
      continue;
    }

    if (hitbox->isTransformDirty) {
      guTranslateF(positionMatrix.data, hitbox->position.x, hitbox->position.y, hitbox->position.z);
      guRotateRPYF(rotationMatrix.data, hitbox->rotation.x, hitbox->rotation.y, hitbox->rotation.z);
      guScaleF(scaleMatrix.data, hitbox->scale.x, hitbox->scale.y, hitbox->scale.z);
      
      guMtxCatF(scaleMatrix.data, rotationMatrix.data, hitbox->computedTransform.data);
      guMtxCatF(hitbox->computedTransform.data, positionMatrix.data, hitbox->computedTransform.data);

      mat4x4_invert(&(hitbox->computedInverse), &(hitbox->computedTransform));

      hitbox->isTransformDirty = 0;
    }
  }
}

void fireLaser(const vec3* location) {
  laserChargeFactor = 0.f;
  zoomState = NOT_ZOOMED_IN;

  divineLineStartSpot = (vec3){ location->x, location->y, DIVINE_LINE_START_HEIGHT };
  divineLineEndSpot = (vec3){ location->x, location->y, location->z };

  divineLineState = DIVINE_LINE_ON;
  divineLineTimePassed = 0.f;
}

void getParentAdjustedInverses(mat4* result, KaijuHitbox* hitbox) {
  if (hitbox->parent) {
    getParentAdjustedInverses(result, hitbox->parent);
    guMtxCatF(result->data, hitbox->computedInverse.data, result->data);
  } else {
    *result = hitbox->computedInverse;
  }
}

void raymarchAimLineAgainstHitboxes() {
  int stepI;
  vec3 checkPoint = cameraPos;
  float nextStepDistance = INITIAL_STEP_DISTANCE;
  const vec3 aimDirection = { cameraTarget.x - cameraPos.x, cameraTarget.y - cameraPos.y, cameraTarget.z - cameraPos.z };
  const float aimDirLengthSq = distanceSq(&aimDirection, &zeroVector);
  const float invAimDirLengthSq = Q_rsqrt(aimDirLengthSq);
  const vec3 normalizedAimDirection = { aimDirection.x * invAimDirLengthSq, aimDirection.y * invAimDirLengthSq, aimDirection.z * invAimDirLengthSq };
  int hitAnything = 0;

  for (stepI = 0; stepI < MAX_STEP_COUNT; stepI++) {
    int i;
    float closestDistance = 9999999.f; // TODO: make this a special max-float
    KaijuHitbox* closestHitbox = NULL;

    checkPoint = (vec3){ checkPoint.x + (nextStepDistance * normalizedAimDirection.x), checkPoint.y + (nextStepDistance * normalizedAimDirection.y), checkPoint.z + (nextStepDistance * normalizedAimDirection.z) };

    // Check against the ground
    closestDistance = getDistanceFromGround(&(checkPoint));

    // If we're below ground, then we've hit the laser against that
    if (closestDistance <= 0) {
      fireLaser(&(checkPoint));
      noiseFactor = 0.07f;
      hitAnything = 1;

      break;
    }

    // Check agianst each of the Kaiju hitboxes
    for (i = 0; i < NUMBER_OF_KAIJU_HITBOXES; i++) {
      KaijuHitbox* hitbox = &(hitboxes[i]);
      vec3 checkPointInHitboxSpace;
      mat4 inverseToCheck;
      float distanceToHitbox = 9999999.f; // TODO: make this a special max-float

      if (!(hitbox->alive)) {
        continue;
      }

      getParentAdjustedInverses(&inverseToCheck, hitbox);

      guMtxXFMF(inverseToCheck.data, checkPoint.x, checkPoint.y, checkPoint.z, &(checkPointInHitboxSpace.x), &(checkPointInHitboxSpace.y), &(checkPointInHitboxSpace.z));
      distanceToHitbox = hitbox->check(&checkPointInHitboxSpace);
      if (distanceToHitbox < closestDistance) {
        closestDistance = distanceToHitbox;
        closestHitbox = hitbox;
      }
    }

    // If the SDF distance is less than zero to something, we've hit it
    if ((closestDistance <= 0) && (closestHitbox != NULL)) {
      fireLaser(&(checkPoint));
      hitAnything = 1;
      noiseFactor = 0.10000524f;

      if (closestHitbox->destroyable) {
        closestHitbox->alive = 0;
        noiseFactor = 1.00f;
      }
      break;
    }

    nextStepDistance = closestDistance + 0.001f;
  }

  if (!hitAnything) {
    laserChargeFactor = 0.f;
    noiseFactor = 0.f;
  }
}

void updateHitboxCheck() {
  if ((zoomState == ZOOMED_IN) && (contdata->button & Z_TRIG) && (laserChargeFactor > 0.98f)) {
    raymarchAimLineAgainstHitboxes();
  }
}

void updateDivineLine(float deltaSeconds) {
  if (divineLineState == DIVINE_LINE_ON) {
    divineLineTimePassed += deltaSeconds;
    divineLineStartSpot.z -= deltaSeconds * 98.f;

    if (divineLineTimePassed > DIVINE_LINE_DURATION) {
      divineLineState = DIVINE_LINE_OFF;
    }
  }
}

void checkIfPlayerHasWon() {
  int i;
  int defeatedAllHitboxes = 1;

  for (i = 0; i < NUMBER_OF_KAIJU_HITBOXES; i++) {
    if (hitboxes[i].alive && hitboxes[i].destroyable) {
      defeatedAllHitboxes = 0;
      break;
    }
  }

  if (defeatedAllHitboxes) {
    screenType = DialogueScreen;
    changeScreensFlag = 1;
  }
}

void updateGame00(void) {
  int i;
  float deltaSeconds = 0.f;
  OSTime newTime = OS_CYCLES_TO_USEC(osGetTime());

  nuContDataGetEx(contdata,0);

  delta = (newTime - time);
  time = newTime;
  deltaSeconds = delta * 0.000001f;

  noiseFactor = lerp(noiseFactor, DEFAULT_NOISE_LEVEL, 0.04f);

  nuDebPerfMarkSet(0);
  updateKaijuCallback(deltaSeconds);
  nuDebPerfMarkSet(1);
  updateKaijuHitboxes(deltaSeconds);
  nuDebPerfMarkSet(2);
  updatePlayer(deltaSeconds);
  nuDebPerfMarkSet(3);
  updateHitboxCheck();
  nuDebPerfMarkSet(4);
  updateDivineLine(deltaSeconds);
  nuDebPerfMarkSet(5);
  checkIfPlayerHasWon();

}
