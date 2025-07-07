#include "math3d.h"
#include <stdio.h>
#include <stdbool.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int main() {
    printf("=== Task 2: 3D Math Foundation Demo ===\n\n");
    
    // Test 1: Vector operations
    printf("1. Vector Operations:\n");
    vec3_t v1 = {3.0f, 4.0f, 5.0f, 0, 0, 0};
    vec3_update_spherical(&v1);
    print_vec3(v1);
    
    vec3_t v2 = vec3_from_spherical(10.0f, M_PI/4, M_PI/3);
    print_vec3(v2);
    
    vec3_t normalized = vec3_normalize(v1);
    printf("Normalized: ");
    print_vec3(normalized);
    
    // Test 2: SLERP
    printf("\n2. SLERP Test:\n");
    vec3_t start = {1, 0, 0, 0, 0, 0};
    vec3_t end = {0, 1, 0, 0, 0, 0};
    
    for (float t = 0.0f; t <= 1.0f; t += 0.25f) {
        vec3_t interpolated = vec3_slerp(start, end, t);
        printf("t=%.2f: ", t);
        print_vec3(interpolated);
    }
    
    // Test 3: Matrix transformations on cube
    printf("\n3. Cube Transformation:\n");
    
    // Define cube vertices
    vec3_t cube[8] = {
        // Bottom face (z = -1)
        {-1.0f, -1.0f, -1.0f, 0, 0, 0}, // 0: bottom-left-back
        { 1.0f, -1.0f, -1.0f, 0, 0, 0}, // 1: bottom-right-back
        { 1.0f,  1.0f, -1.0f, 0, 0, 0}, // 2: top-right-back
        {-1.0f,  1.0f, -1.0f, 0, 0, 0}, // 3: top-left-back
        // Top face (z = 1)
        {-1.0f, -1.0f,  1.0f, 0, 0, 0}, // 4: bottom-left-front
        { 1.0f, -1.0f,  1.0f, 0, 0, 0}, // 5: bottom-right-front
        { 1.0f,  1.0f,  1.0f, 0, 0, 0}, // 6: top-right-front
        {-1.0f,  1.0f,  1.0f, 0, 0, 0}  // 7: top-left-front
    };
    
    // Cube edges for wireframe rendering
    int edges[12][2] = {
        {0,1}, {1,2}, {2,3}, {3,0}, // bottom face
        {4,5}, {5,6}, {6,7}, {7,4}, // top face
        {0,4}, {1,5}, {2,6}, {3,7}  // vertical edges
    };

    printf("Cube edges defined: 12 edges connecting 8 vertices\n");
    printf("Bottom face: 0-1-2-3-0\n");
    printf("Top face: 4-5-6-7-4\n");
    printf("Vertical edges: 0-4, 1-5, 2-6, 3-7\n\n");
    
    // Update spherical coordinates for all vertices
    for (int i = 0; i < 8; i++) {
        vec3_update_spherical(&cube[i]);
    }
    
    // Create model transformation matrix (applied in reverse order: TRS)
    mat4_t model = mat4_identity();
    model = mat4_multiply(model, mat4_translate(0, 0, -6));       // First: translate away from camera
    model = mat4_multiply(model, mat4_rotate_y(30 * M_PI/180));   // Then: rotate for better view
    model = mat4_multiply(model, mat4_rotate_x(15 * M_PI/180));   // Add slight X rotation
    model = mat4_multiply(model, mat4_scale(1.2f, 1.2f, 1.2f));  // Finally: scale slightly
    
    printf("Model Transformation Matrix:\n");
    print_mat4(model);
    
    // Apply model transformation to all vertices
    vec3_t transformed_vertices[8];
    printf("\nTransformed Cube Vertices:\n");
    for (int i = 0; i < 8; i++) {
        transformed_vertices[i] = mat4_multiply_vec3(model, cube[i]);
        printf("Vertex %d: (%.2f, %.2f, %.2f)\n", i, 
               transformed_vertices[i].x, transformed_vertices[i].y, transformed_vertices[i].z);
    }
    
    // Create projection matrix with proper parameters
    float fov = 60.0f * M_PI / 180.0f;  // 60 degrees field of view
    float aspect = 1.0f;                 // Square viewport
    float near = 1.0f;                   // Near clipping plane
    float far = 100.0f;                  // Far clipping plane
    
    float tan_half_fov = tan(fov / 2.0f);
    float right = near * tan_half_fov;
    float left = -right;
    float top = right / aspect;
    float bottom = -top;
    
    mat4_t projection = mat4_frustum_asymmetric(left, right, bottom, top, near, far);
    
    printf("\nProjection Matrix (FOV=60°, Near=1, Far=100):\n");
    print_mat4(projection);
    
    // Project vertices to screen coordinates
    printf("\nProjected 2D Screen Coordinates:\n");
    int screen_width = 400;
    int screen_height = 400;
    
    for (int i = 0; i < 8; i++) {
        vec3_t v = transformed_vertices[i];
        
        // Check if vertex is behind the camera (negative Z in view space)
        if (v.z >= -near) {
            printf("Vertex %d: Behind camera (z=%.2f) - skipping\n", i, v.z);
            continue;
        }
        
        // Apply projection matrix
        float x = v.x, y = v.y, z = v.z, w = 1.0f;
        
        float px = projection.m[0]*x + projection.m[4]*y + projection.m[8]*z + projection.m[12]*w;
        float py = projection.m[1]*x + projection.m[5]*y + projection.m[9]*z + projection.m[13]*w;
        float pz = projection.m[2]*x + projection.m[6]*y + projection.m[10]*z + projection.m[14]*w;
        float pw = projection.m[3]*x + projection.m[7]*y + projection.m[11]*z + projection.m[15]*w;
        
        // Check for valid w component
        if (fabs(pw) < 1e-6) {
            printf("Vertex %d: Invalid w component (%.6f) - skipping\n", i, pw);
            continue;
        }
        
        // Perspective divide
        px /= pw;
        py /= pw;
        pz /= pw;
        
        // Check if vertex is within NDC bounds
        if (px < -1.0f || px > 1.0f || py < -1.0f || py > 1.0f) {
            printf("Vertex %d: Outside NDC bounds (%.2f, %.2f) - clipped\n", i, px, py);
        }
        
        // Map to screen space
        int sx = (int)((px + 1.0f) * 0.5f * screen_width);
        int sy = (int)((1.0f - py) * 0.5f * screen_height); // flip Y
        
        // Clamp to screen bounds
        sx = sx < 0 ? 0 : (sx >= screen_width ? screen_width - 1 : sx);
        sy = sy < 0 ? 0 : (sy >= screen_height ? screen_height - 1 : sy);
        
        printf("Vertex %d : NDC(%.2f, %.2f)   Screen: (%d, %d)   Depth: %.2f\n", 
               i, px, py, sx, sy, pz);
    }
    
    // Show wireframe edges
    printf("\nWireframe Edges:\n");
    for (int i = 0; i < 12; i++) {
        int v1_idx = edges[i][0];
        int v2_idx = edges[i][1];
        
        vec3_t v1 = transformed_vertices[v1_idx];
        vec3_t v2 = transformed_vertices[v2_idx];
        
        // Simple visibility check
        bool v1_visible = (v1.z < -near);
        bool v2_visible = (v2.z < -near);
        
        if (v1_visible && v2_visible) {
            printf("Edge %d: Connect vertex %d to vertex %d (visible)\n", i, v1_idx, v2_idx);
        } else if (v1_visible || v2_visible) {
            printf("Edge %d: Connect vertex %d to vertex %d (partially visible)\n", i, v1_idx, v2_idx);
        } else {
            printf("Edge %d: Connect vertex %d to vertex %d (hidden)\n", i, v1_idx, v2_idx);
        }
    }
    
    return 0;
}