
#ifndef LIGHTING_H
#define LIGHTING_H

#include "math3d.h"

typedef struct {
    vec3_t position;
    vec3_t direction;
    float intensity;
    vec3_t color;
} light_t;

typedef struct {
    light_t *lights;
    int count;
} light_system_t;

// Function declarations
float calculate_lambert_lighting(vec3_t edge_dir, vec3_t light_dir);
float calculate_edge_lighting(vec3_t v1, vec3_t v2, light_system_t *lights);
light_system_t* create_light_system(int max_lights);
void add_light(light_system_t *system, vec3_t position, vec3_t direction, float intensity, vec3_t color);
void free_light_system(light_system_t *system);

#endif