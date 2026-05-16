#ifndef CORE_STATS_H
#define CORE_STATS_H

#include "Core.h"

#ifdef __cplusplus
extern "C" {
#endif

/**=================================================================
* Stats
* --------------------------------------------------------------------
* Hot Path base Stats - always present, no overhead.
* StatRegistry IDs (Used by C++ switch statments):
* 1 = hp, 2 = max_hp, 3 = strength, 4 = defense
* 5 = dodge, 6 = tenacity, 7 = effort.
* additions_id -> C++ StatRegistry resolves extras
==================================================================*/

/**
 * ## Stats
 * Stats of an Entity
 */
typedef struct {
    /**
     * ## Health Points (Hp)
     * Amount of Health the Entity Currently Has.
     */
    core_i32 hp;
    /**
     * ## Max Health Points (max_hp)
     * Amount of Health the Entity Currently Has.
     */
    core_i32 max_hp;
    /**
     * ## Strength 
     * Increases the amout of attack an attack does.
     */
    core_i32 strength;
    /**
     * ## Attack
     * Amount of damage they currently deal.
     */
    core_i32 attack;
    /**
     * ## Speed
     * How quickly do they reach their turn.
     */
    core_i32 speed;
    /**
     * ## Defense
     * How well they defend against big damage (how well they cushion it)
     */
    core_i32 defense;
    /**
     * ## Dodge
     * How often they dodge an attack.
     */
    core_i32 dodge;
    /**
     * ## Tenacity
     * Endurance How long an effect lasts
     */
    core_i32 tenacity;
    /**
     * ## Effort Points
     * Tracks how much effort a Entity currently has.
     * (Resource consumed by abilities)
     */
    core_i32 effort; 
    /**
     * ## Additions (Any Additions?)
     * C++ StatRegistry Extras
     * If its None (0) = skip.
     */
    core_u32 additions_id;
} core_stats;



#ifdef __cplusplus
}
#endif

#endif