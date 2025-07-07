#ifndef MATH3D_H
#define MATH3D_H
#include <math.h>


// 3D Vector (Cartesian + Spherical)

typedef struct {
    float x, y, z;         // Cartesian
    float r, theta, phi;   // Spherical: r = radius(distance from origin), theta = horizontal angle, vertical angle (radians)
} vec3_t;

// Vector operations
vec3_t vec3_from_spherical(float r, float theta, float phi);
vec3_t vec3_normalize_fast(vec3_t v);
vec3_t vec3_slerp(vec3_t a, vec3_t b, float t);


// 4x4 Matrix (Column-major)

typedef struct {
    float m[16];  // Column-major matrix: m[col * 4 + row]
} mat4_t;

// Matrix generators
mat4_t mat4_identity();
mat4_t mat4_translate(float tx, float ty, float tz);
mat4_t mat4_scale(float sx, float sy, float sz);
mat4_t mat4_rotate_xyz(float rx, float ry, float rz);
mat4_t mat4_frustum_asymmetric(float left, float right, float bottom, float top, float near, float far);

mat4_t mat4_multiply(mat4_t A, mat4_t B);   // Matrix multiply: result = A * B

vec3_t mat4_transform_vec3(mat4_t mat, vec3_t v);   // Transform vector by matrix

#endif

