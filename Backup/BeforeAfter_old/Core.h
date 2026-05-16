#ifndef COREOLDE_H
#define COREOLDE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Standard Libraries
 * ---------------------------------------------------------------------
 * Default Imported elements from the C standart library 
 */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define CORE_EXT_NONE 0

#define CORE_INVALID 0

/**
 * Compiler Hints
 * ---------------------------------------------------------------------
 * Tells the Compiler to format the code in a certain way
 * Helps with Optimization
 */

#if defined(__GNUC__) || defined(__clang__)
    #define CORE_INLINE         static inline __attribute__((always_inline))
    #define CORE_LIKELY(x)      __builtin_expect(!!(x), 1)
    #define CORE_UNLIKELY(x)    __builtin_expect(!!(x), 0)
    #define CORE_RESTRICT       __restrict__
#else   
    #define CORE_INLINE         static inline
    #define CORE_LIKELY(x)      (x)
    #define CORE_UNLIKELY(x)    (x)
    #define CORE_RESTRICT
#endif

/**
 * Primative Types
 * ---------------------------------------------------------------------
 * Define the types used for my game/system.
 */
typedef uint8_t core_u8;
typedef uint32_t core_u32;
typedef uint64_t core_u64;

typedef int8_t core_i8;
typedef int32_t core_i32;
typedef int64_t core_i64;

typedef float core_f32;
typedef double core_f64;

typedef bool core_bool;
typedef char* core_string;

/** ## Additional Things
 * Handled by C++
 */
typedef core_u32 core_ext_id;

/**
 * Entity ID
 * @brief Handle for Entity Referencing 
*/
typedef struct {
    /**
     * @brief Slot in the World Storage
     */
    core_u32 index; 
    /**
     * Version Counter (Invalidates Stale/Old/Zombie Handles/References)
     */
    core_u32 generation; 
} core_entity_id;

#define CORE_INVALID_ENTITY (core_entity_id){UINT32_MAX, 0}

/**
 * Vector Math
 * ---------------------------------------------------------
 * @todo Improve Later Might use more variants of mathematics or more(non-math thigs or logic or illogical things)
 */
typedef struct{
    core_f32 x;
    core_f32 y;
} core_vec2;

typedef struct{
    core_f32 x;
    core_f32 y;
    core_f32 z;
} core_vec3;

/**
 * Math Helpers
 * ---------------------------------------------------------
 * Math Helpers, quick tools
 */

/**
 * Clamp 
 * @brief Put in three values (value, min, max)
 * @param x : the Value to Clamp
 * @param a : Minimum value it can be
 * @param b : Maximum value it can be
 * 
 * @returns clamps the value between the preferred minimum and maximum values
 * 
 * \code
 *  eg. speed = clamp(speed, 1, 10) the value will not go below 1 and not above 10
 * \endcode
 */
CORE_INLINE core_f32 core_clamp(core_f32 x, core_f32 a, core_f32 b){
    return (x < a) ? a : (x > b ? b : x);
}

/**
 * @brief Interpolates between 2 given values (`a` and `b`)
 * 
 * Computes a value between `a` and `b` using interpolation factor `t`.
 * 
 * @param a Start value.
 * @param b End value.
 * @param t Interpolation variable.
 * 
 * @return Interpolated value between `a` and `b`.
 * 
 * \code
 *  eg. core_lerp(1.0f, 10.0f, 0.5f) returns 5.5f
 * \endcode
 */
CORE_INLINE core_f32 core_lerp(core_f32 a, core_f32 b, core_f32 t){
    return a + (b - a) * t; 
}

/**
 * @brief Finds the distance between two points in 2D space (`a` and `b`)
 * 
 * @details Minus 10.0 - 12.0 = -2.0| 12.0 - 10.0 = 2.0 | -2.0 and 2.0
 *  Square them -2.0 * 2.0 | 2.0 * 2.0 | 4.0 and 4.0
 *  Add them 4.0 + 4.0 = 8.0f
 * 
 * @param a Point A.
 * @param b Point B
 * 
 * @return The distance between `a` and `b` in 2D space.
 * 
 * \code
 *  eg. core_distance_vec2(`(10.0, 12.0), (12.0, 10.0)`) returns `8.0f`
 * \endcode
 */
CORE_INLINE core_f32 core_distance_vec2(core_vec2 a, core_vec2 b){
    core_f32 dx = a.x - b.x;
    core_f32 dy = a.y - b.y;
    return dx * dx + dy * dy;
}



/**
 * @brief Finds the distance between three points in 3D space (`a` and `b`)
 * 
 * // Minus the Values with the coresponding dimensions eg. x10 - x12
 * // Square the differences eg. x10 - 12 is -2 square it its 4.0f.
 * // Sum them all up| x = 4.0 | y = 4.0 | x = 0.0
 * @param a Point A.
 * @param b Point B.
 * 
 * @return The distance between `a` and `b` in 3D space.
 * 
 * \code
 *  eg. core_distance_vec2(`(10.0, 12.0, 10.0), (12.0, 10.0, 10.0)`) returns `8.0f`
 * \endcode
 */
CORE_INLINE core_f32 core_distance_vec3(core_vec3 a, core_vec3 b){
    core_f32 dx = a.x - b.x;
    core_f32 dy = a.y - b.y;
    core_f32 dz = a.z - b.z;
    return dx * dx + dy * dy + dz * dz;
}

/**
 * Enums - 
 * Rarity
 * Gender
 * Size
 * Species
 * Classes
 * Zeals
 * Effects
 * ---------------------------------------------------------
 * Types in the game
 */


/**
 * Item Type
 */

typedef enum {
    CORE_ITEM_NONE = 0,
    CORE_ITEM_CONSUMABLE,
    CORE_ITEM_EQUIPMENT,
    CORE_ITEM_KEY,
    CORE_ITEM_MATERIAL,
    CORE_ITEM_CARD
} core_item_type;

/**
 * Rarity of the Character
 * Common, Uncommon, Rare, Legendary and Superb
 */
typedef enum {
    CORE_RARITY_COMMON = 0,
    CORE_RARITY_UNCOMMON,
    CORE_RARITY_RARE,
    CORE_RARITY_LEGENDARY,
    CORE_RARITY_SUPERB
} core_rarity;

/**
 * Gender of the Character
 * None, Male, Female, Other.
 */
typedef enum {
    CORE_GENDER_NONE = 0,
    CORE_GENDER_MALE,
    CORE_GENDER_FEMALE,
    CORE_GENDER_OTHER,
} core_gender;

/**
 * Size of the Character
 * Small, medium, large, bigback
 */
typedef enum {
    CORE_SIZE_SMALL = 0,
    CORE_SIZE_MEDIUM,
    CORE_SIZE_LARGE,
    CORE_SIZE_BIGBACK,
} core_size;

/**
 * Species of the Character
 * ---------------------------------------------------------------------------------
 * None, Human, Humanoid, Alien, Tech, NotTech, Animal, Beast,  Construct, Unknown
 */
typedef core_u32 core_species_id;
#define CORE_SPECIES_NONE 0


/**
 * Classes of the Character and Relationships
 * ---------------------------------------------------------------------------------
 * Role/Archetype
 */
typedef core_u32 core_class_id;
#define CORE_CLASS_NONE 0



typedef enum {
    CORE_BOND_NONE = 0,
    CORE_BOND_FRIENDSHIP,
    CORE_BOND_RIVALRY,
    CORE_BOND_SQUAD,
    CORE_BOND_FORBIDDEN,
} core_bond_type;

/**
 * BOND
 * Team-up slot. When paried with matching partner_class or relationship,
 * they get special benefits.
 */
#define CORE_BOND_PARTNER_MAX 8

typedef struct {
    core_u32 ids[CORE_BOND_PARTNER_MAX];
    core_u32 count; /* total known to C++ — may exceed CORE_BOND_PARTNER_MAX */
} core_bond_partner_pool;

#define CORE_RIVAL_POOL_MAX 8

typedef struct {
    core_u32 ids[CORE_RIVAL_POOL_MAX];
    core_u32 count; /* total known to C++ — may exceed CORE_RIVAL_POOL_MAX */
} core_rival_pool;


typedef struct {
    core_bond_type type;
    core_class_id trigger; // What Characters are paried up together.
    core_bond_partner_pool partners;
    core_rival_pool rival_id;
    core_u32 special_given; // What Special Zeal does the character(s) gain
    core_u32 effect_given; // What effect do they all gain
    core_bool is_active; // is it active?
} core_bond;

typedef core_u32 core_item_id;

typedef struct {
    core_item_id item_id;
    core_u32 info_id; // links to its info pool
    core_u32 sprite_id; // links to its sprite pool
    core_item_type type; // what type of item is this? (consumable, equip, etc)
    core_u32 effect_id;

    core_u32 ability_id;
    core_rarity rarity; // how rare is this guy
    core_bool is_stackable;
    core_u32 stack_count;
    core_u32 max_stack;

    core_u32 subtype_id; // what subtype of item is this? (health, weapon, etc)
    core_bool gainable; // can this dude be gained?
} core_item;


#define CORE_INVENTORY_MAX 32

typedef struct {
    core_item slots[CORE_INVENTORY_MAX];
    core_u32 count;
    core_f32 weight; // slows you down
} core_inventory;

/**
 * State Flags
 * --------------------------------------------------------------------------
 * Bit Flags that Describes what an entity can do and what is happening to it
 */

 typedef core_u32 core_state_flags;

 typedef enum {

    CORE_CAN_MOVE = 1 << 0,
    CORE_CAN_ACT = 1 << 1,
    CORE_CAN_ATTACK = 1 << 2,
    CORE_CAN_DEFEND = 1 << 3,

    CORE_IS_FROZEN = 1 << 4,
    CORE_IS_STUNNED = 1 << 5,
    CORE_IS_ON_FIRE = 1 << 6,

    CORE_SILENCED = 1 << 7,
    CORE_LOCKED =  1 << 8,
    CORE_INVULNERABLE = 1 << 9

 } core_state_flag_bits;

/**
 * Zeal Instance
 * --------------------------------------------------------------------------
 * Data-Driven
 * Passive skill or special traits atttached to an entity
 * 
 */
typedef core_u32 core_zeal_id;
#define CORE_ZEAL_NONE 0

typedef struct {
    core_zeal_id id;
    core_u32 source;
    core_i32 stacks;
    core_u32 duration;
}core_zeal_instance;

#define CORE_ZEAL_POOL_MAX 8

typedef struct {
    core_zeal_instance slots[CORE_ZEAL_POOL_MAX];
    core_u32 count;
} core_zeal_pool;


/**
 * Effects
 * --------------------------------------------------------------------------
 * Shared pool of buffs and debuffs. Max 45 - 90
 */

typedef core_u32 core_effect_id;
#define CORE_EFFECT_NONE 0

typedef struct {
    core_effect_id id;
    core_u32 source;
    core_i32 stacks;
    core_u32 duration;
} core_effect_instance;

#define CORE_EFFECT_POOL_MAX 45


typedef struct {
    core_effect_instance effects[CORE_EFFECT_POOL_MAX];
    core_u32 count;
} core_effect_pool;

/**
 * Stats 
 * --------------------------------------------------------------------------
 * Character statisics
 */

#define CORE_STAT_NO_ADDITIONS 0
typedef struct {
    core_i32 hp;
    core_i32 max_hp;
    core_i32 strength;
    core_i32 attack;
    core_i32 speed;
    core_i32 defense;
    core_i32 dodge;
    core_i32 tenacity; // Endurance How long smth lasts
    core_i32 effort; // Points used up by ablity
    core_u32 additions_id;
    core_u32 info_id; // links to its info pool
} core_stats;

/**
 * Combo
 * --------------------------------------------------------------------------
 * Improve later
 */

 typedef struct{
    core_i32 hits;
    core_i32 multiplier;
 } core_combo;




/**
 * Gauge
 * --------------------------------------------------------------------------
 * Fill meter - energy, ultimate, charge, etc
 */
typedef struct{
    core_i32 current_val;
    core_i32 max;
} core_gauge;

/**
 * Timer
 * --------------------------------------------------------------------------
 * Tick based timer for special triggers
 */

typedef struct{
    core_u32 current;
    core_u32 duration;
} core_timer;

/**
 * Info entry
 * --------------------------------------------------------------------------
 * Pure lore for the entity -- look up by id stores an entity's revelant data
 */

 typedef struct {
    core_u32 name_id; // string table -> display name */
    core_u32 description_id; // string table -> display description */
    core_u32 faction_id; // string table -> faction name */
    core_u32 home_id; // string table -> home region */
} core_info_entry;

/**
 * Gained Variants
 * --------------------------------------------------------------------------
 * Character Modifications when a player earns them
 */

typedef struct{
    core_bool is_gained;
    core_u32 sprite_id;
    core_stats stats;
} core_gained_variant;


/**
 * Entity 
 * --------------------------------------------------------------------------
 * Base Entity Struct Stored for Entity type to be inherited by a character and such
 */

typedef struct {
    core_entity_id id; //unique identifier
    core_u32 info_id; // links to its info pool
    core_u32 sprite_id; // links to its sprite pool
    core_rarity rarity; // how rare is this guy
    core_gender gender; // gender
    core_size size; // how big is this dude
    core_species_id species; // what is it 
    core_class_id  class_type; // what class does it belog to
    core_bool gainable; // can this dude be gained
    
    
    core_bond bonds; // Team up

    //spatial
    core_vec3 position;
    core_vec3 velocity;

    //stats
    core_stats stats;

    //combat
    core_combo combo;
    core_gauge gauge;
    core_effect_pool effect_pool;

    //state
    core_state_flags state;
    core_zeal_pool   zeal_pool;    /* multiple zeals, data-driven   */
    core_timer       timer;

    core_gained_variant gained;
}core_entity;






#ifdef __cplusplus
}
#endif

#endif