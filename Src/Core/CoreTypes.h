#ifndef CORE_TYPES_H
#define CORE_TYPES_H

/* ====================================
  * Standard Libraries
=============================================*/

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "CoreIDs.h"

#ifdef __cplusplus
extern "C" {
#endif


/** ==================================================================
 * Compiler Hints
 * ---------------------------------------------------------------------
 * Tells the Compiler to format the code in a certain way
 * Helps with Optimization
 =====================================================================*/

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

/**=================================================================
* Primative Types
==================================================================*/

/** ### Unsigned Int(8 bits) */
typedef uint8_t     core_u8;
/** ### Unsigned Int(16 bits) */
typedef uint16_t     core_u16;
/** ### Unsigned Int(32 bits) */
typedef uint32_t    core_u32;
/** ### Unsigned Int(64 bits) */
typedef uint64_t    core_u64;

/** ### Int(8 bits) */
typedef int8_t      core_i8;
/** ### Int(16 bits) */
typedef int16_t      core_i16;
/** ### Int(32 bits) */
typedef int32_t     core_i32;
/** ### Int(64 bits) */
typedef int64_t     core_i64;

/** ### Float(32 bits) */
typedef float       core_f32;
/** ### Float(64 bits) */
typedef double      core_f64;

/** ### Boolean (1 or 0) */
typedef bool        core_bool;


/** ### Char* String 
 * In C there's not really a native string type.
 * so Char* is an array of characters(Chars*).
 * Which is a String. Strings are also null-terminated 
 * So const char* for safety and to avoid accidental modifications.
*/
typedef const char* core_string;


/**=================================================================
* Generic Handle
* ------------------------------------------------------------------
* Safe handle for world storage referencing for any game object.
* Generation counter invalidates stale/zombie handles
==================================================================*/

/**
* ## Generic Handle
* Safe handle for referencing any game object (entities, items, abilities, etc.) in a type-agnostic way.
*/
typedef struct {
    /**
     * ### Index
     * Slot in the World Storage
     */
    core_u32 index;
    /**
     * ### Generation
     * Version Counter (Invalidates Stale/Old/Zombie Handles/References)
     */
    core_u32 generation;
} core_handle;

/**=================================================================
* Reference
* ------------------------------------------------------------------
* Type-Agnostic Reference for any game object (entities, items, abilities, etc.)
* Combines a kind (entity, item, ability, etc.) with an ID to safely reference objects in world storage.
==================================================================*/



/**
 * ### Reference
 * Type-Agnostic Reference for any game object (entities, items, abilities, etc.)
 */
typedef struct {
    core_kind_id kind;
    core_id id;
} core_ref;


/**=================================================================
* Sentinels
==================================================================*/

/** ### INVAILD */
#define CORE_INVALID         0
/** ### NO EXTRAS */
#define CORE_EXT_NONE        0
/** ### NO CLASS */
#define CORE_CLASS_NONE      0
/** ### NO SPECIES */
#define CORE_SPECIES_NONE    0
/** ### NO EFFECT */
#define CORE_EFFECT_NONE     0
/** ### NO ZEAL */
#define CORE_ZEAL_NONE       0
/** ### NO ABILITY */
#define CORE_ABILITY_NONE    0
/** ### NO SUMMMONS */
#define CORE_SUMMON_NONE     0
/** ### NO STAT ADDITIONS*/
#define CORE_STAT_NO_ADDITIONS 0
/** ### NO DAMAGE*/
#define CORE_DAMAGE_NONE 0
/** ### NO AI PROFILE */
#define CORE_AI_NONE 0
/** ### NO REFERENCE */
#define CORE_REF_NONE 0
/** ### NO ASSET */
#define CORE_ASSET_NONE 0
/** ### NO STRING */
#define CORE_STRING_NONE 0
/** ### NO PREFAB */
#define CORE_PREFAB_NONE 0
/** ### NO REGISTRY */
#define CORE_REGISTRY_NONE 0
/** ### NO ENTITY KIND */
#define CORE_KIND_NONE 0
/**
 * No Resistances
 * No Residtances whatsover
 */
#define CORE_RESISTANCE_NONE 0


/** ### VERSIONS */

/**
 * ## Versioning (Major)
 * Major Version - Incompatible Changes
 */
#define CORE_VERSION_MAJOR 1
/**
 * ## Versioning (Minor)
 * Minor Version - Backwards-Compatible Changes
 */
#define CORE_VERSION_MINOR 0


#ifdef __cplusplus
}
#endif

#endif /* CORE_TYPES_H */