#include "animation.h"
#include <stdlib.h>
#include <math.h>

// Cubic Bézier curve interpolation
vec3_t vec3_bezier(vec3_t p0, vec3_t p1, vec3_t p2, vec3_t p3, float t) {
    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;
    
    vec3_t result;
    result.x = uuu * p0.x + 3 * uu * t * p1.x + 3 * u * tt * p2.x + ttt * p3.x;
    result.y = uuu * p0.y + 3 * uu * t * p1.y + 3 * u * tt * p2.y + ttt * p3.y;
    result.z = uuu * p0.z + 3 * uu * t * p1.z + 3 * u * tt * p2.z + ttt * p3.z;
    
    return result;
}

// Create Bézier curve
bezier_curve_t create_bezier_curve(vec3_t p0, vec3_t p1, vec3_t p2, vec3_t p3, float duration) {
    bezier_curve_t curve;
    curve.p0 = p0;
    curve.p1 = p1;
    curve.p2 = p2;
    curve.p3 = p3;
    curve.duration = duration;
    curve.current_time = 0.0f;
    curve.loop = 1;
    return curve;
}

// Animation system management
animation_system_t* create_animation_system(int max_curves) {
    animation_system_t *system = malloc(sizeof(animation_system_t));
    system->curves = malloc(max_curves * sizeof(bezier_curve_t));
    system->count = 0;
    system->global_time = 0.0f;
    return system;
}

void add_bezier_curve(animation_system_t *system, bezier_curve_t curve) {
    system->curves[system->count++] = curve;
}

void update_animation_system(animation_system_t *system, float delta_time) {
    system->global_time += delta_time;
    
    for (int i = 0; i < system->count; i++) {
        system->curves[i].current_time += delta_time;
        
        if (system->curves[i].loop && system->curves[i].current_time > system->curves[i].duration) {
            system->curves[i].current_time = 0.0f;
        }
    }
}

vec3_t get_bezier_position(bezier_curve_t *curve, float time) {
    float t = time / curve->duration;
    if (t > 1.0f) t = 1.0f;
    if (t < 0.0f) t = 0.0f;
    
    return vec3_bezier(curve->p0, curve->p1, curve->p2, curve->p3, t);
}

void free_animation_system(animation_system_t *system) {
    free(system->curves);
    free(system);
}

// ============================================================================
// ENHANCED RENDERER WITH LIGHTING
// ============================================================================

// Enhanced wireframe renderer with lighting
void render_wireframe_lit(canvas_t *canvas, mesh_t *mesh, camera_t *camera, 
                         light_system_t *lights, z_buffer_t *zbuffer) {
    // Create transformation matrices
    mat4_t model = mat4_identity();
    model = mat4_mul(model, mat4_translate(mesh->position.x, mesh->position.y, mesh->position.z));
    model = mat4_mul(model, mat4_rotate_xyz(mesh->rotation.x, mesh->rotation.y, mesh->rotation.z));
    model = mat4_mul(model, mat4_scale(mesh->scale.x, mesh->scale.y, mesh->scale.z));
    
    mat4_t view = create_view_matrix(camera);
    mat4_t projection = create_projection_matrix(camera);
    mat4_t mvp = mat4_mul(mat4_mul(projection, view), model);
    
    // Project all vertices
    vec3_t *projected_vertices = malloc(mesh->vertex_count * sizeof(vec3_t));
    vec3_t *world_vertices = malloc(mesh->vertex_count * sizeof(vec3_t));
    
    for (int i = 0; i < mesh->vertex_count; i++) {
        // Transform to world space for lighting calculations
        vec4_t world_pos = mat4_mul_vec4(model, (vec4_t){mesh->vertices[i].x, mesh->vertices[i].y, mesh->vertices[i].z, 1.0f});
        world_vertices[i] = (vec3_t){world_pos.x, world_pos.y, world_pos.z};
        
        // Project to screen space
        projected_vertices[i] = project_vertex(mesh->vertices[i], mvp, canvas->width, canvas->height);
    }
    
    // Draw edges with lighting
    for (int i = 0; i < mesh->edge_count; i++) {
        int v1_idx = mesh->indices[i * 2];
        int v2_idx = mesh->indices[i * 2 + 1];
        
        vec3_t v1_screen = projected_vertices[v1_idx];
        vec3_t v2_screen = projected_vertices[v2_idx];
        
        // Calculate lighting intensity
        float intensity = calculate_edge_lighting(world_vertices[v1_idx], world_vertices[v2_idx], lights);
        
        // Clip to circular viewport
        if (clip_to_circular_viewport(canvas, v1_screen.x, v1_screen.y) || 
            clip_to_circular_viewport(canvas, v2_screen.x, v2_screen.y)) {
            
            // Draw line with lighting intensity
            draw_line_f_intensity(canvas, v1_screen.x, v1_screen.y, v2_screen.x, v2_screen.y, 1.0f, intensity);
        }
    }
    
    free(projected_vertices);
    free(world_vertices);
}
