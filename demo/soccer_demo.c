#include "canvas.h"
#include "math3d.h"
#include "renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
 
int main(){
    int width=400, height=400;
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

        canvas_t* frame_canvas = canvas_create(width, height);
        render_wireframe(frame_canvas, soccer_vertices, soccer_vertex_count,
                        soccer_edges, soccer_edge_count, soccer_model, soccer_view, soccer_proj);

        sprintf(filename, "soccer_%03d.pgm", frame);
        canvas_save_ppm(frame_canvas, filename);
        canvas_destroy(frame_canvas);
    }
    canvas_save_ppm(soccer_canvas, "soccer.pgm");
    printf("Soccer ball saved to soccer.pgm\n");

    free(soccer_vertices);
    free(soccer_edges);
    canvas_destroy(soccer_canvas);


    return 0;
}

#endif