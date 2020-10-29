
#ifndef _GAME_MATH_H_
#define _GAME_MATH_H_

#define RAD_TO_DEG 57.2958f

/* copy-paste from the nusnake example */
#define TOL ((float)1.0E-7)    /* Fix precision to 10^-7 because of the float type  */
#define M_PI_2    1.57079632679489661923
#define M_PI_4    0.78539816339744830962
#define M_RTOD    (180.0/3.14159265358979323846)

float nu_atan2(float y, float x);

typedef struct {
  float x;
  float y;
} vec2;

typedef struct {
  float x;
  float y;
  float z;
} vec3;

extern const vec3 zeroVector;

typedef struct {
	float data[4][4];
} mat4;

void mat4x4_invert(mat4* T, const mat4* M);

float fabs_d(float x);

float Q_rsqrt( float number );

float lerp(float v0, float v1, float t);
float catmullRom(float t, float p0, float p1, float p2, float p3);

float cubic(float x);

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