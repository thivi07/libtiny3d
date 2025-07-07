#include "canvas.h"
#include <math.h>
#ifndef MATH3D_H
#define MATH3D_H
#ifndef M_PI
#define M_PI 3.14159265358979323846


int main() {
    // Create a 400x400 canvas
    canvas_t* canvas = canvas_create(400, 400);
    
    // Center of canvas
    float center_x = 200.0f;
    float center_y = 200.0f;
    float radius = 150.0f;
    
    // Draw lines every 15 degrees (like clock face)
    for(int angle = 0; angle < 360; angle += 15) {
        float rad = angle * M_PI / 180.0f;  // Convert to radians
        
        // Calculate end point
        float end_x = center_x + radius * cos(rad);
        float end_y = center_y + radius * sin(rad);
        
        // Draw line from center to edge
        draw_line_f(canvas, center_x, center_y, end_x, end_y, 2.0f);
    }
    
    // Save or display canvas somehow
    canvas_save_ppm(canvas, "clock_demo.ppm");
    
    // Clean up
    canvas_destroy(canvas);
    return 0;

    // Soccer Ball Rendering //
    int width=400, height=400
    canvas_t* soccer_canvas = canvas_create(width, height);
    if (!soccer_canvas) {
        fprintf(stderr, "Failed to create soccer canvas\n");
        return 1;
    }

    vec3_t* soccer_vertices = NULL;
    int soccer_vertex_count = 0;
    int (*soccer_edges)[2] = NULL;
    int soccer_edge_count = 0;

    generate_soccer_ball(&soccer_vertices, &soccer_vertex_count, &soccer_edges, &soccer_edge_count);

    
    float aspect_ratio = (float)width / height;
    mat4_t soccer_proj = mat4_frustum_asymmetric(-aspect_ratio, aspect_ratio, -1.0f, 1.0f, 1.0f, 10.0f);
    mat4_t soccer_model = mat4_identity(); // no rotation
    mat4_t soccer_view = mat4_translate(0.0f, 0.0f, -4.0f);
    
    
    render_wireframe(soccer_canvas, soccer_vertices, soccer_vertex_count, soccer_edges, soccer_edge_count,soccer_model, soccer_view, soccer_proj);
    
    char filename[64];
    for (int frame = 0; frame < 60; ++frame) {
        float angle = frame * (2.0f * M_PI / 60);  // One full rotation
        mat4_t soccer_model = mat4_rotate_xyz(0.0f, angle, 0.0f);  // Y-axis rotation

        canvas_t* frame_canvas = create_canvas(width, height);
        render_wireframe(frame_canvas, soccer_vertices, soccer_vertex_count,
                        soccer_edges, soccer_edge_count, soccer_model, soccer_view, soccer_proj);

        sprintf(filename, "soccer_%03d.pgm", frame);
        canvas_save(frame_canvas, filename);
        canvas_free(frame_canvas);
    }
    canvas_save(soccer_canvas, "soccer.pgm");
    printf("Soccer ball saved to soccer.pgm\n");

    free(soccer_vertices);
    free(soccer_edges);
    canvas_free(soccer_canvas);


    return 0;
}