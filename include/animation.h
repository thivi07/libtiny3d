#ifndef ANIMATION_H
#define ANIMATION_H

#include "math3d.h"

typedef struct {
    vec3_t p0, p1, p2, p3;  // Control points
    float duration;
    float current_time;
    int loop;
} bezier_curve_t;

typedef struct {
    bezier_curve_t *curves;
    int count;
    float global_time;
} animation_system_t;

// Function declarations
vec3_t vec3_bezier(vec3_t p0, vec3_t p1, vec3_t p2, vec3_t p3, float t);
bezier_curve_t create_bezier_curve(vec3_t p0, vec3_t p1, vec3_t p2, vec3_t p3, float duration);
animation_system_t* create_animation_system(int max_curves);
void add_bezier_curve(animation_system_t *system, bezier_curve_t curve);
void update_animation_system(animation_system_t *system, float delta_time);
vec3_t get_bezier_position(bezier_curve_t *curve, float time);
void free_animation_system(animation_system_t *system);

#endif
