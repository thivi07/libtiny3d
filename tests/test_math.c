#include <stdio.h>
#include <math.h>
#include "../include/math3d.h"
#include "canvas.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#define DEG2RAD(a) ((a) * M_PI / 180.0f)


vec3_t cube[8] = {
        // Bottom face (z = -1)
        {-0.5f, -0.5f, -0.5f, 0, 0, 0}, // 0: bottom-left-back
        { 0.5f, -0.5f, -0.5f, 0, 0, 0}, // 1: bottom-right-back
        { 0.5f,  0.5f, -0.5f, 0, 0, 0}, // 2: top-right-back
        {-0.5f,  0.5f, -0.5f, 0, 0, 0}, // 3: top-left-back
        // Top face (z = 1)
        {-0.5f, -0.5f,  0.5f, 0, 0, 0}, // 4: bottom-left-front
        { 0.5f, -0.5f,  0.5f, 0, 0, 0}, // 5: bottom-right-front
        { 0.5f,  0.5f,  0.5f, 0, 0, 0}, // 6: top-right-front
        {-0.5f,  0.5f,  0.5f, 0, 0, 0}  // 7: top-left-front
    };

// Cube edges for wireframe rendering
int edges[12][2] = {
    {0,1}, {1,2}, {2,3}, {3,0}, // back face
    {4,5}, {5,6}, {6,7}, {7,4}, // front face
    {0,4}, {1,5}, {2,6}, {3,7}  // side edges
};

int main() {
    int canvas_width = 400, canvas_height = 400;
    canvas_t* canvas = canvas_create(canvas_width, canvas_height);

    mat4_t rotate = mat4_rotate_xyz(DEG2RAD(30), DEG2RAD(45), 0);
    mat4_t translate = mat4_translate(0, 0, -3);
    mat4_t model = mat4_multiply(translate, rotate);

    float fov = DEG2RAD(60.0f);
    float aspect = (float)canvas_width / canvas_height;
    float top = tanf(fov / 2.0f);
    float right = top * aspect;

    mat4_t proj = mat4_frustum_asymmetric(-right, right, -top, top, 1.0f, 10.0f);
    mat4_t mvp = mat4_multiply(proj, model);

    // Project and convert to screen space
    vec3_t screen[8];
    for (int i = 0; i < 8; ++i) {
        vec3_t p = mat4_transform_vec3(mvp, cube[i]);
        screen[i].x = (p.x + 1.0f) * 0.5f * canvas_width;
        screen[i].y = (1.0f - p.y) * 0.5f * canvas_height;
    }

    // Draw edges
    for (int i = 0; i < 12; i++) {
        int a = edges[i][0];
        int b = edges[i][1];
        draw_line_f(canvas, screen[a].x, screen[a].y, screen[b].x, screen[b].y, 1.0f);
    }

    canvas_save_ppm(canvas, "cube_output.ppm");
    canvas_destroy(canvas);
    return 0;
}
#endif