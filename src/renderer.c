#include "renderer.h"
#include "canvas.h"
#include "math3d.h"
#include <math.h>

// Projects a 3D vertex through all transformations
vec3_t project_vertex(vec3_t vertex, mat4_t mvp, int screen_width, int screen_height) {
    vec4_t v = {vertex.x, vertex.y, vertex.z, 1.0f};
    vec4_t transformed = mat4_mul_vec4(mvp, v);
    
    // Perspective divide
    if (transformed.w != 0.0f) {
        transformed.x /= transformed.w;
        transformed.y /= transformed.w;
        transformed.z /= transformed.w;
    }

    // Convert to screen coordinates
    vec3_t screen;
    screen.x = (transformed.x * 0.5f + 0.5f) * screen_width;
    screen.y = (1.0f - (transformed.y * 0.5f + 0.5f)) * screen_height; // Flip Y
    screen.z = transformed.z;
    return screen;
}

// Clips point to a circular viewport
int clip_to_circular_viewport(canvas_t* canvas, float x, float y) {
    float cx = canvas->width / 2.0f;
    float cy = canvas->height / 2.0f;
    float dx = x - cx;
    float dy = y - cy;
    float dist_sq = dx * dx + dy * dy;
    float radius = (canvas->width < canvas->height ? canvas->width : canvas->height) / 2.0f;
    return dist_sq <= radius * radius;
}

// Render wireframe from list of vertices and edge indices
void render_wireframe(canvas_t* canvas, vec3_t* vertices, int num_vertices, int (*edges)[2], int num_edges, mat4_t mvp) {
    vec3_t projected[256]; // max 256 verts
    for (int i = 0; i < num_vertices; i++) {
        projected[i] = project_vertex(vertices[i], mvp, canvas->width, canvas->height);
    }

    for (int i = 0; i < num_edges; i++) {
        vec3_t a = projected[edges[i][0]];
        vec3_t b = projected[edges[i][1]];

        // Simple Z-sort: skip if behind
        if (a.z < -1.0f || a.z > 1.0f || b.z < -1.0f || b.z > 1.0f) continue;

        if (clip_to_circular_viewport(canvas, a.x, a.y) && clip_to_circular_viewport(canvas, b.x, b.y)) {
            draw_line_f(canvas, a.x, a.y, b.x, b.y, 1.0f);
        }
    }
}
