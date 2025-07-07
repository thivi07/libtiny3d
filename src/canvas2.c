#include "canvas.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

canvas_t* create_canvas(int width, int height) {
    canvas_t* canvas = malloc(sizeof(canvas_t));
    canvas->width = width;
    canvas->height = height;
    
    // Allocate array of row pointers
    canvas->pixels = malloc(height * sizeof(float*));
    
    // Allocate each row
    for(int y = 0; y < height; y++) {
        canvas->pixels[y] = malloc(width * sizeof(float));
        // Initialize all pixels to 0.0 (black)
        for(int x = 0; x < width; x++) {
            canvas->pixels[y][x] = 0.0f;
        }
    }
    return canvas;
}

void set_pixel_f(canvas_t* canvas, float x, float y, float intensity) {
    // Integer coordinates of surrounding pixels
    int x0 = (int)floor(x);
    int x1 = x0 + 1;
    int y0 = (int)floor(y);
    int y1 = y0 + 1;

    // Calculate fractional parts
    float fx = x - x0;
    float fy = y - y0;

    // Calculate weights for bilinear interpolation
    float w00 = (1.0f - fx) * (1.0f - fy);
    float w01 = fx * (1.0f - fy);
    float w10 = (1.0f - fx) * fy;
    float w11 = fx * fy;

    // Apply intensity to surrounding pixels with boundary checks
    if(x0 >= 0 && x0 < canvas->width && y0 >= 0 && y0 < canvas->height)
        canvas->pixels[y0][x0] += intensity * w00;
    if(x1 >= 0 && x1 < canvas->width && y0 >= 0 && y0 < canvas->height)
        canvas->pixels[y0][x1] += intensity * w01;
    if(x0 >= 0 && x0 < canvas->width && y1 >= 0 && y1 < canvas->height)
        canvas->pixels[y1][x0] += intensity * w10;
    if(x1 >= 0 && x1 < canvas->width && y1 >= 0 && y1 < canvas->height)
        canvas->pixels[y1][x1] += intensity * w11;
}

void draw_line_f(canvas_t* canvas, float x0, float y0, float x1, float y1, float thickness) {
    // Calculate line direction and normal
    float dx = x1 - x0;
    float dy = y1 - y0;
    float length = sqrtf(dx*dx + dy*dy);
    
    if (length < 0.0001f) return; // Too short to draw
    
    // Normalize direction
    dx /= length;
    dy /= length;
    
    // Calculate perpendicular direction for thickness
    float nx = -dy;
    float ny = dx;
    
    // DDA algorithm for the line core
    float steps = fmaxf(fabsf(x1 - x0), fabsf(y1 - y0));
    float x_inc = (x1 - x0) / steps;
    float y_inc = (y1 - y0) / steps;
    
    float x = x0;
    float y = y0;
    
    for(int i = 0; i <= steps; i++) {
        // Draw thickness perpendicular to the line direction
        for(float t = -thickness/2; t <= thickness/2; t += 0.5f) {
            float px = x + nx * t;
            float py = y + ny * t;
            set_pixel_f(canvas, px, py, 1.0f);
        }
        
        x += x_inc;
        y += y_inc;
    }
}

void save_canvas_to_file(canvas_t* canvas, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) return;
    
    // PPM P2 format (ASCII grayscale)
    fprintf(file, "P2\n%d %d\n255\n", canvas->width, canvas->height);
    
    for(int y = 0; y < canvas->height; y++) {
        for(int x = 0; x < canvas->width; x++) {
            int pixel_value = (int)(canvas->pixels[y][x] * 255);
            pixel_value = pixel_value > 255 ? 255 : pixel_value; // Clamp to 255
            fprintf(file, "%d ", pixel_value);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

void destroy_canvas(canvas_t* canvas) {
    if(canvas) {
        // Free each row
        for(int y = 0; y < canvas->height; y++) {
            free(canvas->pixels[y]);
        }
        // Free array of row pointers
        free(canvas->pixels);
        // Free the canvas structure itself
        free(canvas);
    }
}