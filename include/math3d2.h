#ifndef MATH3D_H
#define MATH3D_H

#include <math.h>

// 3D Vector structure
typedef struct {
    float x, y, z;// Cartesian coordinates
    
    // Spherical coordinates
    float r;      // radius (distance from origin)
    float theta;  // horizontal angle (radians)
    float phi;    // vertical angle (radians)
} vec3_t;

// 4x4 Matrix structure
typedef struct {
    float m[16];  // Column-major layout
} mat4_t;

// Vector function declarations
vec3_t vec3_from_spherical(float r, float theta, float phi);
vec3_t vec3_normalize(vec3_t v);
vec3_t vec3_normalize_fast(vec3_t v);
vec3_t vec3_slerp(vec3_t a, vec3_t b, float t);
void vec3_update_spherical(vec3_t* v);
void vec3_update_cartesian(vec3_t* v);

// Matrix function declarations
mat4_t mat4_identity();
mat4_t mat4_translate(float tx, float ty, float tz);
mat4_t mat4_scale(float sx, float sy, float sz);
mat4_t mat4_rotate_x(float angle);
mat4_t mat4_rotate_y(float angle);
mat4_t mat4_rotate_z(float angle);
mat4_t mat4_rotate_xyz(float rx, float ry, float rz);
mat4_t mat4_multiply(mat4_t a, mat4_t b);
vec3_t mat4_multiply_vec3(mat4_t m, vec3_t v);
mat4_t mat4_frustum_asymmetric(float left, float right, float bottom, float top, float near, float far);

// Helper functions
float fast_inverse_sqrt(float x);
void print_vec3(vec3_t v);
void print_mat4(mat4_t m);

#endif