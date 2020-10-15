
#include <nusys.h>

#include "gamemath.h"

const vec3 zeroVector = { 0.f, 0.f, 0.f };

// Copied and adapted from exezin's exengine:
// Hoisted the variables and changed the row-column ordering
// https://github.com/exezin/exengine/blob/master/src/exengine/math/mathlib.h
void mat4x4_invert(mat4* T, const mat4* M)
{
  float s[6];
  float c[6];
  float idet;

  s[0] = M->data[0][0]*M->data[1][1] - M->data[0][1]*M->data[1][0];
  s[1] = M->data[0][0]*M->data[2][1] - M->data[0][1]*M->data[2][0];
  s[2] = M->data[0][0]*M->data[3][1] - M->data[0][1]*M->data[3][0];
  s[3] = M->data[1][0]*M->data[2][1] - M->data[1][1]*M->data[2][0];
  s[4] = M->data[1][0]*M->data[3][1] - M->data[1][1]*M->data[3][0];
  s[5] = M->data[2][0]*M->data[3][1] - M->data[2][1]*M->data[3][0];

  c[0] = M->data[0][2]*M->data[1][3] - M->data[0][3]*M->data[1][2];
  c[1] = M->data[0][2]*M->data[2][3] - M->data[0][3]*M->data[2][2];
  c[2] = M->data[0][2]*M->data[3][3] - M->data[0][3]*M->data[3][2];
  c[3] = M->data[1][2]*M->data[2][3] - M->data[1][3]*M->data[2][2];
  c[4] = M->data[1][2]*M->data[3][3] - M->data[1][3]*M->data[3][2];
  c[5] = M->data[2][2]*M->data[3][3] - M->data[2][3]*M->data[3][2];
  
  /* Assumes it is invertible */
  idet = 1.0f/( s[0]*c[5]-s[1]*c[4]+s[2]*c[3]+s[3]*c[2]-s[4]*c[1]+s[5]*c[0] );
  
  T->data[0][0] = ( M->data[1][1] * c[5] - M->data[2][1] * c[4] + M->data[3][1] * c[3]) * idet;
  T->data[1][0] = (-M->data[1][0] * c[5] + M->data[2][0] * c[4] - M->data[3][0] * c[3]) * idet;
  T->data[2][0] = ( M->data[1][3] * s[5] - M->data[2][3] * s[4] + M->data[3][3] * s[3]) * idet;
  T->data[3][0] = (-M->data[1][2] * s[5] + M->data[2][2] * s[4] - M->data[3][2] * s[3]) * idet;
  
  T->data[0][1] = (-M->data[0][1] * c[5] + M->data[2][1] * c[2] - M->data[3][1] * c[1]) * idet;
  T->data[1][1] = ( M->data[0][0] * c[5] - M->data[2][0] * c[2] + M->data[3][0] * c[1]) * idet;
  T->data[2][1] = (-M->data[0][3] * s[5] + M->data[2][3] * s[2] - M->data[3][3] * s[1]) * idet;
  T->data[3][1] = ( M->data[0][2] * s[5] - M->data[2][2] * s[2] + M->data[3][2] * s[1]) * idet;
  
  T->data[0][2] = ( M->data[0][1] * c[4] - M->data[1][1] * c[2] + M->data[3][1] * c[0]) * idet;
  T->data[1][2] = (-M->data[0][0] * c[4] + M->data[1][0] * c[2] - M->data[3][0] * c[0]) * idet;
  T->data[2][2] = ( M->data[0][3] * s[4] - M->data[1][3] * s[2] + M->data[3][3] * s[0]) * idet;
  T->data[3][2] = (-M->data[0][2] * s[4] + M->data[1][2] * s[2] - M->data[3][2] * s[0]) * idet;
  
  T->data[0][3] = (-M->data[0][1] * c[3] + M->data[1][1] * c[1] - M->data[2][1] * c[0]) * idet;
  T->data[1][3] = ( M->data[0][0] * c[3] - M->data[1][0] * c[1] + M->data[2][0] * c[0]) * idet;
  T->data[2][3] = (-M->data[0][3] * s[3] + M->data[1][3] * s[1] - M->data[2][3] * s[0]) * idet;
  T->data[3][3] = ( M->data[0][2] * s[3] - M->data[1][2] * s[1] + M->data[2][2] * s[0]) * idet;
}

float fabs_d(float x) {
  if (x < 0.f) {
    return -x;
  }
  return x;
}

float lerp(float v0, float v1, float t) {
  return (1 - t) * v0 + t * v1;
}

float bilinear(float v00, float v10, float v11, float v01, float x, float y) {
  const float invX = (1.f - x);
  const float invY = (1.f - y);

  return (invX * invY * v00)
       + (x * invY * v10)
       + (x * y * v11)
       + (invX * y * v01);
}

float clamp(float value, float min, float max) {
	if (value < min) {
		return min;
	} else if (value > max) {
		return max;
	}

	return value;
}

float distanceSq(const vec3* a, const vec3* b) {
	const float deltaX = (a->x - b->x);
	const float deltaY = (a->y - b->y);
	const float deltaZ = (a->z - b->z);

	return (deltaX * deltaX) + (deltaY * deltaY) + (deltaZ * deltaZ);
}

float length(const vec3* a) {
	const float dSq = distanceSq(a, &zeroVector);

	return sqrtf(dSq);
}

// Quake III Fast-inverse square root
// Copied from Wikipedia: https://en.wikipedia.org/wiki/Fast_inverse_square_root#Overview_of_the_code
float Q_rsqrt( float number )
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y  = number;
	i  = * ( long * ) &y;                       
	i  = 0x5f3759df - ( i >> 1 );               
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );   

	return y;
}

float dotProduct(const vec3* a, const vec3* b) {
	const float lengthASq = distanceSq(a, &zeroVector);
	const float invLengthA = Q_rsqrt(lengthASq);
	const vec3 normalizedA = { a->x * invLengthA, a->y * invLengthA, a->z * invLengthA };

	const float lengthBSq = distanceSq(b, &zeroVector);
	const float invLengthB = Q_rsqrt(lengthBSq);
	const vec3 normalizedB = { b->x * invLengthB, b->y * invLengthB, b->z * invLengthB };

	//return fabs_d(a->x * invLengthA) + fabs_d(a->y * invLengthA) + fabs_d(a->z * invLengthA);
	return (normalizedA.x * normalizedB.x) + (normalizedA.y * normalizedB.y) + (normalizedA.z * normalizedB.z);
}

float sdSphere(const vec3* p) {
  return length(p) - 1.f;
}

float sdOctahedron(const vec3* point) {
  const float s = 1.f;
  const vec3 p = { fabs_d(point->x), fabs_d(point->y), fabs_d(point->z) };
  float k;
  vec3 q;
  vec3 qp;
  
  float m = p.x + p.y + p.z - s;
  if ( (3.0f * p.x) < m ) {
  	q = (vec3){ p.x, p.y, p.z };
  } else if ( (3.0f * p.y) < m ) {
  	q = (vec3){ p.y, p.z, p.x };
  } else if ( (3.0f * p.z) < m ) {
  	q = (vec3){ p.z, p.x, p.y };
  }
  else {
  	return m * 0.57735027f;
  }
    
  k = clamp(0.5f * (q.z - q.y + s), 0.0f, s); 
  qp = (vec3){ q.x, q.y - s + k, q.z - k };

  return length(&qp); 
}

float sdBox(const vec3* p) {
  const vec3 q = { MAX(0.f, fabs_d(p->x) - 1.f), MAX(0.f, fabs_d(p->y) - 1.f), MAX(0.f, fabs_d(p->z) - 1.f) };
  const float longest = MIN( MAX( q.x, MAX( q.y, q.z ) ), 0.f);

  return length(&q) + longest;
}

