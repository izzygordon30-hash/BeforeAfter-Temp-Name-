#ifndef CORE_RELATIONS_H
#define CORE_RELATIONS_H

#include "Core.h"

#ifdef __cplusplus
extern "C" {
#endif

/**=================================================================
* Bond
* --------------------------------------------------------------------
* Team-Up slot. C holds fixed windows. 
* C++ RelationshipSystem manages overflow.
* Rust tracks organic bond progression thresholds.
==================================================================*/

/**
 * ## BOND
 * Team-up slot. When paried with matching partner_class or relationship,
 * they get special benefits.
 */

/**
 * ## Max Partners
 * Max Amount of Persons A Character/Entity can have relationships with. 
 */
#define CORE_BOND_PARTNER_MAX 10

/**
 * ## Partners Pool
 * Pools Partners. 
 */
typedef struct {
    /** 
     * ## Ids[CORE_BOND_PARTNER_MAX]
     * All Entities in the Bond By their IDs.
     */ 
    core_u32 ids[CORE_BOND_PARTNER_MAX];
    /** 
     * ## Count
     * Amount of Partners currently
     * total known to C++ — may exceed CORE_BOND_PARTNER_MAX 
     */ 
    core_u32 count; 
} core_bond_partner_pool;

/**
 * Max Rivals
 * Max Amount of Persons A Character/Entity can have a rivalry with. 
 */
#define CORE_RIVAL_POOL_MAX 10

/**
 * ## Rivals Pool
 * Pools Rivals. 
 */
typedef struct {
    /** 
     * ## Ids[CORE_RIVAL_POOL_MAX]
     * All Entities in the Bond By their IDs.
     */ 
    core_u32 ids[CORE_RIVAL_POOL_MAX];
    /** 
     * ## Count
     * Amount of Rivals currently
     * total known to C++ — may exceed CORE_RIVAL_POOL_MAX
     */ 
    core_u32 count;
} core_rival_pool;


#ifdef __cplusplus
}
#endif

#endif
