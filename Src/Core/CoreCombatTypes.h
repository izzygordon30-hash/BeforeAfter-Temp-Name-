#ifndef CORE_COMBAT_TYPES_H
#define CORE_COMBAT_TYPES_H

#include "Core.h"

#ifdef __cplusplus
extern "C" {
#endif

/**=================================================================
* Combat-Adjacent Data
* --------------------------------------------------------------------
* Lightweight shared mechanics. No logic embedded. Full combat
* system lives in Src/Systems/Combat.
==================================================================*/

/**
 * ## Combo
 * ----------------------------------------------------------------------
 * Hit-Tracking only
 */
typedef struct {
    /**
     * ## Hits
     * How many hits the Entity/Character has dealt.
     */
    core_i32 hits;
    /**
     * ## Multiplier
     * Mutiplies an attack on account of a combo.
     */
    core_i32 multiplier;
} core_combo;

/**
 * Gauge (Generic Meter)
 * --------------------------------------------------------------------------
 * Generic Fill meter - energy, ultimate, charge, etc
 */
typedef struct{
    /** 
     * ## Current Value
     * Where is the Gauge Currently At.
     */
    core_i32 current_val;
    /** 
     * ## Max Value
     * Where is the Gauge Hits when completed. (100%)
     */
    core_i32 max;
} core_gauge;

/**
 * ## Timer (Universal Tick Counter)
 * --------------------------------------------------------------------------
 * Tick based timer for special triggers
 */
typedef struct{
    /**
     * ## Current
     * ------------------------------------
     * Where is the Timer is Currently At
     */
    core_u32 current;
    /**
     * ## Duration
     * ------------------------------------
     * How Long in Ticks
     */
    core_u32 duration;
} core_timer;

/**=================================================================
*  Zeal Instance
* --------------------------------------------------------------------
* Data-Driven passive trait pool.
* C - Holds the fixed pool. C++ ZealRegistry resolves IDs.
* source: Who applied it
* stacks: intensity
* duration: ticks remaining (0 = permanent)
==================================================================*/

/**
 * ## A Zeal Instance
 * Permanent or semi-permanent trait modifier (Passive Skills)
 * -----------------------------------------------------------
 * (This is literaly a zeal)
 * Its Stores:
 * - The Zeal Id
 * - Who it came from
 * - Amount of it
 * - Duration
 */
typedef struct {
    /**
     * ## Zeal ID
     * Unique Indentifier - Which Zeal
    */
    core_zeal_id id;
    /**
     * ## Source
     * Source Where did the Zeal come from
    */
    core_u32 source;
    /**
     * ## Stacks
     * Stacks? of zeal
    */
    core_i32 stacks;
    /**
     * ## Length - For a Temp Zeal
     * Zeal Duration - Zeals are normally permanent
    */
    core_u32 duration;
}core_zeal_instance;

/**
 * ## Zeal Pool Max
 * Max Amount of Zeals a Character/Entity can own
*/
#define CORE_ZEAL_POOL_MAX 8

/**
 * ## Zeal Pool
 * Stores Zeals in a Pool
*/
typedef struct {
    /**
     * ## Slots
     * Tracks Zeal Slots
     */
    core_zeal_instance slots[CORE_ZEAL_POOL_MAX];
    /**
     * ## Count
     * Amount of Zeals Currently
     */
    core_u32 count;
} core_zeal_pool;

/**=================================================================
*  Effect Instance
* --------------------------------------------------------------------
* Data-Driven buff/debuff pool. Buffs and Debuffs live together
* C++ EffectRegistry resolved IDs. 
* Expand CORE_EFFECT_POOL_MAX to 90 if needed
==================================================================*/

/**
 * ## An Effect 
 * (This is literaly an effect)
 * Its Stores:
 * - The Effect Id
 * - Who it came from
 * - Amount of it
 * - Duration
 */
typedef struct {
    core_effect_id id;
    core_u32 source;
    core_i32 stacks;
    core_u32 duration;
} core_effect_instance;

/**
 * ## Effect Pool Max
 * Max Amount of Effects an Entity can have.
 */
#define CORE_EFFECT_POOL_MAX 45

/**
 * ## Effect Pool
 * Stores Effect in a Pool
*/
typedef struct {
    /**
     * ## Effects
     * Amount of Effects Pooled
     */
    core_effect_instance effects[CORE_EFFECT_POOL_MAX];
    /**
     * ## Effects
     * Amount of Effects Currently
     */
    core_u32 count;
} core_effect_pool;


#ifdef __cplusplus
}
#endif

#endif
