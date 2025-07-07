#ifndef CANVAS_H
#define CANVAS_H

typedef struct {
    int width, height;
    float **pixels;
} canvas_t;

// Canvas management functions
canvas_t* canvas_create(int width, int height);
void canvas_destroy(canvas_t* canvas);
void canvas_clear(canvas_t* canvas);    // Clear canvas to black/zero
void canvas_save_ppm(canvas_t* canvas, const char* filename);

// Drawing functions
// Uses bilinear filtering to spread intensity across nearby 4 pixels
void set_pixel_f(canvas_t* canvas, float x, float y, float intensity);
// Uses DDA algorithm with thickness support
void draw_line_f(canvas_t* canvas, float x0, float y0, float x1, float y1, float thickness);

#endif