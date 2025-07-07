#include <stdio.h>
#include "canvas.h"
#include <math.h>
#include <stdlib.h>

canvas_t* canvas_create(int width, int height) {
    if(width <= 0 || height <= 0) return NULL;
    
    canvas_t* canvas = malloc(sizeof(canvas_t));
    if(!canvas) return NULL;

    canvas->width = width;
    canvas->height = height;
    
    // Allocate array of row pointers
    canvas->pixels = malloc(height * sizeof(float*));
    if(!canvas->pixels) {  
        free(canvas);
        return NULL;
    }
    
    // Allocate each row
    for(int y = 0; y < height; y++) {
        canvas->pixels[y] = malloc(width * sizeof(float));
        if(!canvas->pixels[y]) {  // ← Add this check
            // Clean up previously allocated rows
            for(int i = 0; i < y; i++) {
                free(canvas->pixels[i]);
            }
            free(canvas->pixels);
            free(canvas);
            return NULL;
        }
    // Initialize pixels to 0.0f (black)
        for(int x = 0; x < width; x++) {
            canvas->pixels[y][x] = 0.0f;
            }   
    }
    return canvas;
}

void set_pixel_f(canvas_t* canvas, float x, float y, float intensity) {
    int x0 = (int)floor(x);
    int x1 = x0 + 1;
    int y0 = (int)floor(y);
    int y1 = y0 + 1;

    float fx = x - x0;
    float fy = y - y0;

    float w00 = (1.0f - fx) * (1.0f - fy);  // top-left
    float w01 = fx * (1.0f - fy);           // top-right
    float w10 = (1.0f - fx) * fy;           // bottom-left
    float w11 = fx * fy;                    // bottom-right

    if(x0 >= 0 && x0 < canvas->width && y0 >= 0 && y0 < canvas->height)
        canvas->pixels[y0][x0] += intensity * w00;
    if(x1 >= 0 && x1 < canvas->width && y0 >= 0 && y0 < canvas->height)
        canvas->pixels[y0][x1] += intensity * w01;
    if(x0 >= 0 && x0 < canvas->width && y1 >= 0 && y1 < canvas->height)
        canvas->pixels[y1][x0] += intensity * w10;
    if(x1 >= 0 && x1 < canvas->width && y1 >= 0 && y1 < canvas->height)
        canvas->pixels[y1][x1] += intensity * w11;
}

// Consider using integer DDA for better performance:
void draw_line_f(canvas_t* canvas, float x0, float y0, float x1, float y1, float thickness) {
    float dx = x1 - x0;
    float dy = y1 - y0;
    
    int steps = (int)fmax(fabs(dx), fabs(dy));
    if(steps == 0) {
        set_pixel_f(canvas, x0, y0, 1.0f);
        return;
    }
    
    float x_inc = dx / steps;
    float y_inc = dy / steps;
    
    for(int i = 0; i <= steps; i++) {
        float x = x0 + i * x_inc;
        float y = y0 + i * y_inc;
        
        // Simpler thickness implementation
        int thick_pixels = (int)thickness;
        for(int tx = -thick_pixels/2; tx <= thick_pixels/2; tx++) {
            for(int ty = -thick_pixels/2; ty <= thick_pixels/2; ty++) {
                set_pixel_f(canvas, x + tx, y + ty, 1.0f);
            }
        }
    }
}


void canvas_save_ppm(canvas_t* canvas, const char* filename) {
    // Code to save canvas as image file (PPM format is easiest)
    if(!canvas || !filename) return;
    FILE* file = fopen(filename, "w");
    if(!file) {
        printf("Error: Could not open file %s\n", filename);
        return;
        }    
    
    fprintf(file, "P2\n%d %d\n255\n", canvas->width, canvas->height);

    for(int y = 0; y < canvas->height; y++) {
        for(int x = 0; x < canvas->width; x++) {
            int pixel_value = (int)(canvas->pixels[y][x] * 255);
            if(pixel_value > 255) pixel_value = 255;  // Clamp to max
            fprintf(file, "%d ", pixel_value);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

void canvas_destroy(canvas_t* canvas){
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

void canvas_clear(canvas_t* canvas) {
    for(int y = 0; y < canvas->height; y++) {
        for(int x = 0; x < canvas->width; x++) {
            canvas->pixels[y][x] = 0.0f;
        }
    }
}


   
