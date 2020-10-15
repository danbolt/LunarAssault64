
#ifndef _GAME_MATH_H_
#define _GAME_MATH_H_

#define RAD_TO_DEG 57.2958f

typedef struct {
  float x;
  float y;
  float z;
} vec3;

typedef struct {
	float data[4][4];
} mat4;

void mat4x4_invert(mat4* T, const mat4* M);

float fabs_d(float x);

float lerp(float v0, float v1, float t);

// Copied from:
// https://math.stackexchange.com/questions/3230376/interpolate-between-4-points-on-a-2d-plane
float bilinear(float v00, float v10, float v11, float v01, float x, float y);

float clamp(float value, float min, float max);

float distanceSq(const vec3* a, const vec3* b);

float dotProduct(const vec3* a, const vec3* b);

// distance functions copied from 
// https://iquilezles.org/www/articles/distfunctions/distfunctions.htm
float sdSphere(const vec3* p);
float sdOctahedron(const vec3* p);
float sdBox(const vec3* p);

#endif