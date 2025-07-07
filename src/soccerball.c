#include <stdio.h>
#include <stdlib.h>
#include "math3d.h"
#include <math.h>

// Golden ratio constants
#define C0 0.8090169943749474f    // (1 + sqrt(5)) / 4
#define C1 1.618033988749895f     // (1 + sqrt(5)) / 2
#define C2 1.8090169943749474f    // (5 + sqrt(5)) / 4
#define C3 2.118033988749895f     // (2 + sqrt(5)) / 2
#define C4 2.4270509831248424f    // 3 * (1 + sqrt(5)) / 4

// 60 vertices of truncated icosahedron
static const vec3_t raw_vertices[60] = {
    {  0.5f,  0.0f,  C4 }, {  0.5f,  0.0f, -C4 }, { -0.5f,  0.0f,  C4 }, { -0.5f,  0.0f, -C4 },
    {  C4,  0.5f,  0.0f }, {  C4, -0.5f,  0.0f }, { -C4,  0.5f,  0.0f }, { -C4, -0.5f,  0.0f },
    { 0.0f,  C4,  0.5f }, { 0.0f,  C4, -0.5f }, { 0.0f, -C4,  0.5f }, { 0.0f, -C4, -0.5f },
    { 1.0f,  C0,  C3 }, { 1.0f,  C0, -C3 }, { 1.0f, -C0,  C3 }, { 1.0f, -C0, -C3 },
    { -1.0f,  C0,  C3 }, { -1.0f,  C0, -C3 }, { -1.0f, -C0,  C3 }, { -1.0f, -C0, -C3 },
    {  C3,  1.0f,  C0 }, {  C3,  1.0f, -C0 }, {  C3, -1.0f,  C0 }, {  C3, -1.0f, -C0 },
    { -C3,  1.0f,  C0 }, { -C3,  1.0f, -C0 }, { -C3, -1.0f,  C0 }, { -C3, -1.0f, -C0 },
    {  C0,  C3,  1.0f }, {  C0,  C3, -1.0f }, {  C0, -C3,  1.0f }, {  C0, -C3, -1.0f },
    { -C0,  C3,  1.0f }, { -C0,  C3, -1.0f }, { -C0, -C3,  1.0f }, { -C0, -C3, -1.0f },
    { 0.5f,  C1,  C2 }, { 0.5f,  C1, -C2 }, { 0.5f, -C1,  C2 }, { 0.5f, -C1, -C2 },
    { -0.5f,  C1,  C2 }, { -0.5f,  C1, -C2 }, { -0.5f, -C1,  C2 }, { -0.5f, -C1, -C2 },
    {  C2,  0.5f,  C1 }, {  C2,  0.5f, -C1 }, {  C2, -0.5f,  C1 }, {  C2, -0.5f, -C1 },
    { -C2,  0.5f,  C1 }, { -C2,  0.5f, -C1 }, { -C2, -0.5f,  C1 }, { -C2, -0.5f, -C1 },
    {  C1,  C2,  0.5f }, {  C1,  C2, -0.5f }, {  C1, -C2,  0.5f }, {  C1, -C2, -0.5f },
    { -C1,  C2,  0.5f }, { -C1,  C2, -0.5f }, { -C1, -C2,  0.5f }, { -C1, -C2, -0.5f }
};

// 32 faces defined by 5 or 6 vertices (but we only need the edges)
static const int faces[][6] = {
    { 0,  2, 18, 42, 38, 14}, { 1,  3, 17, 41, 37, 13},
    { 2,  0, 12, 36, 40, 16}, { 3,  1, 15, 39, 43, 19},
    { 4,  5, 23, 47, 45, 21}, { 5,  4, 20, 44, 46, 22},
    { 6,  7, 26, 50, 48, 24}, { 7,  6, 25, 49, 51, 27},
    { 8,  9, 33, 57, 56, 32}, { 9,  8, 28, 52, 53, 29},
    {10, 11, 31, 55, 54, 30}, {11, 10, 34, 58, 59, 35},
    {12, 44, 20, 52, 28, 36}, {13, 37, 29, 53, 21, 45},
    {14, 38, 30, 54, 22, 46}, {15, 47, 23, 55, 31, 39},
    {16, 40, 32, 56, 24, 48}, {17, 49, 25, 57, 33, 41},
    {18, 50, 26, 58, 34, 42}, {19, 43, 35, 59, 27, 51},
    { 0, 14, 46, 44, 12, -1}, { 1, 13, 45, 47, 15, -1},
    { 2, 16, 48, 50, 18, -1}, { 3, 19, 51, 49, 17, -1},
    { 4, 21, 53, 52, 20, -1}, { 5, 22, 54, 55, 23, -1},
    { 6, 24, 56, 57, 25, -1}, { 7, 27, 59, 58, 26, -1},
    { 8, 32, 40, 36, 28, -1}, { 9, 29, 37, 41, 33, -1},
    {10, 30, 38, 42, 34, -1}, {11, 35, 43, 39, 31, -1}
};

void generate_soccer_ball(vec3_t** out_vertices, int* out_vertex_count, int (**out_edges)[2], int* out_edge_count) {
    int vertex_count = 60;

    // Normalize all vertices
    vec3_t* v_copy = malloc(sizeof(vec3_t) * vertex_count);
    for (int i = 0; i < vertex_count; ++i)
        v_copy[i] = vec3_normalize_fast(raw_vertices[i]);

    // Collect all unique edges from faces
    int edge_flags[60][60] = {0}; // avoid duplicates
    int max_edges = 180;          // generous allocation
    int (*e_copy)[2] = malloc(sizeof(int[2]) * max_edges);
    int edge_count = 0;

    for (int f = 0; f < sizeof(faces)/sizeof(faces[0]); ++f) {
        for (int i = 0; i < 6; ++i) {
            int a = faces[f][i];
            int b = faces[f][(i + 1) % 6];
            if (a == -1 || b == -1) break;

            if (!edge_flags[a][b] && !edge_flags[b][a]) {
                e_copy[edge_count][0] = a;
                e_copy[edge_count][1] = b;
                edge_flags[a][b] = edge_flags[b][a] = 1;
                edge_count++;
            }
        }
    }

    *out_vertices = v_copy;
    *out_vertex_count = vertex_count;
    *out_edges = e_copy;
    *out_edge_count = edge_count;
}
