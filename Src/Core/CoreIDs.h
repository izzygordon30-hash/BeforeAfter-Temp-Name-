#ifndef COREIDS_H
#define COREIDS_H

#include "CoreTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

/**=================================================================
* Universal IDs
* ------------------------------------------------------------------
* One Type for referencing any game object by ID.
* C++ Registries resolve what each ID means.
==================================================================*/

/* ### Base ID Type*/
typedef core_u32 core_id;

/* ### Info Entry Table*/
typedef core_id core_info_id;

/* ### Sprite Pool Table*/
typedef core_id core_sprite_id;

/* ### C++ Class Registry*/
typedef core_id core_class_id;

/* ### C++ Species Registry*/
typedef core_id core_species_id;

/* ### C++ Effect Registry*/
typedef core_id core_effect_id;

/* ### C++ Zeal Registry*/
typedef core_id core_zeal_id;

/* ### C++ Class Registry*/
typedef core_id core_ability_id;

/* ### Item Table*/
typedef core_id core_item_id;

/* ### C++ Summon Registry*/
typedef core_id core_summon_id;

/* ### AdditionMachine Port*/
typedef core_id core_ext_id;

/** ### Tag Mask 
 * Bitmask for Tag-Based Systems (e.g., targeting, filtering, interactions)
 * Boss, Flying, Melee, Ranged, Stealth, etc.
*/
typedef core_u64 core_tag_mask;

/* ### Unique Entity ID 
 * Combines an index with a generation counter to safely reference entities in world storage.
 * Prevents stale/zombie references.
*/

/**
 * ### Unique ID 
 * A simple unique identifier for any game object (entities, items, abilities, etc.) that doesn't require generation tracking.
 * Useful for static references or objects that don't get destroyed/recreated frequently.
*/
typedef core_u64 core_uid;

/* ### Primitive ID 
 * A compact identifier for core primitives (e.g., ability steps, effect types) used in bytecode execution.
 * Allows for efficient dispatching in C++ without needing to resolve to a full info entry.
*/
typedef core_u32 core_damage_type_id;

/** ### AI Profile ID 
 * Identifier for AI behavior profiles, used by the AI system to determine how an entity should act in combat or other situations.
*/
typedef core_u32 core_ai_profile_id;

/**
 * ### Runtime ID
 * A unique identifier assigned at runtime to entities or objects that need to be tracked during gameplay but don't require the full generation tracking of `core_entity_id`.
 */
typedef core_u32 core_runtime_id;

/**
 * ### Reference ID
 * A type-agnostic identifier used for referencing any game object (entities, items, abilities, etc.) in a way that can be resolved by C++ registries.
 */
typedef core_u32 core_ref_id;

/**
 * ### Asset ID
 * Identifier for game assets (e.g., textures, models, sounds) that can be loaded and referenced in C++.
 */
typedef core_u32 core_asset_id;

/**
 * ### String ID
 * Identifier for strings stored in a string table, allowing for localization and efficient string management in C++.
 */
typedef core_u32 core_string_id;

/**
 * ### Prefab ID
 * Identifier for prefabricated entities or objects that can be instantiated in the game world, with their configurations defined in C++.
 */
typedef core_u32 core_prefab_id;

/**
 * ### Registry ID
 * Identifier for entries in C++ registries (e.g., entities, items, abilities) that can be looked up and instantiated based on their configurations defined in C++.
 */
typedef core_u32 core_registry_id;

/**=================================================================
* Resistances
* --------------------------------------------------------------------
* None
* Male
* Female
* Unknown
* Other
==================================================================*/

/**
 * ## Resistance ID
 * Unique identifier for each type of resistance (e.g., Fire, Ice, Poison).
 */
typedef core_u32 core_resistance_id;

/**
 * ### Core Hash
 * A compact, fixed-size identifier generated from a string or combination of properties, used for efficient lookups and comparisons in C++.
 */
typedef core_u64 core_hash;



/**=================================================================
* Entity ID
* ------------------------------------------------------------------
* Safe handle for world storage referencing.
* Generation counter invalidates stale/zombie handles
==================================================================*/

/**
* ## Entity ID
* Safe handle for world storage referencing.
* Generation counter invalidates stale/zombie handles
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
} core_entity_id;

/**
 * ## INVALID ENTITY
 */
#define CORE_INVALID_ENTITY (core_entity_id){UINT32_MAX, 0}

/**
 * ### Kind ID
 * Unique identifier for the type of object being referenced.
 */
typedef core_u32 core_kind_id;

/**
 * ## Role Flags
 * BitFlags for what kind of role does this Entity plays
 */
typedef core_u32 core_role_flags;



#ifdef __cplusplus
}
#endif

#endif