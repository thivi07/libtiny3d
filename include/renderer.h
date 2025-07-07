#ifndef RENDERER_H
#define RENDERER_H

#include "canvas.h"
#include "math3d.h"

vec3_t project_vertex(vec3_t vertex, mat4_t mvp, int screen_width, int screen_height);
int clip_to_circular_viewport(canvas_t* canvas, float x, float y);
void render_wireframe(canvas_t* canvas, vec3_t* vertices, int num_vertices, int (*edges)[2], int num_edges, mat4_t mvp);

#endif
