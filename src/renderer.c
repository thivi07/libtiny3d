#include <stdio.h>
#include "renderer.h"
#include "canvas.h"
#include <math.h>

// Projects a 3D vertex through model → view → projection transforms
vec3_t project_vertex(vec3_t vertex, mat4_t model, mat4_t view, mat4_t projection) {
    vec3_t world = mat4_transform_vec3(model, vertex);
    vec3_t camera = mat4_transform_vec3(view, world);
    vec3_t projected = mat4_transform_vec3(projection, camera);
    return projected;
}

// Clips a pixel to a circular viewport at canvas center
int clip_to_circular_viewport(canvas_t* canvas, int x, int y) {
    int cx = canvas->width / 2;
    int cy = canvas->height / 2;
    int radius = (canvas->width < canvas->height ? canvas->width : canvas->height) / 2;
    int dx = x - cx;
    int dy = y - cy;
    return (dx * dx + dy * dy <= radius * radius);
}

// Draws a wireframe using projected 3D vertices
void render_wireframe(canvas_t* canvas, vec3_t* vertices, int vertex_count, int (*edges)[2], int edge_count,
                      mat4_t model, mat4_t view, mat4_t projection) {
    for (int i = 0; i < edge_count; ++i) {
        vec3_t p0 = project_vertex(vertices[edges[i][0]], model, view, projection);
        vec3_t p1 = project_vertex(vertices[edges[i][1]], model, view, projection);

        int x0 = (int)((p0.x + 1.0f) * 0.5f * canvas->width);
        int y0 = (int)((1.0f - (p0.y + 1.0f) * 0.5f) * canvas->height);
        int x1 = (int)((p1.x + 1.0f) * 0.5f * canvas->width);
        int y1 = (int)((1.0f - (p1.y + 1.0f) * 0.5f) * canvas->height);

        if (clip_to_circular_viewport(canvas, x0, y0) || clip_to_circular_viewport(canvas, x1, y1)) {
            draw_line_f(canvas, x0, y0, x1, y1, 1.0f); // Draw white line
        }
    }
}