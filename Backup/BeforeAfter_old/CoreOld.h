#ifndef COREOLD_H
#define COREOLD_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 
 * Standard Libraries
 */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdatomic.h>


/**
 * Compiler Hints 
 */
#if defined(__GNUC__) || defined(__clang__)
    #define CORE_INLINE static inline __attribute__((always_inline))
    #define CORE_LIKELY(x) __builtin_expect(!!(x), 1)
    #define CORE_UNLIKELY(x) __builtin_expect(!!(x), 0)
    #define CORE_RESTRICT __restrict__
#else   
    #define CORE_INLINE static inline
    #define CORE_LIKELY(x) (x)
    #define CORE_UNLIKELY(x) (x)
    #define CORE_RESTRICT
#endif

/**
 * Basic Types
 */
typedef uint32_t    core_u32; // 32 bit unsigned integer
typedef int32_t     core_i32;  // 32 bit integer
typedef float       core_f32;  // 32 bit float
typedef bool        core_bool;  // boolean

typedef struct {
    core_u32 index;
    core_u32 generation;
}core_entity_id;

#define CORE_INVALID_ENTITY (core_entity_id){0, 0}

/**
 * Vector Math (2d core gameplay)
 */
typedef struct{
    core_f32 x;
    core_f32 y;
} core_vec2;

/**
 * State Flags
 */

typedef core_u32 core_state_flags;

enum {
    CORE_CAN_MOVE       =   1 << 0,
    CORE_CAN_ACT        =   1 << 1,
    CORE_CAN_ATTACK     =   1 << 2,
    CORE_CAN_DEFEND     =   1 << 3,
    
    CORE_IS_FROZEN      =   1 << 4,
    CORE_IS_STUNNED     =   1 << 5,
    CORE_IS_ON_FIRE     =   1 << 6,

    CORE_SILENCED       =  1 << 7,
    CORE_LOCKED         =  1 << 8,
    CORE_INVULNERABLE    =  1 << 9
};

/**
 * Zeals (stats)
 */

typedef enum {
    CORE_ZEAL_NONE = 0,
    CORE_ZEAL_FIRE,
    CORE_ZEAL_ENRAGED,
    CORE_ZEAL_FROZEN,
    CORE_ZEAL_POISONED,
    CORE_ZEAL_BURNING,
    CORE_ZEAL_PATHETIC,
    CORE_ZEAL_BLESSED,
    CORE_ZEAL_CURSED,
    CORE_ZEAL_PATIENCE,
}core_zeal;

typedef enum{
    CORE_EFFECT_DAMAGE = 0,
    CORE_EFFECT_HEAL,
    CORE_EFFECT_SHIELD,
    CORE_EFFECT_STUN,
    CORE_EFFECT_BUFF,
} core_effect_type;

typedef struct {
    core_effect_type type;

    union{
        struct {core_i32 amount;}damage;
        struct {core_i32 amount;}heal;
        struct {core_i32 duration;}stun;
        struct {core_i32 value;}shield;
        struct {core_i32 stat_id; core_i32 value;}buff;
    }data;
}core_effect;


/***
 * 
 * Stats Cache
 */
typedef struct{
    core_i32 hp;
    core_i32 max_hp;
    core_i32 attack;
    core_i32 defense;
    core_i32 speed;
}core_stats;

/**
 * Combo system
 */
typedef struct{
    core_i32 points;
    core_i32 multiplier;
}core_combo;


/**
 * Entity
 */

typedef struct{
    core_entity_id id;

    core_vec2 position;
    core_vec2 velocity;

    core_stats stats;

    core_state_flags state;
    core_zeal zeal;
    
    core_combo combo;
    core_effect active_effect;
}core_entity;


/***
 * World Simulator
 */
typedef struct{
    core_u32 tick;

    core_entity* CORE_RESTRICT entities;
    core_u32 entity_count;
    core_u32 capacity;
}core_world;

/**
 * Input System
 */

typedef struct {
    core_entity_id entity_id;
    core_i32 move_x;
    core_i32 move_y;

    core_bool attack;
    core_bool defend;
    core_bool interact;
    core_bool use_card;
}core_input;

/**
 * Math Helpers
 */

CORE_INLINE core_f32 core_clamp(core_f32 x, core_f32 a, core_f32 b){
    return (x < a) ? a : (x > b ? b : x);
}

CORE_INLINE core_f32 core_lerp(core_f32 a, core_f32 b, core_f32 t){
    return a + (b - a) * t; 
}

/**
 * Handle Validation Idea
 */




#ifdef __cplusplus
}
#endif

#endif