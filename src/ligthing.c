#include "lighting.h"
#include <math.h>
#include <stdlib.h>

// Lambert lighting calculation
float calculate_lambert_lighting(vec3_t edge_dir, vec3_t light_dir) {
    float dot_product = vec3_dot(vec3_normalize(edge_dir), vec3_normalize(light_dir));
    return fmaxf(0.0f, dot_product);
}

// Calculate lighting for an edge
float calculate_edge_lighting(vec3_t v1, vec3_t v2, light_system_t *lights) {
    vec3_t edge_dir = vec3_normalize(vec3_sub(v2, v1));
    vec3_t edge_midpoint = vec3_scale(vec3_add(v1, v2), 0.5f);
    
    float total_intensity = 0.0f;
    
    for (int i = 0; i < lights->count; i++) {
        vec3_t light_dir = vec3_normalize(vec3_sub(lights->lights[i].position, edge_midpoint));
        float intensity = calculate_lambert_lighting(edge_dir, light_dir);
        total_intensity += intensity * lights->lights[i].intensity;
    }
    
    return fminf(1.0f, total_intensity);
}

// Light system management
light_system_t* create_light_system(int max_lights) {
    light_system_t *system = malloc(sizeof(light_system_t));
    system->lights = malloc(max_lights * sizeof(light_t));
    system->count = 0;
    return system;
}

void add_light(light_system_t *system, vec3_t position, vec3_t direction, float intensity, vec3_t color) {
    light_t *light = &system->lights[system->count++];
    light->position = position;
    light->direction = direction;
    light->intensity = intensity;
    light->color = color;
}

void free_light_system(light_system_t *system) {
    free(system->lights);
    free(system);
