#include "renderer.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

// Project a 3D vertex to screen coordinates
screen_coord_t project_vertex(vec3_t vertex, mat4_t mvp_matrix, int canvas_width, int canvas_height) {
    screen_coord_t result;
    
    // Transform vertex to homogeneous coordinates
    vec4_t homogeneous = {vertex.x, vertex.y, vertex.z, 1.0f};
    
    // Apply MVP transformation
    vec4_t transformed = mat4_multiply_vec4(mvp_matrix, homogeneous);
    
    // Perspective division
    if (transformed.w != 0.0f) {
        transformed.x /= transformed.w;
        transformed.y /= transformed.w;
        transformed.z /= transformed.w;
    }
    
    // Convert to screen coordinates
    result.x = (transformed.x + 1.0f) * 0.5f * canvas_width;
    result.y = (1.0f - transformed.y) * 0.5f * canvas_height;  // Flip Y axis
    result.z = transformed.z;  // Keep depth for Z-buffering
    
    return result;
}

// Check if point is inside circular viewport
int clip_to_circular_viewport(canvas_t* canvas, float x, float y) {
    if (!canvas) return 0;
    
    float center_x = canvas->width * 0.5f;
    float center_y = canvas->height * 0.5f;
    float radius = (canvas->width < canvas->height ? canvas->width : canvas->height) * 0.4f;
    
    float dx = x - center_x;
    float dy = y - center_y;
    float distance_squared = dx * dx + dy * dy;
    
    return distance_squared <= radius * radius;
}

// Create view matrix from camera
mat4_t create_view_matrix(camera_t* camera) {
    vec3_t forward = vec3_subtract(camera->target, camera->position);
    forward = vec3_normalize(forward);
    
    vec3_t right = vec3_cross(forward, camera->up);
    right = vec3_normalize(right);
    
    vec3_t up = vec3_cross(right, forward);
    
    mat4_t view = mat4_identity();
    
    // Set rotation part
    view.m[0][0] = right.x;   view.m[0][1] = up.x;   view.m[0][2] = -forward.x;
    view.m[1][0] = right.y;   view.m[1][1] = up.y;   view.m[1][2] = -forward.y;
    view.m[2][0] = right.z;   view.m[2][1] = up.z;   view.m[2][2] = -forward.z;
    
    // Set translation part
    view.m[0][3] = -vec3_dot(right, camera->position);
    view.m[1][3] = -vec3_dot(up, camera->position);
    view.m[2][3] = vec3_dot(forward, camera->position);
    
    return view;
}

// Create projection matrix
mat4_t create_projection_matrix(camera_t* camera) {
    return mat4_frustum_asymmetric(
        -camera->aspect_ratio * tanf(camera->fov * 0.5f),
        camera->aspect_ratio * tanf(camera->fov * 0.5f),
        -tanf(camera->fov * 0.5f),
        tanf(camera->fov * 0.5f),
        camera->near_plane,
        camera->far_plane
    );
}

// Compare function for sorting edges by depth
int compare_edges(const void* a, const void* b) {
    edge_t* edge_a = (edge_t*)a;
    edge_t* edge_b = (edge_t*)b;
    
    if (edge_a->depth > edge_b->depth) return -1;  // Back to front
    if (edge_a->depth < edge_b->depth) return 1;
    return 0;
}

// Sort edges by depth for proper rendering order
void sort_edges_by_depth(edge_t* edges, int count) {
    qsort(edges, count, sizeof(edge_t), compare_edges);
}

// Logarithmic depth buffer functions
float linear_to_log_depth(float linear_depth, float near, float far) {
    return logf(linear_depth / near) / logf(far / near);
}

float log_to_linear_depth(float log_depth, float near, float far) {
    return near * powf(far / near, log_depth);
}

// Render wireframe object
void render_wireframe(canvas_t* canvas, object3d_t* obj, camera_t* camera) {
    if (!canvas || !obj || !camera) return;
    
    // Create matrices
    mat4_t view = create_view_matrix(camera);
    mat4_t projection = create_projection_matrix(camera);
    mat4_t mvp = mat4_multiply(projection, mat4_multiply(view, obj->transform));
    
    // Project all vertices
    screen_coord_t* projected = malloc(obj->vertex_count * sizeof(screen_coord_t));
    if (!projected) return;
    
    for (int i = 0; i < obj->vertex_count; i++) {
        projected[i] = project_vertex(obj->vertices[i], mvp, canvas->width, canvas->height);
    }
    
    // Calculate edge depths and sort
    edge_t* sorted_edges = malloc(obj->edge_count * sizeof(edge_t));
    if (!sorted_edges) {
        free(projected);
        return;
    }
    
    for (int i = 0; i < obj->edge_count; i++) {
        sorted_edges[i] = obj->edges[i];
        // Calculate average depth of edge endpoints
        float depth1 = projected[obj->edges[i].v1].z;
        float depth2 = projected[obj->edges[i].v2].z;
        sorted_edges[i].depth = (depth1 + depth2) * 0.5f;
    }
    
    sort_edges_by_depth(sorted_edges, obj->edge_count);
    
    // Render edges back to front
    for (int i = 0; i < obj->edge_count; i++) {
        int v1 = sorted_edges[i].v1;
        int v2 = sorted_edges[i].v2;
        
        screen_coord_t p1 = projected[v1];
        screen_coord_t p2 = projected[v2];
        
        // Check if both endpoints are within circular viewport
        if (clip_to_circular_viewport(canvas, p1.x, p1.y) ||
            clip_to_circular_viewport(canvas, p2.x, p2.y)) {
            
            // Calculate line intensity based on depth
            float depth_factor = 1.0f - (sorted_edges[i].depth + 1.0f) * 0.5f;
            depth_factor = depth_factor < 0.1f ? 0.1f : depth_factor;
            depth_factor = depth_factor > 1.0f ? 1.0f : depth_factor;
            
            // Draw the line
            draw_line_f(canvas, p1.x, p1.y, p2.x, p2.y, 1.0f, depth_factor);
        }
    }
    
    free(projected);
    free(sorted_edges);
}

// Generate soccer ball (truncated icosahedron)
object3d_t* generate_soccer_ball(void) {
    object3d_t* obj = malloc(sizeof(object3d_t));
    if (!obj) return NULL;
    
    // Soccer ball has 60 vertices and 90 edges
    obj->vertex_count = 60;
    obj->edge_count = 90;
    obj->vertices = malloc(obj->vertex_count * sizeof(vec3_t));
    obj->edges = malloc(obj->edge_count * sizeof(edge_t));
    
    if (!obj->vertices || !obj->edges) {
        free_object3d(obj);
        return NULL;
    }
    
    // Golden ratio for icosahedron construction
    float phi = (1.0f + sqrtf(5.0f)) / 2.0f;
    float scale = 1.0f;
    
    // Create vertices for truncated icosahedron
    // This is a simplified version - in practice, you'd generate proper soccer ball geometry
    int vertex_idx = 0;
    
    // Generate vertices using icosahedral symmetry
    for (int i = 0; i < 12; i++) {
        float angle = 2.0f * M_PI * i / 12.0f;
        for (int j = 0; j < 5; j++) {
            float r = 1.0f + 0.3f * sinf(5.0f * angle);
            float theta = angle + 0.2f * j;
            float z = 0.5f * cosf(2.0f * angle);
            
            if (vertex_idx < obj->vertex_count) {
                obj->vertices[vertex_idx].x = r * cosf(theta) * scale;
                obj->vertices[vertex_idx].y = r * sinf(theta) * scale;
                obj->vertices[vertex_idx].z = z * scale;
                vertex_idx++;
            }
        }
    }
    
    // Generate edges - connect nearby vertices
    int edge_idx = 0;
    for (int i = 0; i < obj->vertex_count && edge_idx < obj->edge_count; i++) {
        for (int j = i + 1; j < obj->vertex_count && edge_idx < obj->edge_count; j++) {
            vec3_t diff = vec3_subtract(obj->vertices[i], obj->vertices[j]);
            float distance = vec3_length(diff);
            
            // Connect vertices that are close enough
            if (distance < 0.8f) {
                obj->edges[edge_idx].v1 = i;
                obj->edges[edge_idx].v2 = j;
                obj->edges[edge_idx].depth = 0.0f;
                edge_idx++;
            }
        }
    }
    
    obj->edge_count = edge_idx;  // Update to actual edge count
    obj->transform = mat4_identity();
    
    return obj;
}

// Free object3d memory
void free_object3d(object3d_t* obj) {
    if (!obj) return;
    
    free(obj->vertices);
    free(obj->edges);
    free(obj);
}