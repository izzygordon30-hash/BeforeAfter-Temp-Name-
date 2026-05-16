#ifndef CORE_SUMMONS_H
#define CORE_SUMMONS_H

#include "Core.h"

#ifdef __cplusplus
extern "C" {
#endif

/**=================================================================
* Summon Pool
* --------------------------------------------------------------------
* Used by SummonRegistry and Combat.h
* Lives Here so Combat.h and registries share the type.
==================================================================*/


/**
 * ## Summon Max
 * Max Amount of Summons
 */
#define CORE_SUMMON_MAX 4

/** Summoned Entity
 * -------------------------------------------------------
 *  Summoned Buddy Struct - Setup for the amt of creatures to summon*/
typedef struct {
    /**
     * ## IDS
     * < Amount of Entities Summmoned ids
     */
    core_summon_id ids[CORE_SUMMON_MAX]; 
    /**
     * ## Handles
     * < The Entities Configurations (The Entities themselves)(Instances)
     */
    core_entity_id handles[CORE_SUMMON_MAX]; 
    /**
     * ## Count
     * < Amount Currently Summoned
     */
    core_u32 count; 
    /**
     * ## Potency
     * < Strength handled by C++
     */
    core_u32 potency_id;
} core_summoned_buddy;


#ifdef __cplusplus
}
#endif

#endif