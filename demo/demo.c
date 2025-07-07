// Add this to your main.c or create a separate demo file

#include "tiny3d.h"
#include <stdio.h>
#include <math.h>

void demo_task3(void) {
    printf("=== Task 3 Demo: 3D Rendering Pipeline ===\n");
    
    // Create canvas
    canvas_t* canvas = create_canvas(800, 600);
    if (!canvas) {
        printf("Failed to create canvas\n");
        return;
    }
    
    // Generate soccer ball
    object3d_t* soccer_ball = generate_soccer_ball();
    if (!soccer_ball) {
        printf("Failed to generate soccer ball\n");
        destroy_canvas(canvas);
        return;
    }
    
    // Setup camera
    camera_t camera = {
        .position = {0.0f, 0.0f, 5.0f},
        .target = {0.0f, 0.0f, 0.0f},
        .up = {0.0f, 1.0f, 0.0f},
        .fov = M_PI / 3.0f,  // 60 degrees
        .near_plane = 0.1f,
        .far_plane = 100.0f,
        .aspect_ratio = 800.0f / 600.0f
    };
    
    // Animation loop
    int frame_count = 60;
    for (int frame = 0; frame < frame_count; frame++) {
        // Clear canvas
        clear_canvas(canvas);
        
        // Calculate rotation
        float time = (float)frame / frame_count * 2.0f * M_PI;
        float rotation_y = time;
        float rotation_x = time * 0.5f;
        
        // Apply transformations
        mat4_t rotation = mat4_multiply(
            mat4_rotate_xyz(rotation_x, rotation_y, 0.0f),
            mat4_scale(1.5f, 1.5f, 1.5f)
        );
        soccer_ball->transform = rotation;
        
        // Render wireframe
        render_wireframe(canvas, soccer_ball, &camera);
        
        // Save frame (optional - for demonstration)
        char filename[256];
        snprintf(filename, sizeof(filename), "frame_%03d.txt", frame);
        save_canvas_ascii(canvas, filename);
        
        printf("Rendered frame %d/%d\n", frame + 1, frame_count);
    }
    
    // Cleanup
    free_object3d(soccer_ball);
    destroy_canvas(canvas);
    
    printf("Task 3 demo completed!\n");
}

// Test the math pipeline separately
void test_projection_pipeline(void) {
    printf("=== Testing Projection Pipeline ===\n");
    
    // Test vertex
    vec3_t test_vertex = {1.0f, 1.0f, -2.0f};
    
    // Create test matrices
    mat4_t model = mat4_identity();
    mat4_t view = mat4_translate(0.0f, 0.0f, -5.0f);
    mat4_t projection = mat4_frustum_asymmetric(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 100.0f);
    mat4_t mvp = mat4_multiply(projection, mat4_multiply(view, model));
    
    // Project vertex
    screen_coord_t projected = project_vertex(test_vertex, mvp, 800, 600);
    
    printf("Original vertex: (%.2f, %.2f, %.2f)\n", 
           test_vertex.x, test_vertex.y, test_vertex.z);
    printf("Projected to screen: (%.2f, %.2f) depth: %.2f\n", 
           projected.x, projected.y, projected.z);
    
    // Test circular clipping
    int inside = clip_to_circular_viewport(NULL, projected.x, projected.y);
    printf("Inside circular viewport: %s\n", inside ? "Yes" : "No");
}

// Add to your main() function:
/*
int main(void) {
    // Previous task demos...
    
    // Task 3 demo
    test_projection_pipeline();
    demo_task3();
    
    return 0;
}
*/