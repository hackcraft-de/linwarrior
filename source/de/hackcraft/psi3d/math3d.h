/**
 * File:   math3d.h
 * Author: Benjamin Pickhardt
 * Home:   hackcraft.de
 *
 * Created on Dez 23, 1999, 3:33 PM
 */

#ifndef _MATH3D_H
#define _MATH3D_H

#include <math.h>

//--[Include Files]----------------->
/*
#include <math.h>
#include <stdio.h>
#include <string.h>
*/

#define PI_OVER_180 0.017453292

#ifndef finitef
#define finitef(x) (!(x != x))
#endif

// Note sqrtf may not be supported on some plattforms
// and it is a undefined builtin function =>
// ifdef doesn't work at all.
// I'm not sure sqrtf is necessary at all?
// Maybe the compiler already optimises this itself
// if only floats are involved?
//#define sqrtf(x) ((float)sqrt((float)(x)))


//--[Machine Specific]-------------->

/*
typedef  float           FLOAT;

typedef  long            INTEG;

typedef  unsigned char   BOOLEAN;

typedef  unsigned long   FLAGS;

typedef  unsigned char   BYTE;

typedef  unsigned short  WORD;

typedef  unsigned long   DWORD;
*/

//--[Non Machine Specific]---------->

/*
typedef  FLOAT           ANGLE;

typedef  FLOAT           SCALAR;

typedef  SCALAR          VECTOR[4]; // homogenous/plane/rgba

typedef  SCALAR          MATRIX[4*4];
*/

//typedef  float          MATRIX[4*4];

typedef float vec2[3];
typedef float vec3[3];
typedef float vec4[4];
typedef float mat4[16];

typedef vec4 rgba;
typedef vec4 quat;

//--[Misc Defines]------------------>


#define ppercent(zero_pc, hundred_pc, p_pc)  (zero_pc + ((hundred_pc)-(zero_pc))*(p_pc)/100 )

#define inside(min, v, max)    ( ( (min) <= (v) ) && ( (v) <= (max) ) )

#define setflag(a, b)        (a) = ( FLAGS((a)) | FLAGS((b)) )

#define resetflag(a, b)      (a) = ( FLAGS((a)) & ( ~ FLAGS((b)) ) )


//--[Numeric Defines]------------------>

// use pow(x,2) <= compiler optimisations!
//#define xsqr(x)              ( (x) * (x) )

// use copysign(1.0, v)
//#define xsgn(v)              (((v)<0)?(-1):(((v)>0)?(+1):(0)))

// use fabs
//#define xabs(v)              (((v)<0)?-(v):(v))

// use fmod
//#define xmod(a, b)           ((a) -= long((b)) * (long( (a) ) / long( (b) )))

// use fmax, fmaxf
//#define xmax(v1,v2)          (((v1)>(v2))?(v1):(v2))

// use fmin, fminf
//#define xmin(v1,v2)          (((v1)<(v2))?(v1):(v2))

//#define xminmax(v, min, max)   ( xmin(xmax(v, max), min) )

//--[Parametric Spline Defines]--->


// -1.0 <= u <= 2.0

#define spline_b1(u)    ((u) * ((u) - 1) * ((u) - 2) / (-6.0f))

#define spline_b2(u)    (((u) + 1) * ((u) - 1) * ((u) - 2) / (2.0f))

#define spline_b3(u)    (((u) + 1) * (u) * ((u) - 2) / (-2.0f))

#define spline_b4(u)    (((u) + 1) * (u) * ((u) - 1) / (6.0f))

#define spline_func(a, b, c, d, u) ((a) * spline_b1((u)) + (b) * spline_b2((u)) + (c) * spline_b3((u)) + (d) * spline_b4((u)))




// Plane
//

// n = normal of plane;
// p = point of line/ray;
// v = slope of line/ray
// d = "D" from the plane equation

#define calcT(n, p, v, d)            (-(n[0]*p[0]+n[1]*p[1]+n[2]*p[2]+d)/(n[0]*v[0]+n[1]*v[1]+n[2]*v[2]))

#define calcP(nrml, pnt, d)          (nrml[0]*pnt[0]+nrml[1]*pnt[1]+nrml[2]*pnt[2]+d)

#define calcA(nrml, pnt, d)          (-(nrml[1]*pnt[1]+nrml[2]*pnt[2]+d)/pnt[0])

#define calcB(nrml, pnt, d)          (-(nrml[0]*pnt[0]+nrml[2]*pnt[2]+d)/pnt[1])

#define calcC(nrml, pnt, d)          (-(nrml[0]*pnt[0]+nrml[1]*pnt[1]+d)/pnt[2])

#define calcD(nrml, pnt)             (-(nrml[0]*pnt[0]+nrml[1]*pnt[1]+nrml[2]*pnt[2]))

#define calcX(nrml, pnt, d)          (-(nrml[1]*pnt[1]+nrml[2]*pnt[2]+d)/nrml[0])

#define calcY(nrml, pnt, d)          (-(nrml[0]*pnt[0]+nrml[2]*pnt[2]+d)/nrml[1])

#define calcZ(nrml, pnt, d)          (-(nrml[0]*pnt[0]+nrml[1]*pnt[1]+d)/nrml[2])

#define clip(a1, b1, a2, b2, clp)    ( (a1) - ( (a1) - (a2) ) / ( (b1) - (b2) ) * ( (b1) - (clp) ) )

#define clif(b1, b2, f)              ( (b1) - ( (b1) - (b2) ) * (f) )

#define clic(b1, b2, f, d)           ( (b1) - ( (b1) - (b2) ) * (f) / (d) )

// Intersection of a Line (p0,p1) with the Plane (normal,d) result point exists if result is non-zero.
// plane_lineintersect(int result, VECTOR normal, float d, VECTOR p0, VECTOR p1, VECTOR resultp)
#define plane_lineintersect(result, normal, d, p0, p1, resultp) \
{ \
  (result) = 0 \
  float d0 = calcP((normal), (p0), (d)); \
  float d1 = calcP((normal), (p1), (d)); \
  if ( copysign(1.0, d0) != copysign(1.0, d1) ) { \
    float dd = d0 / (d0 - d1); \
    (resultp)[0] = (p0)[0] - (((p0)[0] - (p1)[0]) * dd); \
    (resultp)[1] = (p0)[1] - (((p0)[1] - (p1)[1]) * dd); \
    (resultp)[2] = (p0)[2] - (((p0)[2] - (p1)[2]) * dd); \
    result = 1; \
  } \
}

// Normal from three Points (VECTOR normal, VECTOR p0, VECTOR p1, VECTOR p2)
#define normal_threepnt(result, p0, p1, p2) \
{ \
    float v0[3]; \
    float v1[3]; \
    vector_sub(v0, (p0), (p1)); \
    vector_sub(v1, (p1), (p2)); \
    vector_cross((result), v0, v1); \
}


// Matrix: float m[16]
//

// Prints an Matrix of gl-Format (as used here)
#define matrix_print(glMatrix) \
{ \
    printf("/ %3.2f %3.2f %3.2f %3.2f \\\n", (glMatrix)[0], (glMatrix)[4], (glMatrix)[ 8], (glMatrix)[12]); \
    printf("| %3.2f %3.2f %3.2f %3.2f |\n",  (glMatrix)[1], (glMatrix)[5], (glMatrix)[ 9], (glMatrix)[13]); \
    printf("| %3.2f %3.2f %3.2f %3.2f |\n",  (glMatrix)[2], (glMatrix)[6], (glMatrix)[10], (glMatrix)[14]); \
    printf("\\ %3.2f %3.2f %3.2f %3.2f /\n", (glMatrix)[3], (glMatrix)[7], (glMatrix)[11], (glMatrix)[15]); \
}

// Transposes a 4x4 Matrix - useful to invert ortho-normal-bases.
#define matrix_transpose(m) { float tmp; loopi(4) { loopj(i) { tmp = m[4*i+j]; m[4*i+j] = m[4*j+i]; m[4*j+i] = tmp; } } }

// Set Matrix Values (MATRIX result, float a1, float ...)
#define matrix_set(result,  a1, a2, a3, a4,  b1, b2, b3, b4,  c1, c2, c3, c4,  d1, d2, d3, d4) \
{ \
  (result)[ 0]=(a1); (result)[ 1]=(a2); (result)[ 2]=(a3); (result)[ 3]=(a4); \
  (result)[ 4]=(b1); (result)[ 5]=(b2); (result)[ 6]=(b3); (result)[ 7]=(b4); \
  (result)[ 8]=(c1); (result)[ 9]=(c2); (result)[10]=(c3); (result)[11]=(c4); \
  (result)[12]=(d1); (result)[13]=(d2); (result)[14]=(d3); (result)[15]=(d4); \
}

// Set Identity Matrix (MATRIX result)
#define matrix_identity(result) \
{ \
  matrix_set((result), 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1); \
}

// Apply Matrix to Vector
// applyMatrix2(MATRIX M, VECTOR V)
#define matrix_apply2(M, V) \
{ \
    float pre[] = { (V)[0], (V)[1], (V)[2] }; \
    (V)[0] = (M)[0]*(pre)[0] + (M)[4]*(pre)[1] + (M)[ 8]*(pre)[2] + (M)[12]; \
    (V)[1] = (M)[1]*(pre)[0] + (M)[5]*(pre)[1] + (M)[ 9]*(pre)[2] + (M)[13]; \
    (V)[2] = (M)[2]*(pre)[0] + (M)[6]*(pre)[1] + (M)[10]*(pre)[2] + (M)[14]; \
}

// Apply Matrix to Vector and stores to result
// applyMatrix3(VECTOR result, MATRIX M, VECTOR V)
#define matrix_apply3(result, M, V) \
{ \
  (result)[0] = ((V)[0]*(M)[0]) + ((V)[1]*(M)[4]) + ((V)[2]*(M)[ 8]) + ((M)[12]); \
  (result)[1] = ((V)[0]*(M)[1]) + ((V)[1]*(M)[5]) + ((V)[2]*(M)[ 9]) + ((M)[13]); \
  (result)[2] = ((V)[0]*(M)[2]) + ((V)[1]*(M)[6]) + ((V)[2]*(M)[10]) + ((M)[14]); \
}

// Multiplies Matrices M and N and stores result to M
// mulMatrix(MATRIX M, MATRIX N)
#define matrix_mult2(M, N) \
{ \
  MATRIX tmpM; \
  loopj(4) { \
    loopi(4) { \
      tmpM[i+4*j] = ((N)[i+4*0]*(M)[0+4*j]) + ((N)[i+4*1]*(M)[1+4*j])+ \
                    ((N)[i+4*2]*(M)[2+4*j]) + ((N)[i+4*3]*(M)[3+4*j]); \
    }; \
  }; \
  memcpy((M), tmpM, sizeof(MATRIX)); \
}

// Add Matrices M and N and stores result to M
// addMatrix(MATRIX M, MATRIX N)
#define matrix_add2(M, N) \
{ \
    loopi(16) \
      (M)[i] += (N)[i]; \
}

// Rotate Matrix M around X-Axis
// RXMatrix(MATRIX M, ANGLE x)
#define matrix_rotx(M, x) \
{ \
  MATRIX N; \
  float c = cos((x)*PI_OVER_180); \
  float s = sin((x)*PI_OVER_180); \
  matrix_set(N, 1, 0, 0, 0, \
                0, c, s, 0, \
                0,-s, c, 0, \
                0, 0, 0, 1); \
  matrix_mult2((M), N); \
}

// Rotate Matrix M around Y-Axis
// RYMatrix(MATRIX M, ANGLE y)
#define matrix_roty(M, y) \
{ \
  MATRIX N; \
  float c = cos((y)*PI_OVER_180); \
  float s = sin((y)*PI_OVER_180); \
  matrix_set(N, c, 0,-s, 0, \
                0, 1, 0, 0, \
                s, 0, c, 0, \
                0, 0, 0, 1); \
  matrix_mult2((M), N); \
}

// Rotate Matrix M around Z-Axis
// RZMatrix(MATRIX M, ANGLE z)
#define matrix_rotz(M, z) \
{ \
  MATRIX N; \
  float c = cos((z)*PI_OVER_180); \
  float s = sin((z)*PI_OVER_180); \
  matrix_set(N, c, s, 0, 0, \
               -s, c, 0, 0, \
                0, 0, 1, 0, \
                0, 0, 0, 1); \
  matrix_mult2((M), N); \
}

// Translates Matrix M by (x,y,z)
// TMatrix(MATRIX M, float x, float y, float z)
#define matrix_translate(M, x, y, z) \
{ \
  (M)[12] += x; \
  (M)[13] += y; \
  (M)[14] += z; \
}

// Scales Matrix M with factors (x,y,z)
// SMatrix(MATRIX M, float x, float y, float z)
#define matrix_scale(M, x, y, z) \
{ \
  MATRIX N; \
  matrix_set(N, (x),  0,  0,  0, \
                  0,(y),  0,  0, \
                  0,  0,(z),  0, \
                  0,  0,  0,  1); \
  matrix_mult2(M, N); \
}


// Quaternion
//

// Prints a 4f quaternion.
#define quat_print(q0) \
{ \
    printf( "< %2.4f %2.4f %2.4f %2.4f >\n", (q0)[0], (q0)[1], (q0)[2], (q0)[3] ); \
}

// Prints a 4f quaternion.
#define quat_print_pos(q0) \
{ \
    if ((q0)[3] > 0) printf( "< %2.4f %2.4f %2.4f %2.4f >\n", (q0)[0], (q0)[1], (q0)[2], (q0)[3] ); \
    else printf( "< %2.4f %2.4f %2.4f %2.4f >\n", -(q0)[0], -(q0)[1], -(q0)[2], -(q0)[3] ); \
}

// "Rectify" a quaternion so that the W-component is positive.
#define quat_rectify_pos(q0) \
{ \
    if ((q0)[3] < 0) { (q0)[0] = -(q0)[0];  (q0)[1] = -(q0)[1];  (q0)[2] = -(q0)[2];  (q0)[3] = -(q0)[3]; } \
}

// "Rectify" a quaternion so that the W-component is negative.
#define quat_rectify_neg(q0) \
{ \
    if ((q0)[3] > 0) { (q0)[0] = -(q0)[0];  (q0)[1] = -(q0)[1];  (q0)[2] = -(q0)[2];  (q0)[3] = -(q0)[3]; } \
}

#define quat_zero(result) \
{ \
    (result)[0] =  \
    (result)[1] =  \
    (result)[2] = 0.0f; \
    (result)[3] = 1.0f; \
}

#define quat_set(result, x, y, z, w) \
{ \
    (result)[0] = (x); \
    (result)[1] = (y); \
    (result)[2] = (z); \
    (result)[3] = (w); \
}

#define quat_expand(q_) \
{ \
    const float w2 = 1.0f - ( (q_[0]*q_[0]) + ((q_)[1]*(q_)[1]) + ((q_)[2]*(q_)[2]) ); \
    q_[3] = (w2 < 0) ? 0.0f : -sqrtf(w2); \
}

#define quat_cpy(result, q0) \
{ \
    (result)[0] = (q0)[0]; \
    (result)[1] = (q0)[1]; \
    (result)[2] = (q0)[2]; \
    (result)[3] = (q0)[3]; \
}

#define quat_rotaxis(result, radtheta, vnorm) \
{ \
    float radtheta_half = radtheta * 0.5f; \
    float sinhalf = sin(radtheta_half); \
    (result)[0] = (vnorm)[0] * sinhalf; \
    (result)[1] = (vnorm)[1] * sinhalf; \
    (result)[2] = (vnorm)[2] * sinhalf; \
    (result)[3] = cos(radtheta_half); \
}

#define quat_apply(result, q0, v) \
{ \
    const float _v4f_[] = { (v)[0], (v)[1], (v)[2], 0.0f }; \
    const float _q_conj_[] = { -(q0)[0], -(q0)[1], -(q0)[2], (q0)[3] }; \
    quat _temp0_; \
    quat _temp1_; \
    quat_mul(_temp0_, (q0), _v4f_); \
    quat_mul(_temp1_, _temp0_, _q_conj_); \
    vector_set((result), _temp1_[0],_temp1_[1],_temp1_[2]); \
}

#define quat_nlerp(result, q0, q1, delta) \
{ \
    quat _q0_part_; \
    quat _q1_part_; \
    quat _unnorm_; \
    quat_scale(_q0_part_, (q0), (1.0f - (delta))); \
    quat_scale(_q1_part_, (q1), delta); \
    quat_add(_unnorm_, _q0_part_, _q1_part_); \
    const float mag_inv = 1.0f / quat_mag(_unnorm_); \
    quat_scale((result), _unnorm_, mag_inv); \
}

#define quat_mag(q0) \
( \
    sqrtf( (q0)[0] * (q0)[0] + (q0)[1] * (q0)[1] + (q0)[2] * (q0)[2] + (q0)[3] * (q0)[3] ) \
)

#define quat_mag_inv(q0) \
( \
    1.0 / quat_mag(q0) \
)

#define quat_conj(q_) \
{ \
    (q_)[0] = -(q_)[0]; \
    (q_)[1] = -(q_)[1]; \
    (q_)[2] = -(q_)[2]; \
    (q_)[3] = +(q_)[3]; \
}

#define quat_scale(q_, q0, scale) \
{ \
    auto _sf_ = scale; \
    (q_)[0] = (q0)[0] * (_sf_); \
    (q_)[1] = (q0)[1] * (_sf_); \
    (q_)[2] = (q0)[2] * (_sf_); \
    (q_)[3] = (q0)[3] * (_sf_); \
}

#define quat_add(q_, q0, q1) \
{ \
    (q_)[0] = (q0)[0] + (q1)[0]; \
    (q_)[1] = (q0)[1] + (q1)[1]; \
    (q_)[2] = (q0)[2] + (q1)[2]; \
    (q_)[3] = (q0)[3] + (q1)[3]; \
}

#define quat_mul_(q_, q0, q1) \
{ \
  (q_)[0] = (q0)[3] * (q1)[0] + (q0)[0] * (q1)[3] + (q0)[1] * (q1)[2] - (q0)[2] * (q1)[1]; \
  (q_)[1] = (q0)[3] * (q1)[1] + (q0)[1] * (q1)[3] + (q0)[2] * (q1)[0] - (q0)[0] * (q1)[2]; \
  (q_)[2] = (q0)[3] * (q1)[2] + (q0)[2] * (q1)[3] + (q0)[0] * (q1)[1] - (q0)[1] * (q1)[0]; \
  (q_)[3] = (q0)[3] * (q1)[3] - (q0)[0] * (q1)[0] - (q0)[1] * (q1)[1] - (q0)[2] * (q1)[2]; \
}

#define quat_mul(q_, q0, q1) \
{ \
    const float _a_ = ((q0)[3] + (q0)[0]) * ((q1)[3] + (q1)[0]); \
    const float _b_ = ((q0)[2] - (q0)[1]) * ((q1)[1] - (q1)[2]); \
    const float _c_ = ((q0)[3] - (q0)[0]) * ((q1)[1] + (q1)[2]);  \
    const float _d_ = ((q0)[1] + (q0)[2]) * ((q1)[3] - (q1)[0]); \
    const float _e_ = ((q0)[0] + (q0)[2]) * ((q1)[0] + (q1)[1]); \
    const float _f_ = ((q0)[0] - (q0)[2]) * ((q1)[0] - (q1)[1]); \
    const float _g_ = ((q0)[3] + (q0)[1]) * ((q1)[3] - (q1)[2]); \
    const float _h_ = ((q0)[3] - (q0)[1]) * ((q1)[3] + (q1)[2]); \
    (q_)[0] = _a_ - ( _e_ + _f_ + _g_ + _h_) * 0.5f; \
    (q_)[1] = _c_ + ( _e_ - _f_ + _g_ - _h_) * 0.5f; \
    (q_)[2] = _d_ + ( _e_ - _f_ - _g_ + _h_) * 0.5f; \
    (q_)[3] = _b_ + (-_e_ - _f_ + _g_ + _h_) * 0.5f; \
}


// Vector operations

// Prints a 3f vector.
#define vector_print(A) \
{ \
    printf("< %3.4f %3.4f %3.4f >\n", (A)[0], (A)[1], (A)[2]); \
}

#define vector_new(x, y, z)             ({ float* v = new vec3; v[0] = (x);  v[1] = (y); v[2] = (z); v; })

#define vector_zero(result)             { (result)[0] = (result)[1] = (result)[2] = 0.0f; }

#define vector_cpy(result, A)           { (result)[0] = (A)[0]; (result)[1] = (A)[1]; (result)[2] = (A)[2]; }

#define vector_set(result, x, y, z)     { (result)[0] = x; (result)[1] = y; (result)[2] = z; }

#define vector_set3i(result, fun_of_i)  { loop3i(result[i] = fun_of_i;) }

#define vector_norm(result, A)          { auto inv = 1/vector_mag((A)); vector_scale( (result), (A), inv); }

#define vector_mag(A)                   ( sqrtf( (A)[0]*(A)[0] + (A)[1]*(A)[1] + (A)[2]*(A)[2] ) )

#define vector_dot(A, B)                ((A)[0] * (B)[0] + (A)[1] * (B)[1] + (A)[2] * (B)[2] )

#define vector_neg(result, A)           { (result)[0] = -(A)[0]; (result)[1] = -(A)[1]; (result)[2] = -(A)[2]; }

#define vector_cross(result, A, B)      { (result)[0] = (A)[1] * (B)[2] - (A)[2] * (B)[1]; (result)[1] = (A)[2] * (B)[0] - (A)[0] * (B)[2]; (result)[2] = (A)[0] * (B)[1] - (A)[1] * (B)[0]; }

#define vector_add(result, A, B)        { (result)[0] = (A)[0] + (B)[0]; (result)[1] = (A)[1] + (B)[1]; (result)[2] = (A)[2] + (B)[2]; }

#define vector_sub(result, A, B)        { (result)[0] = (A)[0] - (B)[0]; (result)[1] = (A)[1] - (B)[1]; (result)[2] = (A)[2] - (B)[2]; }

#define vector_scale(result, A, B)      { auto _sf_ = (B); (result)[0] = (A)[0] * _sf_; (result)[1] = (A)[1] * _sf_; (result)[2] = (A)[2] * _sf_; }

#define vector_muladd(result, A, B, C)  { auto _sf_ = (C); (result)[0] = (A)[0] + (B)[0] * _sf_; (result)[1] = (A)[1] + (B)[1] * _sf_; (result)[2] = (A)[2] + (B)[2] * _sf_; }

#define vector_sqrdist(A, B)            ( ((A)[0]-(B)[0]) * ((A)[0]-(B)[0]) + ((A)[1]-(B)[1]) * ((A)[1]-(B)[1]) + ((A)[2]-(B)[2]) * ((A)[2]-(B)[2]) )

#define vector_distance(A, B)           ( sqrtf( ((A)[0]-(B)[0]) * ((A)[0]-(B)[0]) + ((A)[1]-(B)[1]) * ((A)[1]-(B)[1]) + ((A)[2]-(B)[2]) * ((A)[2]-(B)[2]) ) )

#define vector_manhattan(A, B)          ( fabs((A)[0]-(B)[0])) + fabs((A)[1]-(B)[1])) + fabs((A)[2]-(B)[2]))  )

#define vector_rotx(result, radx) \
{ \
  MATRIX M; \
  float c = cos(radx); \
  float s = sin(radx); \
  matrix_set(M, 1, 0, 0, 0, \
               0, c, s, 0, \
               0,-s, c, 0, \
               0, 0, 0, 1); \
  matrix_apply2(M, (result)); \
}

// Rotate Vector Around Y-Axis (VECTOR V, ANGLE y)
#define vector_roty(result, rady) \
{ \
  MATRIX M; \
  float c = cos(rady); \
  float s = sin(rady); \
  matrix_set(M, c, 0,-s, 0, \
                0, 1, 0, 0, \
                s, 0, c, 0, \
                0, 0, 0, 1); \
  matrix_apply2(M, (result)); \
}

// Rotate Vector Around Z-Axis (VECTOR V, ANGLE z)
#define vector_rotz(result, radz) \
{ \
  MATRIX M; \
  float c = cos(radz); \
  float s = sin(radz); \
  matrix_set(M, c, s, 0, 0, \
               -s, c, 0, 0, \
                0, 0, 1, 0, \
                0, 0, 0, 1); \
  matrix_apply2(M, (result)); \
}



#endif
