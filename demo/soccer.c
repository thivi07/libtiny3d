// In soccer.c or main.c
#include "renderer.h"
#include <math.h>

#define PI 3.14159265f

// Vertices and edges of a truncated icosahedron (soccer ball)
// Precomputed data (shortened here for example, complete version should include all)
static const vec3_t soccer_vertices[] = {
    {0, 0, 1.07047}, {0.71364, 0, 0.79788}, {-0.35682, 0.61803, 0.79788},
    // ... (add all 60 vertices of the truncated icosahedron)
};

static const int soccer_edges[][2] = {
    {0, 1}, {1, 2}, {2, 0},
    // ... (add all ~90 edges)
};

void generate_soccer_ball(object3d_t* obj) {
    obj->vertex_count = sizeof(soccer_vertices) / sizeof(vec3_t);
    obj->vertices = malloc(obj->vertex_count * sizeof(vec3_t));
    for (int i = 0; i < obj->vertex_count; ++i) {
        obj->vertices[i] = soccer_vertices[i];
    }

    obj->edge_count = sizeof(soccer_edges) / sizeof(soccer_edges[0]);
    obj->edges = malloc(obj->edge_count * sizeof(edge_t));
    for (int i = 0; i < obj->edge_count; ++i) {
        obj->edges[i].a = soccer_edges[i][0];
        obj->edges[i].b = soccer_edges[i][1];
    }
}
