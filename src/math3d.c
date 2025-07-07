#include "math3d2.h"


// Vector functions //

vec3_t vec3_from_spherical(float r, float theta, float phi) {
    vec3_t v;
    v.r = r;
    v.theta = theta;
    v.phi = phi;

    // Convert to Cartesian
    v.x = r * sinf(phi) * cosf(theta);
    v.y = r * sinf(phi) * sinf(theta);
    v.z = r * cosf(phi);

    return v;
}

vec3_t vec3_normalize_fast(vec3_t v) {
    float len_sq = v.x * v.x + v.y * v.y + v.z * v.z;
    float inv_len = 1.0f / sqrtf(len_sq);
    v.x *= inv_len;
    v.y *= inv_len;
    v.z *= inv_len;
    return v;
}

vec3_t vec3_slerp(vec3_t a, vec3_t b, float t) {
    // Normalize both vectors
    a = vec3_normalize_fast(a);
    b = vec3_normalize_fast(b);

    // Calculate dot product
    float dot = a.x * b.x + a.y * b.y + a.z * b.z;
    dot = fmaxf(fminf(dot, 1.0f), -1.0f);

    float theta = acosf(dot) * t;

    vec3_t rel = {
        b.x - a.x * dot,
        b.y - a.y * dot,
        b.z - a.z * dot
    };
    rel = vec3_normalize_fast(rel);

    vec3_t result = {
        a.x * cosf(theta) + rel.x * sinf(theta),
        a.y * cosf(theta) + rel.y * sinf(theta),
        a.z * cosf(theta) + rel.z * sinf(theta)
    };
    return result;
}



// Matrix functions //

mat4_t mat4_identity() {
    mat4_t mat = {0};
    mat.m[0] = mat.m[5] = mat.m[10] = mat.m[15] = 1.0f; //[0][0]=[1][1]=[2][2]=[3][3]
    return mat;
}

mat4_t mat4_translate(float tx, float ty, float tz) {
    mat4_t mat = mat4_identity();
    mat.m[12] = tx; // [0][3]
    mat.m[13] = ty; // [1][3]
    mat.m[14] = tz; // [2][3]
    return mat;
}

mat4_t mat4_scale(float sx, float sy, float sz) {
    mat4_t mat = {0};
    mat.m[0] = sx;  // [0][0]
    mat.m[5] = sy;  // [1][1]
    mat.m[10] = sz; // [2][2]
    mat.m[15] = 1.0f;// [3][3]
    return mat;
}

mat4_t mat4_rotate_xyz(float rx, float ry, float rz) {
    float cx = cosf(rx), sx = sinf(rx);
    float cy = cosf(ry), sy = sinf(ry);
    float cz = cosf(rz), sz = sinf(rz);

    mat4_t rxm = mat4_identity();
    rxm.m[5] = cx;  rxm.m[6] = -sx; //  [1][1]  [2][1]
    rxm.m[9] = sx;  rxm.m[10] = cx; //  [1][2]  [2][2]

    mat4_t rym = mat4_identity();   
    rym.m[0] = cy;  rym.m[2] = sy;  //  [0][0]  [2][0]
    rym.m[8] = -sy; rym.m[10] = cy; //  [0][2]  [2][2]

    mat4_t rzm = mat4_identity();   
    rzm.m[0] = cz;  rzm.m[1] = -sz; //  [0][0]  [1][0]
    rzm.m[4] = sz;  rzm.m[5] = cz;  //  [0][1]  [1][1]

    return mat4_multiply(rzm, mat4_multiply(rym, rxm));
}

mat4_t mat4_frustum_asymmetric(float l, float r, float b, float t, float n, float f) {
    //l-left, r-rigth, b-bottom, t-top, n-near, f-far
    mat4_t m = {0};
    m.m[0] = (2 * n) / (r - l);
    m.m[5] = (2 * n) / (t - b);
    m.m[8] = (r + l) / (r - l);
    m.m[9] = (t + b) / (t - b);
    m.m[10] = -(f + n) / (f - n);
    m.m[11] = -1;
    m.m[14] = -(2 * f * n) / (f - n);
    return m;
}

mat4_t mat4_multiply(mat4_t A, mat4_t B) {
    mat4_t result = {0};
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            for (int k = 0; k < 4; ++k) {
                result.m[col * 4 + row] += A.m[k * 4 + row] * B.m[col * 4 + k];
            }
        }
    }
    return result;
}

vec3_t mat4_transform_vec3(mat4_t mat, vec3_t v) {
    float x = v.x, y = v.y, z = v.z;
    float tx = mat.m[0] * x + mat.m[4] * y + mat.m[8] * z + mat.m[12];
    float ty = mat.m[1] * x + mat.m[5] * y + mat.m[9] * z + mat.m[13];
    float tz = mat.m[2] * x + mat.m[6] * y + mat.m[10] * z + mat.m[14];
    float tw = mat.m[3] * x + mat.m[7] * y + mat.m[11] * z + mat.m[15];

    if (tw != 0.0f) {
        tx /= tw; ty /= tw; tz /= tw;
    }

    vec3_t result = {tx, ty, tz};
    return result;
}

