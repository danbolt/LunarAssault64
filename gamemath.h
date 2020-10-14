
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

float fabs_d(float x);

float lerp(float v0, float v1, float t);

float clamp(float value, float min, float max);

float distanceSq(const vec3* a, const vec3* b);

float dotProduct(const vec3* a, const vec3* b);

// distance functions copied from 
// https://iquilezles.org/www/articles/distfunctions/distfunctions.htm
float sdSphere(const vec3* p);
float sdOctahedron(const vec3* p);

#endif