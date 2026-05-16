#ifndef COMBATOLD_H
#define COMBATOLD_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Standard Libraries
 * ---------------------------------------------------------------------
 * Default Imported elements from the C standart library 
 */

#include "Core.h"

typedef core_u32 core_ability_id;
#define CORE_ABILITY_NONE 0
#define CORE_ABILITY_SLOTS_DEFAULT 5 
#define CORE_ABILITY_SLOTS_MAX 6

/**
 * Abilities
 */
typedef struct{
    core_ability_id ids[CORE_ABILITY_SLOTS_MAX];
    core_u32 slot_count;
    core_ability_id ultimate_id;
    core_bool ultimate_ready;
} core_ability_loadout;

typedef struct {
    core_u32 corruption_id;
    core_f32 corruption_rate;
    core_u32 stack_count;
    core_bool is_active;
    core_bool is_permanent;
} core_corrupted_state;

typedef struct{
    core_entity_id owner_id; // Who owns this
    core_u32 stat_id; // which stat preferably numerical stats
    core_i32 trigger_stat_amt; // amt to trigger
    core_u32 reward_type; // Zeal, Effect, Other
    core_bool is_active;
} core_low_stat_act;

typedef struct{
    core_u32 transform_character_id;
    core_u32 sprite_id; //optional
    //abilties for transformed state is stored in the same pool as the character id that transforms
    core_stats changed_stats; //levels transfer though
    core_bool reverts; // does it go away
    core_timer ticks; // if it does then count in ticks
} core_transformation_act;

/**
 * Summonable Buddies
 * ---------------------------------------------------------------------
 * Little Helpers summoned from a character
 */
typedef core_u32 core_summon_id;
#define CORE_SUMMON_NONE 0
#define CORE_SUMMON_MAX 4


typedef struct {
    core_summon_id ids[CORE_SUMMON_MAX];
    core_entity_id handles[CORE_SUMMON_MAX];
    core_u32 count;
    core_u32 potency_id;
} core_summoned_buddy;

typedef core_u32 core_trigger_id;
typedef core_u32 core_condition_id;
typedef core_u32 core_scope_id;
 
#define CORE_TRIGGER_NONE   0
#define CORE_CONDITION_NONE 0
#define CORE_SCOPE_NONE     0

typedef struct {
    core_zeal_id zeal_id;
    core_scope_id scope_id;
    core_bool is_active;
    core_condition_id condition_id; //passives are owned by a character no condition/ there can be one added with a condition in addtion to this if true.
} core_passive;

#define CORE_PASSIVE_SLOT_MAX 4
 
typedef struct {
    core_passive slots[CORE_PASSIVE_SLOT_MAX];
    core_u32             count;
} core_passive_pool;


typedef struct {
    core_ability_id action_id;
    core_entity_id source;
    core_entity_id target;
    core_u32 priority;
    core_bool can_interrupt;
} core_combat_action;

#define CORE_ACTION_STACK_MAX 16

typedef struct {
    core_combat_action stack[CORE_ACTION_STACK_MAX];
    core_u32 count;
}core_action_stack;

typedef struct {
    core_ability_loadout abilities;
    core_corrupted_state corruption;
    core_low_stat_act low_stat_act; 
    core_bool can_summon;   
    core_summoned_buddy summons;
    core_passive_pool passives;
    core_action_stack actions;
} core_combat;



#ifdef __cplusplus
}
#endif

#endif