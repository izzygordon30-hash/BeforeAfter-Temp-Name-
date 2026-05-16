#ifndef CORE_MATH_H
#define CORE_MATH_H

#include "CoreTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================
 * VECTOR MATH TYPES
 * ============================================================ */

typedef struct {
    core_f32 x;
} core_vec1;

typedef struct {
    core_f32 x;
    core_f32 y;
} core_vec2;

typedef struct {
    core_f32 x;
    core_f32 y;
    core_f32 z;
} core_vec3;

typedef struct {
    core_f32 x;
    core_f32 y;
    core_f32 z;
    core_f32 w;
} core_vec4;

typedef struct {
    core_f32 x;
    core_f32 y;
    core_f32 z;
    core_f32 w;
    core_f32 v;
} core_vec5;

/* ============================================================
 * MATH HELPERS
 * ============================================================ */

CORE_INLINE core_f32 core_sqrt(core_f32 n) {
    if (n <= 0.0f) return 0.0f;

    core_f32 x = n;

    for (core_u32 i = 0; i < 8; i++) {
        x = 0.5f * (x + n / x);
    }

    return x;
}

CORE_INLINE core_f32 core_clamp(core_f32 x, core_f32 a, core_f32 b) {
    return (x < a) ? a : (x > b ? b : x);
}

CORE_INLINE core_f32 core_lerp(core_f32 a, core_f32 b, core_f32 t) {
    return a + (b - a) * t;
}

CORE_INLINE core_f32 core_distance_vec2(core_vec2 a, core_vec2 b) {
    core_f32 dx = a.x - b.x;
    core_f32 dy = a.y - b.y;
    return core_sqrt(dx * dx + dy * dy);
}

CORE_INLINE core_f32 core_distance_vec3(core_vec3 a, core_vec3 b) {
    core_f32 dx = a.x - b.x;
    core_f32 dy = a.y - b.y;
    core_f32 dz = a.z - b.z;
    return core_sqrt(dx * dx + dy * dy + dz * dz);
}

#ifdef __cplusplus
}
#endif

#endif /* CORE_MATH_H */