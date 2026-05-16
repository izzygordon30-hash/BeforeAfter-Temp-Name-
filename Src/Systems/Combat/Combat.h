#ifndef COMBAT_H
#define COMBAT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Core.h"

typedef core_u32 core_primitive_id;

#define CORE_PRIMITIVE_NONE 0 //None
#define CORE_PRIMITIVE_MODIFY 1 // CHANGE ANY VALUE( STATS NO META THINGS SUCH AS NAME)
#define CORE_PRIMITIVE_MOVE 2 // CHANGE ANY OWNER/TARGET (STEAL, REDIRECT, SWAP, DRAIN)
#define CORE_PRIMITIVE_LOCK 3 // LOCK ANY STATE()

#define CORE_PRIMITIVE_LINK 4 // CONNECT TWO THINGS (CHAIN ATKS, PULSE, MERGE)
#define CORE_PRIMITIVE_GATE 5 // CONDITION ANY ACTION
#define CORE_PRIMITIVE_SPAWN 6 // CREATE OR DESTROY ANY ENTITY(Summonable)
#define CORE_PRIMITIVE_TIME 7 // MANIPULATE WHEN (DELAY, REWIND, SPEED, SLOW)

#define CORE_PRIMITIVE_SCOPE 8 // THE REACH OF THIS EFFECT/ZEAL/ABILITY
#define CORE_PRIMITIVE_ORDER 9 // THE SEQUENCE OF SOMETHING (PRIORITY SKIP INTERRUPT)
#define CORE_PRIMITIVE_CHANCE 10 // RANDOMNESS (CRIT, DODGE, RANDOM EFFECT, CHAY'S GAMBLING PROBLEM)
#define CORE_PRIMITIVE_REVEAL 11 // HIDE OR EXPOSE THINGS (STEALTH, HIDDEN, SCAN, DETECT)

#define CORE_PRIMITIVE_FORGE 12 // MODIFY ANY META THING(CLASS, BOND, SPECIES)
#define CORE_PRIMITIVE_BOND 13 // CREATE OR DESTROY RELATIONSHIPS
#define CORE_PRIMITIVE_RECALL 14 // REFERNCE PAST STATES (RESTORE HISTORY, DEAD MEMEBER)
#define CORE_PRIMITIVE_SURGE 15 // BREAK PAST NORMAL STAT LIMITS, OVERFLOW

#define CORE_PRIMITIVE_CONVERT 16 //FLIPS ANYTHING (BUFF -> DEBUFF, FLIP, TRANSFORM)
#define CORE_PRIMITIVE_RESTORE 17 //RETURN TO ORIGINAL STATE (+OPTIONAL MODIFIER ON TOP LIKE 5% MORE.)
#define CORE_PRIMITIVE_MIRROR 18 // SHARE THINGS (WHAT HAPPENS TO THIS CHARACTER HAPPENS TO THIS ONE W)
#define CORE_PRIMITIVE_INHERIT 19 // PERMANENTLY TAKE ON (ABSORB ANY PROPERTY OF ANY MEMBER DEFEATED ALLY OR EMEMY)

#define CORE_PRIMITIVE_UNWRITE 20 //REMOVE FORM HISTORY (NEVER HAPPENED, CAN USE THIS FOR HITS) EXAMPLE AN ABILITY "DID YOU REALLY HIT ME...HMM?" can be applied the text time someone attacks
#define CORE_PRIMITIVE_DIVERGE 21 // SPLIT INTO MULTIPLES (ONE ATK BECOMES THREE)
#define CORE_PRIMITIVE_ECHO 22 // REPLAY LAST EVENT (ABLITY HAPPENS AGAIN)
#define CORE_PRIMITIVE_SENSE 23 // READ WITHOUT ACTING (CHECK THE EMEMYS HUD WHAT ABILTIY CAN BE PLAYED NEXT)
 
#define CORE_PRIMITIVE_WEIGHT 24 //CHANGE TURN ORDER (GO FIRST, SKIP, REQUEQE) CAN BE USED TO MAKE YOUR CHARCETER GO FIRST, SKIP THEIR TURN OR MAKE ANY EMEMY GO LAST
#define CORE_PRIMITIVE_SHIELD 25 // INTERECPT BEFORE TAREGET (BARRIER ABSORBS BEFORE HP), YOU PERPARE FOR A HIT BASICALLY A BARRIER TO HIT UNTIL IT TOUCHES YOUR ACTAULLY HP
#define CORE_PRIMITIVE_MARK 26 // TAGGED FOR FUTURE USE (MARKED = 2X FROM THE NEXT HIT)
#define CORE_PRIMITIVE_PIERCE 27  // BYPASS ALL DEFENSIVE MECHANICS

#define CORE_PRIMITIVE_COUNT 28

#define CORE_ABILITY_STEP_MAX 8 // MAX STEPS PER ABILITY
#define CORE_ABILITY_BYTECODE_MAX 64 // MAX BYTES PER ABILITY PROGRESS



typedef core_u32 core_scope_id;

/** # Target
 * -------------------------------------------------------
 * Target Struct */
typedef struct {
    core_u32 source_id; /* Who fires this */
    core_u32 target_id; /* Who recieved this */
    core_scope_id scope_id; /* How much ppl are affected */
} core_step_target;

/** Payload
 * -------------------------------------------------------
 */
/** Payload Struct*/
typedef struct {
    core_u32 stat_id; /* Affected Stat */
    core_i32 value; /** by how much */
    core_f32 ratio; /* percentage modifier */
    core_timer duration; /* How long it lasts */
    core_u32 reward_id; /* Resolved in C++ but its what reward the chracter receives */
} core_step_payload;


/** Condition ID
 * -------------------------------------------------------
 */
typedef core_u32 core_condition_id;
/** Condition 
 * -------------------------------------------------------
 */
/** Condition Struct*/
typedef struct {
    core_condition_id condition_id; 
    core_i32 trigger_amt; 
    core_bool has_condition;
} core_step_condition;

typedef struct {
    core_primitive_id primitive_id;
    core_step_target target;
    core_step_payload payload;
    core_step_condition when;
} core_ability_step_readable;

/** @brief Union-based variant for memory-efficient bytecode execution */
typedef struct {
    core_primitive_id type;
    union {
         
        struct {
            core_u32 stat_id;
            core_i32 value;
            core_f32 ratio;
            core_u32 target;
        } modify;

        struct {
            core_u32 what;
            core_u32 from;
            core_u32 to;
        } move;
        
        struct {
            core_u32 stat_id;
            core_timer duration;
            core_bool is_permanent; 
        } lock;

        struct {
            core_u32 entity_a;
            core_u32 entity_b;
            core_u32 link_type;
        } link;

        struct {
            core_u32 condition_id;
            core_u32 threshold;
            core_u32 effect_id;
        } gate;

        struct {
            core_u32 what;
            core_u32 where;
            core_u32 potency;
        } spawn;

         struct {
            core_timer delay_ticks;
            core_u32 target;
            core_f32 rate;
        } time;

        struct { 
            core_u32 scope_id;
            core_f32 radius;
        } scope;

        struct {
            core_u32 priority;
            core_u32 target;
        } order;

        struct {
            core_f32 probability;
            core_u32 effect_id;
            core_u32 fallback_id;
        } chance;

        struct {
            core_u32 target;
            core_bool hidden;
            core_u32 duration;
        } reveal;

        struct {
            core_u32 target;
            core_u32 state_shapshot_id;
        } recall;

        struct {
            core_u32 stat_id;
            core_f32 multiplier;
            core_timer duration;
        } surge;

        struct {
            core_u32 target;
            core_u32 from_type;
            core_u32 to_type;
        } convert;

        struct {
            core_u32 target;
            core_f32 bonus_ratio;
            core_u32 scope_id;
        } restore;

        struct {
            core_u32 enitity_a;
            core_u32 entity_b;
            core_timer duration;
        } mirror;

        struct {
            core_u32 source;
            core_u32 what_id;
        } inherit;

        struct {
            core_u32 target;
            core_u32 effect_id;
        } unwrite;

        struct {
            core_u32 source_effect;
            core_u32 split_count;
            core_f32 value_ratio;
        } diverge;

        struct {
            core_u32 ability_id;
            core_timer delay_ticks;
        } echo;

        struct {
            core_u32 target;
            core_u32 new_identity_id;
        } forge;

        struct {
            core_u32 entity_a;
            core_u32 entity_b;
            core_u32 bond_type;
        } bond;

        struct {
            core_u32 target;
            core_u32 what_to_read;
        } sense;
        
          struct {
            core_u32 target;
            core_i32 priority_delta;
        } weight;

        struct {
            core_u32 target;
            core_u32 what_to_pierce; // dodge, armor, shield, stealth, immunity
        } pierce;

          struct {
            core_u32 target;
            core_i32 absorb_amt;
            core_timer duration;
        } shield;

          struct {
            core_u32 target;
            core_u32 mark_id;
            core_timer duration;
        } mark;
        
    } params;
} core_ability_step;

typedef struct {
    core_u8 bytecode[CORE_ABILITY_BYTECODE_MAX];
    core_u32 length;
    core_u32 step_count;
    core_u32 ability_id;
} core_ability_program;

#define CORE_ABILITY_NAME_MAX 32
typedef core_u32 core_ability_id;

typedef core_u32 core_info_id;
//Apply info for any ability effect zeal..etc
typedef struct {
    core_info_id info_id;
    core_u32 description_id;
    core_u32 name_id;
    core_bool point_driven;
    core_u32 effort_count;
} apply_description;



typedef struct{
    core_ability_id id; 

    core_ability_step steps[CORE_ABILITY_STEP_MAX]; /** RUNTIME */

    core_ability_step_readable readable[CORE_ABILITY_STEP_MAX]; /** DEBUG */
    
    core_ability_program program; /** STORAGE */

    core_u32 step_count; 
    core_u32 effort_cost; /**< How much Effort lost */
    core_u32 effort_gain; /**< How much Effort gained */
    apply_description desc; /**< Add Description */

} core_ability_entry;


#define CORE_ABILITY_NONE 0
#define CORE_ABILITY_SLOTS_DEFAULT 5 
#define CORE_ABILITY_SLOTS_MAX 6

/**
 * ## Abilities
 * Sets up the Entities Abilites or Loads them out
 */
typedef struct{
    core_ability_id ids[CORE_ABILITY_SLOTS_MAX];  /**< regular slots */
    core_u32 slot_count; /**< 5 or 6 - set by DSL */
    core_ability_id ultimate_id; /*< dedicated ultimate*/
    core_bool ultimate_ready; /* gauge full enough*/
} core_ability_loadout;

/**
 * ## Corrupted State
 * Sets up the Entities Corruption status if so
 */
typedef struct {
    core_u32 corruption_id; /**< What Stat/ System is corrupted */
    core_timer corruption_rate; /** decay per tick */
    core_u32 stack_count; /* intensity of corruption*/
    core_bool is_active; /**< Is the Corruption Active? */
    core_bool is_permanent; /* True = cleanse ability required*/
} core_corrupted_state;

/**
 * ## Low Stat Action
 * Sets up the Entities Low stat action when they reach a certain stat theshold
 */
typedef struct{
    core_entity_id owner_id; /**< Who owns this */
    core_u32 stat_id; /**< Which stat preferably numerical stats */
    core_i32 trigger_stat_amt; /**< Amount to trigger */
    core_u32 reward_type; /**< Zeal, Effect, Other */
    core_u32 reward_id; /**< C++ Defines Reward*/ 
    core_u32 scope_id; /**< How much ppl are affected */
    core_bool is_active; /**< Is it Active? */
} core_low_stat_act_trigger;

#define CORE_LOW_STAT_TRIGGER_MAX 4

/** Low Stat Ability
 * -------------------------------------------------------
 */
/** Summoned Buddy Struct - Setup for an Entity's Transformation*/
typedef struct {
    core_low_stat_act_trigger slot[CORE_LOW_STAT_TRIGGER_MAX]; /**< Amount of low stat triggers a character can own */
    core_u32 count; /**< Amount of low stat triggers a character owns */
} core_low_stat_pool;


/** Availability
 * -------------------------------------------------------
 */
/** Availability Struct - Entity's Availability for battle*/
typedef struct {
    core_entity_id availability_id; /**< Who's unavailable */
    core_u32 reason_id; /**< Why they unavailable */
    core_bool is_available; /**< Are they available */
} availability;


/** Transform
 * -------------------------------------------------------
 */
/** Summoned Buddy Struct - Setup for an Entity's Transformation
 *  abilties for transformed state is stored in the same pool as the character id that transforms
*/
typedef struct{
    core_u32 transform_character_id; /**< Which Character */
    core_u32 sprite_id; /**< ### OPTIONAL IF THEIR SHOULD A NEW SPRITE */
    
    core_stats changed_stats; /**< Levels transfer though */
    core_bool reverts; /**< does it go away */
    core_timer ticks; /** if it does then count in ticks */
    core_bool to_another_character; /** some can transform to any entity in the game*/ 
    core_entity_id which_one; /**< which person*/
} core_transformation_act;

/** Missed Attack
 * -------------------------------------------------------
 */
/** Missed Attack Struct - */
typedef struct {
    core_entity_id source; /**< Who Missed? */
    core_effect_id on_miss_effect; /**< What happens to them if they miss? */
    core_scope_id scope_id; /** Does it affect anyone else? */
    core_bool is_active; /** is it active? */
} core_miss_trigger;

/** Chain Ability
 * -------------------------------------------------------
 */
/** Chain Ability Struct - Abilities that play after one another*/
typedef struct {
    core_ability_id chain_ids[CORE_ABILITY_SLOTS_MAX]; /* In what Sequence*/
    core_u32 chain_count; /** How many parcipate */
    core_u32 current_index; /* Current Person in the Chain*/
} core_chain_attack;

/** Summon Entity
 * -------------------------------------------------------
 */
/** Summoned Buddy Struct - Sets up the Summoned Entity*/
typedef struct {
    core_u32 summoner_lvl; /*< Spawned Entity Matches Summoner Level*/
    core_rarity min_rarity; /*< Rarity Floor*/
    core_entity_id spawned_id; /*< Who Appears*/
    core_bool is_active;
} core_summon;



typedef core_u32 core_summon_id;
#define CORE_SUMMON_NONE 0
#define CORE_SUMMON_MAX 4

/** Summoned Entity
 * -------------------------------------------------------
 */
/** Summoned Buddy Struct - Setup for the amt of creatures to summon*/
typedef struct {
    core_summon_id ids[CORE_SUMMON_MAX]; /**< Amount of Entities Summmoned ids*/
    core_entity_id handles[CORE_SUMMON_MAX]; /**< The Entities Configurations The Entities themselves*/
    core_u32 count; /**< Amount Currently there*/
    core_u32 potency_id; /**< Strength handled by C++ */
} core_summoned_buddy;



#define CORE_CONDITION_NONE 0
#define CORE_SCOPE_NONE     0
 
/** Passive Stat
 * -------------------------------------------------------
 */
/** Passive Stat Struct - Setup a Passive Stat for an Entity*/
typedef struct {
    core_zeal_id zeal_id;
    core_scope_id scope_id;
    core_condition_id condition_id;
    core_bool has_condition_id;
    core_bool is_active;
} core_passive;

#define CORE_PASSIVE_SLOT_MAX 4

/** Passive Stat Pool
 * -------------------------------------------------------
 */
/** Passive Pool Struct - Setups the amount of passive abilities this Entity has*/
typedef struct {
    core_passive slots[CORE_PASSIVE_SLOT_MAX];
    core_u32 count;
} core_passive_pool;

/** Combat Action
 * -------------------------------------------------------
 */
/** Combat Action Struct - Attack Setup for an Entity*/
typedef struct {
    core_ability_id action_id; /**< What Action is taking place */
    core_entity_id source; /**< The Character peforming this action? */
    core_entity_id target; /**< The Target Entity that recieves this */
    core_u32 priority; /**< How important is this action */
    core_bool can_interrupt; /**< this this action be interupted */
} core_combat_action;

#define CORE_ACTION_STACK_MAX 16

/** Action Pool Stack
 * -------------------------------------------------------
 */
/** Action Stack Struct - Amount of Attacks Stacked*/
typedef struct {
    core_combat_action stack[CORE_ACTION_STACK_MAX]; /**< The MAX amount of stacked abilities */
    core_u32 count; /**< Current Amount */
} core_action_stack;



/** Combat Context
 * -------------------------------------------------------
 */
/** Combat Struct - Full combat context for an entity*/
typedef struct {
    core_ability_loadout abilities; /**< Amount of abilities the entity has (5 - 6) */
    core_corrupted_state corruption; /**< Corrupted State if it happens */
    core_low_stat_pool thresholds; /**< Triggers on Low Stats */
    core_transformation_act transform; /**< Transforms if it does happen */
    core_miss_trigger on_miss; //**< If the Attacks misses what happens */
    core_chain_attack chain; //**< Any attacks Chain? */
    core_summon summon; //**< If the Entity can Summon - Let them do so*/
    core_bool can_summon; //**< If the Entity can Summon (Gate)*/
    core_summoned_buddy summons; /**< Tracks how many has been summoned */
    core_passive_pool passives; //**< Amount of passives the Entity has */
    core_action_stack actions; //** Amount of Actions the Entity has*/
} core_combat;



#ifdef __cplusplus
}
#endif

#endif