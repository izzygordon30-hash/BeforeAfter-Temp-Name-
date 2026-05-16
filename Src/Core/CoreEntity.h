#ifndef CORE_ENTITY_H
#define CORE_ENTITY_H


#include "Core.h"

#ifdef __cplusplus
extern "C" {
#endif


/**=================================================================
* Bond Type
* --------------------------------------------------------------------
* Bond Types
*
==================================================================*/


/**
 * ## Bond Type
 * Bond Type Characters/Entities Have
 */
typedef enum {
    /**
     * ## No Relationship
     * No Relationships with a group or person
     */
    CORE_BOND_TYPE_NONE = 0,
    /**
     * ## Friendship
     * Nice Relationships with a group or person
     */
    CORE_BOND_TYPE_FRIENDSHIP,
    /**
     * ## Rivalry
     * Bad Relationships with a group or person
     */
    CORE_BOND_TYPE_RIVALRY,
    /**
     * ## Squad
     * Relationships with a group or person
     */
    CORE_BOND_TYPE_SQUAD,
    /**
     * ## Forbidden
     * Not at All!
     */
    CORE_BOND_TYPE_FORBIDDEN,
} core_bond_type;


/**=================================================================
* Kind of Entity
* --------------------------------------------------------------------
* Character, Summon, Projectile, Trap, Object
* ===================================================================*/


/**
 * ## Entity Kind
 * Kind of Entity?
 */
typedef enum {
    /** ## None 
     * No Entity Kind Assigned
     */
    CORE_ENTITY_KIND_NONE = 0,
    /** ## Character
     * A living being
     */
    CORE_ENTITY_KIND_CHARACTER,
    /** ## Summon
     * A summoned creature
     */
    CORE_ENTITY_KIND_SUMMON,
    /** ## Projectile
     * A ranged attack
     */
    CORE_ENTITY_KIND_PROJECTILE,
    /** ## Trap
     * A stationary hazard
     */
    CORE_ENTITY_KIND_TRAP,
    /** ## Object
     * An inanimate object (e.g., destructible environment, lootable item)
     */
    CORE_ENTITY_KIND_OBJECT
} core_entity_kind;

/**=================================================================
* Role Types
* --------------------------------------------------------------------
* What kind of role does this Entity plays
* Are they a Tank, Attacker, Support, Controller, AttackBud, Unknown
==================================================================*/



/**
 * ## Role Types
 * What kind of role does this Entity plays
 * Are they a Tank, Attacker, Support, Controller, AttackBud, Unknown
 */
typedef enum {
    /**
     * ## No Role
     * No Role Assigned
     */
    CORE_ROLE_NONE = 0,
    /**
     * ## Tank 
     * Lots of Health, Sometimes admirable amt of attack.
     * Main Purpose is to usually taunt though some tanks can be supporters or attackers.
     */
    CORE_ROLE_TANK = 1u << 1,
    /**
     * ## Attacker
     * Lots of Attack, Sometimes admirable amt of health.
     * Main Purpose is to usually deal damage though some attackers can be supporters or tanks.
    */
    CORE_ROLE_ATTACKER = 1u << 2,
    /**
     * ## Support
     * Heal and Buffs, Sometimes admirable amt of attack or health.
     * Main Purpose: is to usually heal and buff though some supporters can be attackers or tanks.
     */
    CORE_ROLE_SUPPORT = 1u << 3,
    /**
     * ## Healer
     * Lots of Healing, Sometimes admirable amt of attack or health.
     * Main Purpose: is to usually heal though some healers can be attackers or tanks.
     */
    CORE_ROLE_HEALER = 1u << 4,
    /**
     * ## Controller
     * Crowd Control, Debuffs, Sometimes admirable amt of attack or health.
     * Main Purpose: is to usually debuff and crowd control though some controllers can be attackers or tanks.
     */
    CORE_ROLE_CONTROLLER = 1u << 5,   
    /**
     * ## Summoner
     * Summons a bunch of things to the battlefield to fight for them.
     * Main Purpose: Summoning things to fight for them.
     */
    CORE_ROLE_SUMMONER = 1u << 6,
    /**
     * ## Unknown Role
     * We have no idea what they do. Could be anything.
     */
    CORE_ROLE_UNKNOWN = 1u << 7,
} core_role_type;




typedef struct {
    /**
     * ## Bond Type
     * Type of Bond
     */
    core_bond_type type; 
    /**
     * ## Trigger
     * What Happens Specfic Characters are paried up together.
     */
    core_class_id trigger; 
    /**
     * ## Partners
     * Who is in this bond
     */
    core_bond_partner_pool partners;
    /**
     * ## Rival Team
     * Which Team is this one's rival
     */
    core_rival_pool rival_id;
    /**
     * ## Special Given (C++ Resolved)
     * What Special Zeal does the character(s) gain
     */
    core_u32 special_given;
    /**
     * ## Effect Given (C++ Resolved)
     * What effect do they all gain
     */
    core_u32 effect_given; 
    /**
     * ## Is Active
     * Is it active?
     */
    core_bool is_active; 
} core_bond;



/**=================================================================
* Entity
* --------------------------------------------------------------------
* Pure simulation object. Container of Data that applies to an Entity.
* What it IS - not how it behaves.
* 
* Does Not contain:
* - Abilities/Loadout -> Combat.h (core_combat)
* - Transformation -> Combat.h (core_transformation_act)
* - Corruption state -> Combat.h (core_corrupted_state)
* - Gained Variant -> Character layer (core_character)
* - or any other combat attributes
*
* C++ registries always receive core_entity* directly.
* core_character embeds core_entity as its first member
==================================================================*/

/**
 * ## Entity
 * Defines an Entity
 */
typedef struct {
    /**-----Identity------ */
    
    /** ## Entity Id 
     * Unique identifier for an Entity
    */
    core_entity_id  id; 
    /** ## Entity Info ID
     *  Universal info Entry Table
     *  Links to its info pool
    */
    core_info_id       info_id; 
    /** ## Sprite ID
     *  links to its sprite pool
    */
    core_sprite_id        sprite_id; 
    /** ## Rarity
     *  How rare is this guy
    */
    core_rarity_flags     rarity;
    /** ## Gender
     *  What Gender is this Entity
    */
    core_gender_flags     gender;
    /** ## Size
     *  What Size / How big is this Entity
    */
    core_size_flags       size; 
    /** ## Species
     *  What is the Entity
    */
    core_species_id     species;
    /** ## Class 
     *  What class does it belong to.
    */
    core_class_id       class_type;
    /** ## Class 
     *  Can this dude be gained.
    */
    core_bool           gainable; 
    /** ## Role
     *  What Type is this Entity In Combat
     *  Tank, Attacker, etc..
    */
    core_role_flags     role; 


    /* ---- Relationships ---- */
    /**
     * ## Bonds
     * Do they know anyone 
     */
    core_bond bonds; 

    /* ---- Spatial ---- */
    /**
     * ## Position
     * Where are they in the world.
     */
    core_vec3 position;
    /**
     * ## Velocity
     * Speed + Direction.
     */
    core_vec3 velocity;

    /**
     * ## Stats
     * What are their stats
     */
    core_stats stats;

    /* ---- Lightweight combat-adjacent ---- */
    /**
     * ## Combo
     * combo.hits fuel the gauge.
     * The combo.multiplier will mutiply an attacks damage.
     */
    core_combo combo;
    /**
     * ## Gauge
     * Connected Gauge for any ultimates
     */
    core_gauge gauge;

    /**
     * ## Effect Pool
     * Effects that the Entity Currently has
     */
    core_effect_pool effect_pool;
    /**
     * ## Zeal Pool
     * Effects that the Zeal Currently has.
     */
    core_zeal_pool   zeal_pool;    
    
    /**
     * Runtime State
     * The Entities State.
     * eg. Can_Move Can_Act
     */
    core_state_flags state;
    
    /**
     * ECS Hint Mask
     * Which Conpontents are active.
    */
    core_component_mask mask; 

    /* ---- AdditionMachine port ---- */
    /**
     * ## Extension Port
     * C++ fills meaning — 0 = none 
     */
    core_ext_id extension;      

} core_entity;


#ifdef __cplusplus
}
#endif

#endif /* CORE_TYPES_H */