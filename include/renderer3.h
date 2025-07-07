#ifndef RENDERER_H
#define RENDERER_H

#include "canvas.h"
#include "math3d.h"

// Screen coordinate structure
typedef struct {
    float x, y;
    float z;  // Depth value for Z-buffering
} screen_coord_t;

// Edge structure for wireframe rendering
typedef struct {
    int v1, v2;  // Vertex indices
    float depth; // Average depth for sorting
} edge_t;

// 3D object structure
typedef struct {
    vec3_t* vertices;
    edge_t* edges;
    int vertex_count;
    int edge_count;
    mat4_t transform;
} object3d_t;

// Camera structure
typedef struct {
    vec3_t position;
    vec3_t target;
    vec3_t up;
    float fov;
    float near_plane;
    float far_plane;
    float aspect_ratio;
} camera_t;

// Function declarations
screen_coord_t project_vertex(vec3_t vertex, mat4_t mvp_matrix, int canvas_width, int canvas_height);
int clip_to_circular_viewport(canvas_t* canvas, float x, float y);
void render_wireframe(canvas_t* canvas, object3d_t* obj, camera_t* camera);
object3d_t* generate_soccer_ball(void);
void free_object3d(object3d_t* obj);

// Helper functions
mat4_t create_view_matrix(camera_t* camera);
mat4_t create_projection_matrix(camera_t* camera);
void sort_edges_by_depth(edge_t* edges, int count);

// Logarithmic Z-buffer helpers
float linear_to_log_depth(float linear_depth, float near, float far);
float log_to_linear_depth(float log_depth, float near, float far);

#endif // RENDERER_H