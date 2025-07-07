#include "canvas.h"
#include <math.h>
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
}