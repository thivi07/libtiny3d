#include "math3d.h"
#include <stdio.h>
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846

// =============================================================================
// VECTOR FUNCTIONS
// =============================================================================

vec3_t vec3_from_spherical(float r, float theta, float phi) {
    vec3_t result;
    result.r = r;
    result.theta = theta;
    result.phi = phi;
    
    // Convert to Cartesian
    result.x = r * sin(phi) * cos(theta);
    result.y = r * sin(phi) * sin(theta);
    result.z = r * cos(phi);
    
    return result;
}

void vec3_update_spherical(vec3_t* v) {
    v->r = sqrt(v->x*v->x + v->y*v->y + v->z*v->z);
    if (v->r > 0.0f) {
        v->theta = atan2(v->y, v->x);
        v->phi = acos(v->z / v->r);
    } else {
        v->theta = 0.0f;
        v->phi = 0.0f;
    }
}

void vec3_update_cartesian(vec3_t* v) {
    v->x = v->r * sin(v->phi) * cos(v->theta);
    v->y = v->r * sin(v->phi) * sin(v->theta);
    v->z = v->r * cos(v->phi);
}

vec3_t vec3_normalize(vec3_t v) {
    float length = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
    if (length > 0.0f) {
        v.x /= length;
        v.y /= length;
        v.z /= length;
    }
    vec3_update_spherical(&v);
    return v;
}

float fast_inverse_sqrt(float x) {
    float xhalf = 0.5f * x;
    int i = *(int*)&x;
    i = 0x5f3759df - (i >> 1);
    x = *(float*)&i;
    x = x * (1.5f - (xhalf * x * x));
    return x;
}

vec3_t vec3_normalize_fast(vec3_t v) {
    float inv_length = fast_inverse_sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
    v.x *= inv_length;
    v.y *= inv_length;
    v.z *= inv_length;
    vec3_update_spherical(&v);
    return v;
}

vec3_t vec3_slerp(vec3_t a, vec3_t b, float t) {
    // Normalize both vectors
    a = vec3_normalize(a);
    b = vec3_normalize(b);
    
    // Calculate dot product
    float dot = a.x*b.x + a.y*b.y + a.z*b.z;
    
    // Clamp dot to avoid numerical errors
    if (dot > 0.9995f) {
        // Vectors are almost identical, use linear interpolation
        vec3_t result;
        result.x = a.x + t * (b.x - a.x);
        result.y = a.y + t * (b.y - a.y);
        result.z = a.z + t * (b.z - a.z);
        return vec3_normalize(result);
    }
    
    float angle = acos(fabs(dot));
    float sin_angle = sin(angle);
    float weight_a = sin((1.0f - t) * angle) / sin_angle;
    float weight_b = sin(t * angle) / sin_angle;
    
    vec3_t result;
    result.x = weight_a * a.x + weight_b * b.x;
    result.y = weight_a * a.y + weight_b * b.y;
    result.z = weight_a * a.z + weight_b * b.z;
    
    vec3_update_spherical(&result);
    return result;
}

// =============================================================================
// MATRIX FUNCTIONS
// =============================================================================

mat4_t mat4_identity() {
    mat4_t result = {0};
    result.m[0] = 1.0f;   // [0][0]
    result.m[5] = 1.0f;   // [1][1]
    result.m[10] = 1.0f;  // [2][2]
    result.m[15] = 1.0f;  // [3][3]
    return result;
}

mat4_t mat4_translate(float tx, float ty, float tz) {
    mat4_t result = mat4_identity();
    result.m[12] = tx;  // [0][3]
    result.m[13] = ty;  // [1][3]
    result.m[14] = tz;  // [2][3]
    return result;
}

mat4_t mat4_scale(float sx, float sy, float sz) {
    mat4_t result = {0};
    result.m[0] = sx;     // [0][0]
    result.m[5] = sy;     // [1][1]
    result.m[10] = sz;    // [2][2]
    result.m[15] = 1.0f;  // [3][3]
    return result;
}

mat4_t mat4_rotate_x(float angle) {
    mat4_t result = mat4_identity();
    float c = cos(angle);
    float s = sin(angle);
    
    result.m[5] = c;   // [1][1]
    result.m[6] = s;   // [2][1]
    result.m[9] = -s;  // [1][2]
    result.m[10] = c;  // [2][2]
    return result;
}

mat4_t mat4_rotate_y(float angle) {
    mat4_t result = mat4_identity();
    float c = cos(angle);
    float s = sin(angle);
    
    result.m[0] = c;   // [0][0]
    result.m[2] = -s;  // [2][0]
    result.m[8] = s;   // [0][2]
    result.m[10] = c;  // [2][2]
    return result;
}

mat4_t mat4_rotate_z(float angle) {
    mat4_t result = mat4_identity();
    float c = cos(angle);
    float s = sin(angle);
    
    result.m[0] = c;   // [0][0]
    result.m[1] = s;   // [1][0]
    result.m[4] = -s;  // [0][1]
    result.m[5] = c;   // [1][1]
    return result;
}

mat4_t mat4_rotate_xyz(float rx, float ry, float rz) {
    mat4_t result = mat4_identity();
    result = mat4_multiply(result, mat4_rotate_x(rx));
    result = mat4_multiply(result, mat4_rotate_y(ry));
    result = mat4_multiply(result, mat4_rotate_z(rz));
    return result;
}

mat4_t mat4_multiply(mat4_t a, mat4_t b) {
    mat4_t result = {0};
    
    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            float sum = 0.0f;
            for (int k = 0; k < 4; k++) {
                sum += a.m[k*4 + row] * b.m[col*4 + k];
            }
            result.m[col*4 + row] = sum;
        }
    }
    
    return result;
}

vec3_t mat4_multiply_vec3(mat4_t m, vec3_t v) {
    vec3_t result;
    
    // Treat vector as (x, y, z, 1) for point transformation
    result.x = m.m[0]*v.x + m.m[4]*v.y + m.m[8]*v.z + m.m[12];
    result.y = m.m[1]*v.x + m.m[5]*v.y + m.m[9]*v.z + m.m[13];
    result.z = m.m[2]*v.x + m.m[6]*v.y + m.m[10]*v.z + m.m[14];
    
    vec3_update_spherical(&result);
    return result;
}

mat4_t mat4_frustum_asymmetric(float left, float right, float bottom, float top, float near, float far) {
    mat4_t result = {0};
    
    result.m[0] = (2.0f * near) / (right - left);
    result.m[5] = (2.0f * near) / (top - bottom);
    result.m[8] = (right + left) / (right - left);
    result.m[9] = (top + bottom) / (top - bottom);
    result.m[10] = -(far + near) / (far - near);
    result.m[11] = -1.0f;
    result.m[14] = -(2.0f * far * near) / (far - near);
    
    return result;
}

// =============================================================================
// HELPER FUNCTIONS
// =============================================================================

void print_vec3(vec3_t v) {
    printf("Vec3: Cart(%.2f, %.2f, %.2f) Sph(r=%.2f, theta=%.2f deg, phi=%.2f deg)\n",
           v.x, v.y, v.z, v.r, v.theta * 180.0f/M_PI, v.phi * 180.0f/M_PI);
}

void print_mat4(mat4_t m) {
    printf("Mat4:\n");
    for (int row = 0; row < 4; row++) {
        printf("  [");
        for (int col = 0; col < 4; col++) {
            printf("%8.2f", m.m[col*4 + row]);
        }
        printf(" ]\n");
    }
}

#endif