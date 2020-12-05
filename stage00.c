#include <assert.h>
#include <nusys.h>
#include <nualsgi_n.h>
#include "stage00.h"
#include "main.h"
#include "segmentinfo.h"
#include "soundid.h"

#include "map.h"
#include "graphic.h"
#include "gamemath.h"
#include "terraintex.h"
#include "time_tex.h"
#include "hitboxes.h"
#include "portraittex.h"
#include "protaggeo.h"
#include "kaiju1.h"

#include "debug.h"

#define SCISSOR_HIGH 64
#define SCISSOR_LOW 24
#define SCISSOR_SIDES 22

#define CAMERA_BASE_FOV 60
#define CAMERA_ZOOM_FOV_CHANGE -35
#define CAMERA_MOVE_SPEED 10.f
#define CAMERA_TURN_SPEED_X 4.373f
#define CAMERA_TURN_SPEED_Y 5.373f
#define CAMERA_TURN_SPEED_X_DIGITAL 2.373f
#define CAMERA_TURN_SPEED_Y_DIGITAL 2.373f
#define CAMERA_TURN_SPEED_ADJUSTMENT_WHILE_ZOOMED -2.2f
#define CAMERA_DISTANCE 3.f
#define CAMERA_LIFT_FRONT 0.7f
#define CAMERA_LIFT_BACK 1.2f

#define FAR_PLANE_DETAIL_CUTOFF 128.f
#define FAR_PLANE_DETAIL_CUTOFF_SQ (FAR_PLANE_DETAIL_CUTOFF * FAR_PLANE_DETAIL_CUTOFF)

#define FOCUS_HIGH_DETAIL_CUTOFF 34.f
#define FOCUS_HIGH_DETAIL_CUTOFF_SQ (FOCUS_HIGH_DETAIL_CUTOFF * FOCUS_HIGH_DETAIL_CUTOFF)

#define HIGH_DETAIL_CUTOFF 70.f
#define HIGH_DETAIL_CUTOFF_SQ (HIGH_DETAIL_CUTOFF * HIGH_DETAIL_CUTOFF)

#define PLAYER_MOVE_SPEED 6.f
#define GRAVITY -5.2f
#define JUMP_VELOCITY 6.f
#define STEP_HEIGHT_CUTOFF 0.0915f

#define NOT_ZOOMED_IN -1
#define ZOOMED_IN 1
#define ZOOM_IN_OUT_SPEED 4.f

#define LASER_CHARGE_SPEED 0.49151f

#define MAX_STEP_COUNT 64
#define INITIAL_STEP_DISTANCE 4.0f

#define DEFAULT_NOISE_LEVEL 0.039f

#define DIVINE_LINE_OFF 0
#define DIVINE_LINE_ON 1
#define DIVINE_LINE_DURATION 100.4f
#define INV_DIVINE_LINE_DURATION (1.f / DIVINE_LINE_DURATION)
#define DIVINE_LINE_START_HEIGHT 70

#define TIME_PER_CHARGE_BIP 0.10161f

#define CINEMA_DURATION 5.f
#define CINEMA_LONG_DURATION 8.7f

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
  gsSPClearGeometryMode(0xFFFFFFFF),
  gsSPSetGeometryMode(G_SHADE | G_SHADING_SMOOTH | G_CULL_BACK),
  gsDPSetCombineLERP(NOISE, 0, ENVIRONMENT, SHADE, 0, 0, 0, SHADE, NOISE, 0, ENVIRONMENT, SHADE, 0, 0, 0, SHADE),
  gsDPPipeSync(),
  gsSPTexture(0x8000, 0x8000, 0, 0, G_OFF),
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
  gsSPTexture(0x8000, 0x8000, 0, 0, G_ON),
  gsSPClearGeometryMode(0xFFFFFFFF),
  gsSPSetGeometryMode(G_CULL_BACK),
  gsDPSetCombineLERP(NOISE, 0, ENVIRONMENT, TEXEL0, 0, 0, 0, SHADE, NOISE, 0, ENVIRONMENT, TEXEL0, 0, 0, 0, SHADE),
  gsDPPipeSync(),
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


  { SCISSOR_SIDES + 224,       SCISSOR_HIGH - 2 - 10,   5, 0, 32 << 6,  0 << 6, 0x23, 0x23, 0x23, 0xff },
  { SCISSOR_SIDES + 224 - 38,  SCISSOR_HIGH - 2 - 10,   5, 0,  0 << 6,  0 << 6, 0x23, 0x23, 0x23, 0xff },
  { SCISSOR_SIDES + 224 - 38,  SCISSOR_HIGH - 2 - 12 - 24,  5, 0,  0 << 6, 32 << 6, 0x18, 0x18, 0x18, 0xff },
  { SCISSOR_SIDES + 224,       SCISSOR_HIGH - 2 - 12 - 24,   5, 0, 32 << 6, 32 << 6, 0x18, 0x18, 0x18, 0xff },
};

static Vtx hud_remaining_targets[] = {
  {  SCISSOR_SIDES + 2 +  0, 28 + 12,  5, 0, 8 << 6, 0 << 6, 0xFF, 0x00, 0x00, 0xff },
  {  SCISSOR_SIDES + 2 + 24, 28 + 12,  5, 0, 0 << 6, 0 << 6, 0xEC, 0x55, 0x50, 0xff },
  {  SCISSOR_SIDES + 2 + 12, 28 + 24,  5, 0, 0 << 6, 8 << 6, 0xFF, 0x00, 0x00, 0xff },
  {  SCISSOR_SIDES + 2 + 12, 28 +  0,  5, 0, 8 << 6, 8 << 6, 0x55, 0x00, 0x00, 0xff },

  {  12 + SCISSOR_SIDES + 2 +  0, 28 + 12,  5, 0, 8 << 6, 0 << 6, 0xFF, 0x00, 0x00, 0xff },
  {  12 + SCISSOR_SIDES + 2 + 24, 28 + 12,  5, 0, 0 << 6, 0 << 6, 0xEC, 0x55, 0x50, 0xff },
  {  12 + SCISSOR_SIDES + 2 + 12, 28 + 24,  5, 0, 0 << 6, 8 << 6, 0xFF, 0x00, 0x00, 0xff },
  {  12 + SCISSOR_SIDES + 2 + 12, 28 +  0,  5, 0, 8 << 6, 8 << 6, 0x55, 0x00, 0x00, 0xff },

  {  24 + SCISSOR_SIDES + 2 +  0, 28 + 12,  5, 0, 8 << 6, 0 << 6, 0xFF, 0x00, 0x00, 0xff },
  {  24 + SCISSOR_SIDES + 2 + 24, 28 + 12,  5, 0, 0 << 6, 0 << 6, 0xEC, 0x55, 0x50, 0xff },
  {  24 + SCISSOR_SIDES + 2 + 12, 28 + 24,  5, 0, 0 << 6, 8 << 6, 0xFF, 0x00, 0x00, 0xff },
  {  24 + SCISSOR_SIDES + 2 + 12, 28 +  0,  5, 0, 8 << 6, 8 << 6, 0x55, 0x00, 0x00, 0xff },

  {  36 + SCISSOR_SIDES + 2 +  0, 28 + 12,  5, 0, 8 << 6, 0 << 6, 0xFF, 0x00, 0x00, 0xff },
  {  36 + SCISSOR_SIDES + 2 + 24, 28 + 12,  5, 0, 0 << 6, 0 << 6, 0xEC, 0x55, 0x50, 0xff },
  {  36 + SCISSOR_SIDES + 2 + 12, 28 + 24,  5, 0, 0 << 6, 8 << 6, 0xFF, 0x00, 0x00, 0xff },
  {  36 + SCISSOR_SIDES + 2 + 12, 28 +  0,  5, 0, 8 << 6, 8 << 6, 0x55, 0x00, 0x00, 0xff },

  {  48 + SCISSOR_SIDES + 2 +  0, 28 + 12,  5, 0, 8 << 6, 0 << 6, 0xFF, 0x00, 0x00, 0xff },
  {  48 + SCISSOR_SIDES + 2 + 24, 28 + 12,  5, 0, 0 << 6, 0 << 6, 0xEC, 0x55, 0x50, 0xff },
  {  48 + SCISSOR_SIDES + 2 + 12, 28 + 24,  5, 0, 0 << 6, 8 << 6, 0xFF, 0x00, 0x00, 0xff },
  {  48 + SCISSOR_SIDES + 2 + 12, 28 +  0,  5, 0, 8 << 6, 8 << 6, 0x55, 0x00, 0x00, 0xff },

  {  60 + SCISSOR_SIDES + 2 +  0, 28 + 12,  5, 0, 8 << 6, 0 << 6, 0xFF, 0x00, 0x00, 0xff },
  {  60 + SCISSOR_SIDES + 2 + 24, 28 + 12,  5, 0, 0 << 6, 0 << 6, 0xEC, 0x55, 0x50, 0xff },
  {  60 + SCISSOR_SIDES + 2 + 12, 28 + 24,  5, 0, 0 << 6, 8 << 6, 0xFF, 0x00, 0x00, 0xff },
  {  60 + SCISSOR_SIDES + 2 + 12, 28 +  0,  5, 0, 8 << 6, 8 << 6, 0x55, 0x00, 0x00, 0xff },

  {  72 + SCISSOR_SIDES + 2 +  0, 28 + 12,  5, 0, 8 << 6, 0 << 6, 0xFF, 0x00, 0x00, 0xff },
  {  72 + SCISSOR_SIDES + 2 + 24, 28 + 12,  5, 0, 0 << 6, 0 << 6, 0xEC, 0x55, 0x50, 0xff },
  {  72 + SCISSOR_SIDES + 2 + 12, 28 + 24,  5, 0, 0 << 6, 8 << 6, 0xFF, 0x00, 0x00, 0xff },
  {  72 + SCISSOR_SIDES + 2 + 12, 28 +  0,  5, 0, 8 << 6, 8 << 6, 0x55, 0x00, 0x00, 0xff },

  {  84 + SCISSOR_SIDES + 2 +  0, 28 + 12,  5, 0, 8 << 6, 0 << 6, 0xFF, 0x00, 0x00, 0xff },
  {  84 + SCISSOR_SIDES + 2 + 24, 28 + 12,  5, 0, 0 << 6, 0 << 6, 0xEC, 0x55, 0x50, 0xff },
  {  84 + SCISSOR_SIDES + 2 + 12, 28 + 24,  5, 0, 0 << 6, 8 << 6, 0xFF, 0x00, 0x00, 0xff },
  {  84 + SCISSOR_SIDES + 2 + 12, 28 +  0,  5, 0, 8 << 6, 8 << 6, 0x55, 0x00, 0x00, 0xff },

  {  96 + SCISSOR_SIDES + 2 +  0, 28 + 12,  5, 0, 8 << 6, 0 << 6, 0xFF, 0x00, 0x00, 0xff },
  {  96 + SCISSOR_SIDES + 2 + 24, 28 + 12,  5, 0, 0 << 6, 0 << 6, 0xEC, 0x55, 0x50, 0xff },
  {  96 + SCISSOR_SIDES + 2 + 12, 28 + 24,  5, 0, 0 << 6, 8 << 6, 0xFF, 0x00, 0x00, 0xff },
  {  96 + SCISSOR_SIDES + 2 + 12, 28 +  0,  5, 0, 8 << 6, 8 << 6, 0x55, 0x00, 0x00, 0xff },
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
  gsSPVertex(&hud_geo, 36, 0),
  gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
  gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
  gsSP2Triangles(8, 9, 10, 0, 8, 10, 11, 0),
  gsSP2Triangles(12, 13, 14, 0, 12, 14, 15, 0),

  gsSP2Triangles(16, 17, 18, 0, 16, 18, 19, 0),
  gsSP2Triangles(16 + 4, 17 + 4, 18 + 4, 0, 16 + 4, 18 + 4, 19 + 4, 0),

  gsSP2Triangles(24, 25, 26, 0, 24, 26, 27, 0),
  gsSP2Triangles(28, 29, 30, 0, 28, 30, 31, 0),

  gsSP2Triangles(32, 33, 34, 0, 32, 34, 35, 0),

  gsSPEndDisplayList()
};

static Vtx hud_zoomed_in_geo[] = {
  { SCREEN_WD / 2 + 2,  SCREEN_HT / 2 + 2,  5, 0, 32 << 6,  0 << 6, 0xFF, 0x00, 0x00, 0xff },
  { SCREEN_WD / 2 - 2,  SCREEN_HT / 2 + 2,  5, 0,  0 << 6,  0 << 6, 0xFF, 0x00, 0x00, 0xff },
  { SCREEN_WD / 2 - 2,  SCREEN_HT / 2 - 2,  5, 0,  0 << 6, 32 << 6, 0xFF, 0x33, 0x00, 0xff },
  { SCREEN_WD / 2 + 2,  SCREEN_HT / 2 - 2,  5, 0, 32 << 6, 32 << 6, 0xFF, 0x33, 0x00, 0xff },

  { SCREEN_WD / 2 + 32 + 4 + 16,  SCISSOR_HIGH + 16,  5, 0, 32 << 6, 32 << 6, 0xAC, 0x12, 0x12, 0xff },
  { SCREEN_WD / 2 + 32 + 0 + 16,  SCISSOR_HIGH + 14,  5, 0, 32 << 6, 32 << 6, 0xAC, 0x12, 0x12, 0xff },
  { SCREEN_WD / 2 + 32 + 0,  SCISSOR_HIGH +  0,  5, 0, 32 << 6, 32 << 6, 0xAC, 0x12, 0x12, 0xff },
  { SCREEN_WD / 2 + 32 + 4,  SCISSOR_HIGH +  0,  5, 0, 32 << 6, 32 << 6, 0xAC, 0x12, 0x12, 0xff },

  { SCREEN_WD / 2 + 32 + 4 + 24,  SCISSOR_HIGH + 32,  5, 0, 32 << 6, 32 << 6, 0xAC, 0x12, 0x12, 0xff },
  { SCREEN_WD / 2 + 32 + 0 + 24,  SCISSOR_HIGH + 30,  5, 0, 32 << 6, 32 << 6, 0xAC, 0x12, 0x12, 0xff },
  { SCREEN_WD / 2 + 32 + 4 + 24,  SCISSOR_HIGH + 64,  5, 0, 32 << 6, 32 << 6, 0xAC, 0x12, 0x12, 0xff },
  { SCREEN_WD / 2 + 32 + 0 + 24,  SCISSOR_HIGH + 60,  5, 0, 32 << 6, 32 << 6, 0xAC, 0x12, 0x12, 0xff },

  { SCREEN_WD / 2 + 32 + 4,  SCISSOR_HIGH + 64,  5, 0, 32 << 6, 32 << 6, 0xAC, 0x00, 0x00, 0xff },
  { SCREEN_WD / 2 + 32 + 0,  SCISSOR_HIGH + 60,  5, 0, 32 << 6, 32 << 6, 0xAC, 0x00, 0x00, 0xff },
};

static Gfx hud_zoomed_in_commands[] = {
  gsSPVertex(&hud_zoomed_in_geo, 14, 0),
  gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
  gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
  gsSP2Triangles(4, 8, 5, 0, 4, 8, 9, 0),
  gsSP2Triangles(8, 10, 9, 0, 8, 10, 11, 0),
  gsSP2Triangles(10, 12, 11, 0, 10, 11, 13, 0),

  gsDPPipeSync(),
  gsDPSetCycleType(G_CYC_FILL),
  gsDPSetFillColor((GPACK_RGBA5551(16, 0, 0, 1) << 16 | GPACK_RGBA5551(16, 0, 0, 1))),
  gsDPFillRectangle(SCISSOR_SIDES + 2, SCISSOR_LOW + 2, SCISSOR_SIDES + 3, SCISSOR_LOW + (SCREEN_HT - SCISSOR_LOW - SCISSOR_HIGH - 2)),
  gsDPFillRectangle((SCREEN_WD - SCISSOR_SIDES) - 4, SCISSOR_LOW + 2, (SCREEN_WD - SCISSOR_SIDES) - 3, SCISSOR_LOW + (SCREEN_HT - SCISSOR_LOW - SCISSOR_HIGH - 2)),
  gsDPFillRectangle(SCREEN_WD / 2 + 6, SCREEN_HT / 2, SCREEN_WD / 2 + 10, SCREEN_HT / 2 + 1),
  gsDPFillRectangle(SCREEN_WD / 2 + 6, SCREEN_HT / 2 + 3, SCREEN_WD / 2 + 10, SCREEN_HT / 2 + 4),
  gsDPFillRectangle(SCREEN_WD / 2 + 6, SCREEN_HT / 2 + 6, SCREEN_WD / 2 + 10, SCREEN_HT / 2 + 10),
  gsDPSetCycleType(G_CYC_1CYCLE),
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
  gsDPSetTexturePersp(G_TP_NONE),
  gsSPEndDisplayList()
};

static Gfx time_prefix_commands[] = {
  gsDPPipeSync(),
  gsDPLoadTextureBlock(time_tex_bin, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, G_TX_CLAMP, G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),

  gsSPTextureRectangle((211 + 0) << 2, (192) << 2, (211 + 32) << 2, (192 + 8) << 2, 0, 0 << 5, 0 << 5, 1 << 10, 1 << 10),
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

vec3 cameraPos = { 0.f, 0.f, 0.f };
vec3 cameraTarget = { 0.f, 0.f, 0.f };
vec3 cameraRotation = { 0.f, 0.f, 0.f };
static const s8 cameraYInvert = -1;

vec3 playerPos = { 0.f, 0.f, 0.f };
vec3 playerVelocity = { 0.f, 0.f, 0.f };
static float playerDisplayRotation = 0.f;
static int playerIsOnTheGround = 0;

static float playerZoomFactor = 0.f;
static s8 zoomState = NOT_ZOOMED_IN;

static float laserChargeFactor = 0.f;
static float laserChargeDelta = 0.2f;

static u8 divineLineState = DIVINE_LINE_OFF;
static float divineLineTimePassed = 0.f;
static vec3 divineLineStartSpot = { 0.f, 0.f, 0.f };
static vec3 divineLineEndSpot = { 0.f, 0.f, 0.f };

static float timeRemaining = 0.f;
static u8 hundredsDigit;
static u8 majorDigit;
static u8 minorDigit;
static u8 s0;
static u8 t0;
static u8 s1;
static u8 t1;
static u8 s2;
static u8 t2;

static OSTime time = 0;
static OSTime delta = 0;

static float noiseFactor = 0.f;

static u8 cinemaMode = 0;
static float cinemaTime = 0;
static u8 ending = 0;
static u8 sinking = 0;
static float explosionTime = 0;

void initStage00(void) {
  int i;
  u32 status;

  //  asm volatile("cfc1 %0, $31"
  //                : "=r"(status));

  // status &= ~(0x1F << 7);

  // asm volatile("ctc1 %0, $31"
  //                :: "r"(status));

  // debug_initialize();

  cameraPos = (vec3){4, 0, 20};
  cameraTarget = (vec3){10.f, 10.f, 10.f};
  cameraRotation = (vec3){0.f, 0.f, M_PI};
  playerDisplayRotation = cameraRotation.z;

  playerPos = (vec3){32.f, 32.f, 0.f};

  playerZoomFactor = 0.f;
  zoomState = NOT_ZOOMED_IN;

  laserChargeFactor = 0.f;

  divineLineState = DIVINE_LINE_OFF;
  divineLineTimePassed = 0.f;
  divineLineStartSpot = (vec3){ MAP_WIDTH * 0.5f, MAP_LENGTH * 0.5f, 60.f };
  divineLineEndSpot = (vec3){ 0.f, 0.f, 0.f };

  timeRemaining = 150.f;

  initKaijuCallback();

  nuPiReadRom(groundTextureROMAddress, tex_terrain_bin, tex_terrain_bin_len);
  nuPiReadRom(terrainROMAddress, GroundTexIndex, MAP_LENGTH);
  nuPiReadRom(topographyROMAddress, GroundMapping, MAP_LENGTH);

  noiseFactor = DEFAULT_NOISE_LEVEL;

  generateSectionDLs();

  nuAuSeqPlayerStop(0);

  if (currentLevel == 0) {
    nuAuSeqPlayerSetNo(0, 1);
  } else {
    nuAuSeqPlayerSetNo(0, 3);
  }
  nuAuSeqPlayerPlay(0);

  time = OS_CYCLES_TO_USEC(osGetTime());
  delta = 0;

  cinemaMode = 1;
  cinemaTime = 0;
  ending = 0;
  sinking = 0;
  explosionTime = 0;
}

void renderDivineLine(DisplayData* dynamicp) {
  if (divineLineState != DIVINE_LINE_ON) {
    return;
  }

  guScale(&(dynamicp->divineLineScale), 0.32f, 0.32f, 1.f);
  guTranslate(&(dynamicp->divineLineTransform), divineLineStartSpot.x, divineLineStartSpot.y, divineLineStartSpot.z);

  gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(dynamicp->divineLineTransform)), G_MTX_MODELVIEW | G_MTX_PUSH);
  gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(dynamicp->divineLineScale)), G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH); 

  gSPDisplayList(glistp++, divine_line_commands);

  gSPPopMatrix(glistp++, G_MTX_MODELVIEW);
}

static Vtx horizon_geo[] = {
  {  SCREEN_WD, SCREEN_HT * 3,  5, 0, 8 << 6, 0 << 6, 0x04, 0x02, 0x01, 0x00 },
  {   0, SCREEN_HT * 3,         5, 0, 0 << 6, 0 << 6, 0x04, 0x02, 0x01, 0x00 },
  {   0,             SCREEN_HT / 2 - 6,         5, 0, 0 << 6, 8 << 6, 0x14, 0x34, 0x54, 0xff },
  {  SCREEN_WD,             SCREEN_HT / 2 - 10,  5, 0, 8 << 6, 8 << 6, 0x14, 0x34, 0x54, 0xff },

  {  SCREEN_WD / 3, SCREEN_HT / 2 + 2,  5, 0, 8 << 6, 0 << 6, 0x88, 0x88, 0x88, 0xff },
  {   0, SCREEN_HT / 2,         5, 0, 0 << 6, 0 << 6, 0x88, 0x88, 0x88, 0xff },
  {   0,             -SCREEN_HT / 2,         5, 0, 0 << 6, 8 << 6, 0x2a, 0x2a, 0x2a, 0xff },
  {  SCREEN_WD / 3,             -SCREEN_HT / 2,  5, 0, 8 << 6, 8 << 6, 0x28, 0x28, 0x28, 0xff },

  {  SCREEN_WD / 3 * 2, SCREEN_HT / 2 - 2,  5, 0, 8 << 6, 0 << 6, 0x88, 0x88, 0x88, 0xff },
  {  SCREEN_WD / 3, SCREEN_HT / 2 + 4,         5, 0, 0 << 6, 0 << 6, 0x88, 0x88, 0x88, 0xff },
  {  SCREEN_WD / 3,             -SCREEN_HT / 2,         5, 0, 0 << 6, 8 << 6, 0x2a, 0x2a, 0x2a, 0xff },
  {  SCREEN_WD / 3 * 2,             -SCREEN_HT / 2,  5, 0, 8 << 6, 8 << 6, 0x28, 0x28, 0x28, 0xff },

  {  SCREEN_WD, SCREEN_HT / 2,  5, 0, 8 << 6, 0 << 6, 0x88, 0x88, 0x88, 0xff },
  {  SCREEN_WD / 3 * 2, SCREEN_HT / 2 - 2,         5, 0, 0 << 6, 0 << 6, 0x88, 0x88, 0x88, 0xff },
  {  SCREEN_WD / 3 * 2,             -SCREEN_HT / 2,         5, 0, 0 << 6, 8 << 6, 0x2a, 0x2a, 0x2a, 0xff },
  {  SCREEN_WD,             -SCREEN_HT / 2,  5, 0, 8 << 6, 8 << 6, 0x28, 0x28, 0x28, 0xff },


  {  240, 162, 5, 0, 8 << 6, 0 << 6, 0xFF, 0xFF, 0xFF, 0x00 },
  {  239, 162, 5, 0, 0 << 6, 0 << 6, 0xFF, 0xFF, 0xFF, 0x00 },
  {  239, 161, 5, 0, 0 << 6, 8 << 6, 0xFF, 0xFF, 0xFF, 0xff },
  {  240, 161, 5, 0, 8 << 6, 8 << 6, 0xFF, 0xFF, 0xFF, 0xff },

  {  39, 151, 5, 0, 8 << 6, 0 << 6, 0xFF, 0xFF, 0xFF, 0x00 },
  {  37, 151, 5, 0, 0 << 6, 0 << 6, 0xFF, 0xFF, 0xFF, 0x00 },
  {  36, 150, 5, 0, 0 << 6, 8 << 6, 0xFF, 0xFF, 0xFF, 0xff },
  {  39, 150, 5, 0, 8 << 6, 8 << 6, 0xFF, 0xFF, 0xFF, 0xff },
};

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
  gDPSetCycleType(glistp++, G_CYC_1CYCLE);

  // hud camera
  guOrtho(&(dynamicp->orthoHudProjection), 0, SCREEN_WD - 1, 0, SCREEN_HT - 1, 0.f, 10.f, 1.f);
  guMtxIdent(&(dynamicp->orthoHudModelling));

  // The "skybox"
  {
    //guMtxIdent(&(dynamicp->horizonTranslation));
    guTranslate(&(dynamicp->horizonTranslation), 0.f, (cameraRotation.y / (M_PI * 0.35f)) * SCREEN_HT * 0.5f + (SCISSOR_HIGH), 0.f);

    gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->orthoHudProjection)), G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->horizonTranslation)), G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPClearGeometryMode(glistp++, 0xFFFFFFFF);
    gSPSetGeometryMode(glistp++, G_SHADE | G_SHADING_SMOOTH | G_CULL_BACK);
    gSPClipRatio(glistp++, FRUSTRATIO_6);
    gDPPipeSync(glistp++);
    gDPSetCombineLERP(glistp++, NOISE, 0, ENVIRONMENT, SHADE, 0, 0, 0, SHADE, NOISE, 0, ENVIRONMENT, SHADE, 0, 0, 0, SHADE);
    gSPVertex(glistp++, OS_K0_TO_PHYSICAL(horizon_geo), 24, 0);
    gSP2Triangles(glistp++, 0, 1, 2, 0, 0, 2, 3, 0);
    gSP2Triangles(glistp++, 4, 5, 6, 0, 4, 6, 7, 0);
    gSP2Triangles(glistp++, 8, 9, 10, 0, 8, 10, 11, 0);
    gSP2Triangles(glistp++, 12, 13, 14, 0, 12, 14, 15, 0);
    gSP2Triangles(glistp++, 16, 17, 18, 0, 16, 18, 19, 0);
    gSP2Triangles(glistp++, 20, 21, 22, 0, 20, 22, 23, 0);
    gSPClipRatio(glistp++, FRUSTRATIO_2);
  }

  // The camera
  {
    guPerspective(&dynamicp->projection, &perspNorm, CAMERA_BASE_FOV + (CAMERA_ZOOM_FOV_CHANGE * playerZoomFactor), (float)SCREEN_WD/(float)SCREEN_HT, 0.90f, 1000.0f, 1.0f);
    gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->projection)), G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    guLookAt(&dynamicp->camera, cameraPos.x * 100.f, cameraPos.y * 100.f, cameraPos.z * 100.f, cameraTarget.x * 100.f, cameraTarget.y * 100.f, cameraTarget.z * 100.f, 0, 0, 1);
    gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(dynamicp->camera)), G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    guLookAt(&dynamicp->cameraLocal, cameraPos.x, cameraPos.y, cameraPos.z, cameraTarget.x, cameraTarget.y, cameraTarget.z, 0, 0, 1);
    
  }

  if (!cinemaMode) {
    gDPSetScissor(glistp++, G_SC_NON_INTERLACE, SCISSOR_SIDES, SCISSOR_LOW, SCREEN_WD - SCISSOR_SIDES, SCREEN_HT - SCISSOR_HIGH);
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
            gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(dynamicp->cameraLocal)), G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
            renderKaijuCallback(dynamicp);
            gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(dynamicp->camera)), G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
          }

          if (dotProductFromCamera < 0.5f && (distanceToSectionSq > 16.f)) {
            continue;
          }

          if ((distanceToSectionSq < HIGH_DETAIL_CUTOFF_SQ)) {
            gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(sections[sectionIndex].commands));
          } else {
            gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(lowDetailSections[sectionIndex].commands));
          }

          if (((int)(divineLineEndSpot.x / 4.f) == x) && ((int)(divineLineEndSpot.y / 4.f) == y)) {
            gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(dynamicp->cameraLocal)), G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
            renderDivineLine(dynamicp);
            gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(dynamicp->camera)), G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
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
            gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(dynamicp->cameraLocal)), G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
            renderKaijuCallback(dynamicp);
            gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(dynamicp->camera)), G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
          }

          if (dotProductFromCamera < 0.5f && (distanceToSectionSq > 16.f)) {
            continue;
          }

          if ((distanceToSectionSq < HIGH_DETAIL_CUTOFF_SQ) || ((distanceToSectionSq < FOCUS_HIGH_DETAIL_CUTOFF_SQ) && (dotProductFromCamera > 0.8975f))) {
            gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(sections[sectionIndex].commands));
          } else {
            gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(lowDetailSections[sectionIndex].commands));
          }

          if (((int)(divineLineEndSpot.x / 4.f) == x) && ((int)(divineLineEndSpot.y / 4.f) == y)) {
            gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(dynamicp->cameraLocal)), G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
            renderDivineLine(dynamicp);
            gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(dynamicp->camera)), G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
          }
        }
      }
    }

    gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(mapSectionsPostamble));
  }

  gDPPipeSync(glistp++);
  gDPSetCombineLERP(glistp++, NOISE, 0, ENVIRONMENT, SHADE, 0, 0, 0, SHADE, NOISE, 0, ENVIRONMENT, SHADE, 0, 0, 0, SHADE);
  gDPSetRenderMode(glistp++, G_RM_ZB_OPA_SURF, G_RM_ZB_OPA_SURF2);
  gSPClearGeometryMode(glistp++, 0xFFFFFFFF);
  gSPSetGeometryMode(glistp++, G_SHADE | G_SHADING_SMOOTH | G_CULL_BACK | G_ZBUFFER);

  // The player
  if (zoomState != ZOOMED_IN) {
    const float st = sinf(time * 0.000005f) * 0.5f + 0.5f;
    gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(dynamicp->cameraLocal)), G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);

    gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(dynamicp->playerTranslation)), G_MTX_MODELVIEW | G_MTX_PUSH);

    guRotate(&(dynamicp->playerRotation), (playerDisplayRotation - (M_PI * 1.5f)) * RAD_TO_DEG, 0.f, 0.f, 1.f);
    gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(dynamicp->playerRotation)), G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);

    guTranslate(&(dynamicp->playerTranslation), playerPos.x, playerPos.y, playerPos.z);
    guScale(&(dynamicp->playerScale), protag_scale, protag_scale, protag_scale);
    gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&(dynamicp->playerScale)), G_MTX_MODELVIEW | G_MTX_NOPUSH);
    gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(protag_head_commands));
    gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(protag_body_commands));
    if (distanceSq(&playerVelocity, &zeroVector) > 0.01f) {
      for (i = 0; i < 56; i++) {
        dynamicp->playerLegVerts[i] = protag_legs_verts[i];
        dynamicp->playerLegVerts[i].v.ob[0] = (short)(lerp(protag_legs_move_a[i].v.ob[0], protag_legs_move_b[i].v.ob[0], st));
        dynamicp->playerLegVerts[i].v.ob[1] = (short)(lerp(protag_legs_move_a[i].v.ob[1], protag_legs_move_b[i].v.ob[1], st));
        dynamicp->playerLegVerts[i].v.ob[2] = (short)(lerp(protag_legs_move_a[i].v.ob[2], protag_legs_move_b[i].v.ob[2], st));
      }
        gSPVertex(glistp++, OS_K0_TO_PHYSICAL(dynamicp->playerLegVerts), 56, 0);
    } else {
      gSPVertex(glistp++, OS_K0_TO_PHYSICAL(protag_legs_verts), 56, 0);
    }
    gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(protag_legs_commands));
    gSPPopMatrix(glistp++, G_MTX_MODELVIEW);
  }

  // HUD
  if (!cinemaMode)
  {
    gDPPipeSync(glistp++);
    gDPSetCombineMode(glistp++, G_CC_SHADE, G_CC_SHADE);
    gDPSetScissor(glistp++, G_SC_NON_INTERLACE, 0, 0, SCREEN_WD - 1, SCREEN_HT - 1);
    gDPSetRenderMode(glistp++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gSPClearGeometryMode(glistp++,0xFFFFFFFF);
    gSPSetGeometryMode(glistp++, G_SHADE | G_SHADING_SMOOTH | G_CULL_BACK);

    gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->orthoHudProjection)), G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->orthoHudModelling)), G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);

    // Hud background
    gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(hud_dl));
    if (zoomState == ZOOMED_IN) {
      gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(hud_zoomed_in_commands));
    }

    guTranslate(&(dynamicp->laserBarTranslation), SCISSOR_SIDES, SCISSOR_HIGH - 10, 0.f);
    if (laserChargeFactor < 0.9f) {
      guScale(&(dynamicp->laserBarScale), cubic(laserChargeFactor / 0.9f), 1.f, 1.f);
    } else {
      guScale(&(dynamicp->laserBarScale), 1.f, 1.f, 1.f);
    }
    gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->laserBarTranslation)), G_MTX_MODELVIEW | G_MTX_PUSH);
    gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->laserBarScale)), G_MTX_MODELVIEW | G_MTX_NOPUSH);
    gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(laser_charge_bar_commands));
    if (laserChargeFactor > 0.9f) {
      gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(laser_charge_almost_max));
    }
    gSPPopMatrix(glistp++, G_MTX_MODELVIEW);

    {
      int it = 0;
      gSPVertex(glistp++, OS_K0_TO_PHYSICAL(hud_remaining_targets), (9 * 4), 0);
      for (i = 0; i < NUMBER_OF_KAIJU_HITBOXES; i++) {
        if (hitboxes[i].destroyable && hitboxes[i].alive) {
          gSP2Triangles(glistp++, it + 0, it + 1, it + 2, 0, it + 0, it + 3, it + 1, 0);
          it += 4;
        }
      }
        
    }


    gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(portrait_commands));
    gDPLoadTextureTile(glistp++, portrait_bin, G_IM_FMT_RGBA, G_IM_SIZ_16b, 48, 48, 0, 0, 48 - 1, 42 - 1, 0, G_TX_NOMIRROR, G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD );
    gSPTextureRectangle(glistp++, (SCREEN_WD - SCISSOR_SIDES - 48 + 4) << 2, (SCREEN_HT - SCISSOR_HIGH + 3) << 2, (SCREEN_WD - SCISSOR_SIDES - 4) << 2, (SCREEN_HT - SCISSOR_HIGH + 42 - 4) << 2, 0, 4 << 5, (3 + (int)(2 * MAX(-1.f, MIN(1.f, playerVelocity.z * 0.3333f)))) << 5, 1 << 10, 1 << 10);

    // Show the remaining time in seconds
    {
      gSPDisplayList(glistp++, OS_K0_TO_PHYSICAL(time_prefix_commands));
      gSPTextureRectangle(glistp++, (211 +  0 + 4) << 2, (200) << 2, (211 +  8 + 4) << 2, (200 + 8) << 2, 0, s2 << 5, t2 << 5, 1 << 10, 1 << 10);
      gSPTextureRectangle(glistp++, (211 +  8 + 4) << 2, (200) << 2, (211 + 16 + 4) << 2, (200 + 8) << 2, 0, s0 << 5, t0 << 5, 1 << 10, 1 << 10);
      gSPTextureRectangle(glistp++, (211 + 16 + 4) << 2, (200) << 2, (211 + 24 + 4) << 2, (200 + 8) << 2, 0, s1 << 5, t1 << 5, 1 << 10, 1 << 10);
    }
  }

  gDPFullSync(glistp++);
  gSPEndDisplayList(glistp++);

  // assert((glistp - gfx_glist[gfx_gtask_no]) < GFX_GLIST_LEN);

  /* Activate the task and 
     switch display buffers */
  nuGfxTaskStart(&gfx_glist[gfx_gtask_no][0],
		 (s32)(glistp - gfx_glist[gfx_gtask_no]) * sizeof (Gfx),
		 NU_GFX_UCODE_F3DLP_REJ , NU_SC_SWAPBUFFER);

  
  // if(contPattern & 0x1)
  //   {
  //     nuDebConTextPos(0,4,4);
  //     sprintf(conbuf, "DL: %3d/%3d", (glistp - gfx_glist[gfx_gtask_no]), GFX_GLIST_LEN );
  //     nuDebConCPuts(0, conbuf);


  //     nuDebConTextPos(0,4,5);
  //     sprintf(conbuf, "time left: %3.2f", timeRemaining);
  //     nuDebConCPuts(0, conbuf);

  //   }
  // else
  //   {
  //     nuDebConTextPos(0,4,4);
  //     nuDebConCPuts(0, "Connect controller #1, kid!");
  //   }

    // nuDebTaskPerfBar1(2, 200, NU_SC_NOSWAPBUFFER);
    
  // nuDebConDisp(NU_SC_SWAPBUFFER);

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
  float walkDirectionX = 0.f;
  float walkDirectionY = 0.f;
  float inputDirectionX = MAX(-65, MIN(65, contdata->stick_x)) / 255.f;
  float inputDirectionY = MAX(-65, MIN(65, contdata->stick_y)) / 255.f;
  float inputDirectionXRotated;
  float inputDirectionYRotated;
  float cosCameraRot, sinCameraRot;

  if (fabs_d(inputDirectionX) < 0.025f) {
    inputDirectionX = 0.f;
  }
  if (fabs_d(inputDirectionY) < 0.025f) {
    inputDirectionY = 0.f;
  }

  if (zoomState == ZOOMED_IN && contdata->button & Z_TRIG) {
    walkDirectionX = 0;
    walkDirectionY = 0;
  }

  if (contdata[0].button & L_JPAD) {
    walkDirectionX = -1.f;
  } else if (contdata[0].button & R_JPAD) {
    walkDirectionX = 1.f;
  }

  if (contdata[0].button & D_JPAD) {
    walkDirectionY = -1.f;
  } else if (contdata[0].button & U_JPAD) {
    walkDirectionY = 1.f;
  }

  cameraRotation.z -= inputDirectionX * (CAMERA_TURN_SPEED_X + (CAMERA_TURN_SPEED_ADJUSTMENT_WHILE_ZOOMED * playerZoomFactor)) * deltaSeconds;
  cameraRotation.y = MAX(-M_PI * 0.35f, MIN(M_PI * 0.35f, cameraRotation.y + ((CAMERA_TURN_SPEED_Y + (CAMERA_TURN_SPEED_ADJUSTMENT_WHILE_ZOOMED * playerZoomFactor)) * deltaSeconds * cameraYInvert * inputDirectionY)));

  cosCameraRot = cosf(cameraRotation.z + M_PI * 0.5f);
  sinCameraRot = sinf(cameraRotation.z + M_PI * 0.5f);
  inputDirectionXRotated = (cosCameraRot * walkDirectionX) - (sinCameraRot * walkDirectionY);
  inputDirectionYRotated = (sinCameraRot * walkDirectionX) + (cosCameraRot * walkDirectionY);

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
    nuAuSndPlayerPlay(SOUND_JUMP1);
  }

  stepX = playerPos.x + (playerVelocity.x * deltaSeconds);
  stepY = playerPos.y + (playerVelocity.y * deltaSeconds);
  stepZ = playerPos.z + (playerVelocity.z * deltaSeconds);

  stepX = clamp(stepX, 1.f, 127.f);
  stepY = clamp(stepY, 1.f, 127.f);

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

  cameraTarget.x =/* lerp( cameraTarget.x, */playerPos.x;/*, 0.12f);*/
  cameraTarget.y =/* lerp( cameraTarget.y, */playerPos.y;/*, 0.12f);*/
  cameraTarget.z =/* lerp( cameraTarget.z, */playerPos.z + CAMERA_LIFT_FRONT;/*, 0.12f);*/
  cameraPos.x = cameraTarget.x + (cosf(cameraRotation.z) * (CAMERA_DISTANCE) * cosf(cameraRotation.y));
  cameraPos.y = cameraTarget.y + (sinf(cameraRotation.z) * (CAMERA_DISTANCE) * cosf(cameraRotation.y));
  cameraPos.z = cameraTarget.z + (CAMERA_DISTANCE * sinf(cameraRotation.y)) + CAMERA_LIFT_BACK;

  if ((zoomState == NOT_ZOOMED_IN) && ((contdata->trigger & L_TRIG))) {
    zoomState = ZOOMED_IN;
    nuAuSndPlayerPlay(SOUND_ZOOM_IN);
  }
  if ((zoomState == ZOOMED_IN) && (!(contdata->button & L_TRIG))) {
    zoomState = NOT_ZOOMED_IN;
    nuAuSndPlayerPlay(SOUND_ZOOM_OUT);
  }

  playerZoomFactor = clamp(playerZoomFactor + (ZOOM_IN_OUT_SPEED * deltaSeconds * zoomState), 0.f, 1.f);

  if (laserChargeFactor < 0.01f && (zoomState == ZOOMED_IN) && ((contdata->trigger & Z_TRIG) || contdata->trigger & R_TRIG)) {
    laserChargeFactor = 0.01f;
  }

  if ((laserChargeFactor >= 0.01f) && ((contdata->button & Z_TRIG) || contdata->button & R_TRIG) && (zoomState == ZOOMED_IN)) {
    laserChargeFactor = clamp(laserChargeFactor + (LASER_CHARGE_SPEED * deltaSeconds * 1), 0.f, 1.f);

    laserChargeDelta += deltaSeconds;
    if ((laserChargeFactor < 0.95f) && (laserChargeDelta > TIME_PER_CHARGE_BIP)) {
      ALSndId chargeSoundId = nuAuSndPlayerPlay(SOUND_LASER_CHARGE);
      if (chargeSoundId != -1) {
        nuAuSndPlayerSetSound(chargeSoundId);
        nuAuSndPlayerSetPitch(1.f + laserChargeFactor);
      }

      laserChargeDelta = 0.f;

    }
  } else {
    laserChargeFactor = clamp(laserChargeFactor + (LASER_CHARGE_SPEED * deltaSeconds * -1), 0.f, 1.f);
  }
}

void updateCinemaMode(float deltaSeconds) {
  if ((!ending )&& (cinemaTime > (CINEMA_DURATION - 1.f))) {
    const float t = cinemaTime - (CINEMA_DURATION - 1.f);

    cameraTarget.x = lerp( hitboxes[0].position.x, playerPos.x, t);
    cameraTarget.y = lerp( hitboxes[0].position.y, playerPos.y, t);
    cameraTarget.z = lerp( hitboxes[0].position.z, playerPos.z + CAMERA_LIFT_FRONT, t);
    cameraPos.x = lerp(hitboxes[0].position.x + 20, cameraTarget.x + (cosf(cameraRotation.z) * (CAMERA_DISTANCE) * cosf(cameraRotation.y)), t);
    cameraPos.y = lerp(hitboxes[0].position.y + 20, cameraTarget.y + (sinf(cameraRotation.z) * (CAMERA_DISTANCE) * cosf(cameraRotation.y)), t);
    cameraPos.z = lerp(hitboxes[0].position.z + 10, cameraTarget.z + (CAMERA_DISTANCE * sinf(cameraRotation.y)) + CAMERA_LIFT_BACK, t);
  } else {
    cameraTarget.x = hitboxes[0].position.x;
    cameraTarget.y = hitboxes[0].position.y;
    cameraTarget.z = hitboxes[0].position.z;

    cameraPos.x = hitboxes[0].position.x + 20;
    cameraPos.y = hitboxes[0].position.y + 20;
    cameraPos.z = MAX(20, hitboxes[0].position.z + 10);
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
  int roll = ((int)timeRemaining) % 3;
  laserChargeFactor = 0.f;

  divineLineStartSpot = (vec3){ location->x, location->y, DIVINE_LINE_START_HEIGHT };
  divineLineEndSpot = (vec3){ location->x, location->y, location->z };

  divineLineState = DIVINE_LINE_ON;
  divineLineTimePassed = 0.f;

  if (roll == 0) {
    nuAuSndPlayerPlay(SOUND_LASER3);
  } else if (roll == 1) {
    nuAuSndPlayerPlay(SOUND_LASER2);
  }else {
    nuAuSndPlayerPlay(SOUND_LASER1);
  }
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
        noiseFactor = 0.70f;
        nuAuSndPlayerPlay(SOUND_EXPLOSION1);
      }
      break;
    }

    nextStepDistance = closestDistance + 0.001f;
  }

  if (!hitAnything) {
    laserChargeFactor = 0.f;
    noiseFactor = 0.f;
    nuAuSndPlayerPlay(SOUND_NO_TARGET);
  }
}

void updateHitboxCheck() {
  if ((zoomState == ZOOMED_IN) && (((contdata->button & Z_TRIG) || contdata->button & R_TRIG)) && (laserChargeFactor > 0.98f)) {
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

void checkIfPlayerHasWonOrLost(float deltaSeconds) {
  if (cinemaMode) {
    cinemaTime += deltaSeconds;

    if (ending && sinking) {
      explosionTime += deltaSeconds;
      if (explosionTime > 0.815515f) {
        explosionTime = 0.f;
        nuAuSndPlayerPlay(SOUND_EXPLOSION1 + (guRandom() % 3));
      }
    }

    if (cinemaTime <  (ending ? CINEMA_LONG_DURATION : CINEMA_DURATION)) {
      return;
    }

    if (ending == 1) {
      nuAuSeqPlayerStop(0);
      changeScreensFlag = 1;
      return;
    } else {
      cinemaTime = 0.f;
      cinemaMode = 0;
      explosionTime = 0.f;
    }
  }

  // Check if we broke all the enemy hitboxes
  {
    int i;
    int defeatedAllHitboxes = 1;

    for (i = 0; i < NUMBER_OF_KAIJU_HITBOXES; i++) {
      if (hitboxes[i].alive && hitboxes[i].destroyable) {
        defeatedAllHitboxes = 0;
      }
    }

    if (defeatedAllHitboxes && (!cinemaMode)) {
      screenType = DialogueScreen;
      ending = 1;
      sinking = 1;
      finishedLevel = 1;
      cinemaMode = 1;
      cinemaTime = 0.f;
      playerZoomFactor = 0.f;
      nuAuSndPlayerPlay(SOUND_SUCCESS);
    }

    if (currentLevel == 0) {
      sinking = 0;
    }
  }

  // Check if we ran out of time
  {
    u32 timeLeft;
    timeRemaining -= deltaSeconds;
    if (timeRemaining < 0.f) {
      screenType = RetryScreen;
      ending = 1;
      cinemaMode = 1;
      explosionTime = -9999.f;
      cinemaTime = 0.f;
      playerZoomFactor = 0.f;
      nuAuSndPlayerPlay(SOUND_TIME_OVER);
    }

    timeLeft = clamp(timeRemaining, 0, 255);
    hundredsDigit = timeLeft / 100;
    majorDigit = (timeLeft / 10) % 10;
    minorDigit = timeLeft % 10;
    s0 = (majorDigit % 4) * 8;
    t0 = ((majorDigit / 4) * 8) + 8;
    s1 = (minorDigit % 4) * 8;
    t1 = ((minorDigit / 4) * 8) + 8;
    s2 = (hundredsDigit % 4) * 8;
    t2 = ((hundredsDigit / 4) * 8) + 8;
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

  //nudebperfMarkSet(0);
  if (!sinking) {
    updateKaijuCallback(deltaSeconds);
  } else if (ending) {
    hitboxes[0].position.z -= deltaSeconds * 4.1f;
    hitboxes[0].isTransformDirty = 1;
  }

  //nudebperfMarkSet(1);
  updateKaijuHitboxes(deltaSeconds);
  //nudebperfMarkSet(2);
  if (!cinemaMode) {
    updatePlayer(deltaSeconds);
  } else {
    updateCinemaMode(deltaSeconds);
  }
  
  //nudebperfMarkSet(3);
  updateHitboxCheck();
  //nudebperfMarkSet(4);
  updateDivineLine(deltaSeconds);
  //nudebperfMarkSet(5);
  checkIfPlayerHasWonOrLost(deltaSeconds);

}
