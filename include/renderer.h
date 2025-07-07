// File: include/renderer.h
#ifndef RENDERER_H
#define RENDERER_H

#include "canvas.h"
#include "math3d.h"

// Project a vertex from world space to screen space
vec3_t project_vertex(vec3_t vertex, mat4_t model, mat4_t view, mat4_t projection);

// Check if pixel is inside circular viewport
int clip_to_circular_viewport(canvas_t* canvas, int x, int y);

// Draw a 3D object as a wireframe
void render_wireframe(canvas_t* canvas, vec3_t* vertices, int vertex_count, int (*edges)[2], int edge_count,
                      mat4_t model, mat4_t view, mat4_t projection);

#endif
