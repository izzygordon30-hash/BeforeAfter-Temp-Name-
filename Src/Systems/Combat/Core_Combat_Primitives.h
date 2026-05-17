#ifndef CORE_COMBAT_PRIMITIVES_H
#define CORE_COMBAT_PRIMITIVES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Src/Core/Core.h"

/**----------------------------------------------------------
 * Core Combat Primitives
 * -----------------------------------------------------------
 * This File defines the "combat instruction language".
 * 
 * - C defines the intruction format (stable ABI).
 * - C++ defines the meaning + execution (AdditionMachine).
 * - No gameplay logic exists here.
 * -----------------------------------------------------------
 */

/**
 * ## Combat Primitive Type
 * Identifies what kind of action this primitive represents.
 * 
 * Notes
 * - These are abstract verbs 
 *   - Modify: Change any Value
 *   - Move: Transfer any thing.
 *   - etc....
 * - C++ interprets args based on primitive type.
 * - Add new primitives only if truly necessary.
 */

typedef enum core_primitive_type {
    
    /**
     * ## None
     * Empty Primitive
     */
    CORE_PRIMITIVE_NONE = 0,

    /**
     * ## Modify 
     * Change Stat Value
     */
    CORE_PRIMITIVE_MODIFY,
    /**
     * ## Move
     * Transfer/Redirect
     */
    CORE_PRIMITIVE_MOVE,
    /**
     * ## Lock
     * Freeze Values. Prevent Change.
     */
    CORE_PRIMITIVE_LOCK,

    /**
     * ## Gate
     * Conditional Execution
     */
    CORE_PRIMITIVE_GATE,
    /**
     * ## Time
     * Delay / Accelerate / rewind.
     */
    CORE_PRIMITIVE_TIME,
    /**
     * ## Order 
     * Change Priority, Interupt, queue.
     */
    CORE_PRIMITIVE_ORDER,
    /**
     * ## Chance
     * Random Execution
     */
    CORE_PRIMITIVE_CHANCE,

    /**
     * ## Spawn
     * Summon, Craete, Destroy
     */
    CORE_PRIMITIVE_SPAWN,
    /**
     * ## Reveal
     * Stealth/ Hidden/ Scan
     */
    CORE_PRIMITIVE_REVEAL,
    /**
     * ## Scope
     * Affect Area/ Group
     */
    CORE_PRIMITIVE_SCOPE,
    
    /**
     * ## Bond
     * Link Entities
     */
    CORE_PRIMITIVE_BOND,
    /**
     * ## Forge
     * Mutate Identity / Meta
     */
    CORE_PRIMITIVE_FORGE,
    /**
     * ## Inherit
     * Absorb Properties
     */
    CORE_PRIMITIVE_INHERIT,

    /**
     * ## Convert
     * Convert Something to Something else
     * Buff -> Debuff -> Buff
     */
    CORE_PRIMITIVE_CONVERT,
    /**
     * ## Restore
     * Revert to baseline original
     */
    CORE_PRIMITIVE_RESTORE,
    /**
     * ## Lock State
     * Freeze current state flags
     */
    CORE_PRIMITIVE_LOCK_STATE,
    
    /**
     * ## Recall
     * Restore past snapshot.
     */
    CORE_PRIMITIVE_RECALL,
    /**
     * ## Echo
     * Replay Last Event
     */
    CORE_PRIMITIVE_ECHO,
    /**
     * ## Unwrite
     * Delete Event / History Impact
     */
    CORE_PRIMITIVE_UNWRITE,


    /**
     * ## Sheild
     * Barrier Intercept Damage
     */
    CORE_PRIMITIVE_SHEILD,
    /**
     * ## Pierce
     * Bypass Defense Rules
     */
    CORE_PRIMITIVE_PIERCE,
    /**
     * ## Mark
     * Tag for future triggers
     */
    CORE_PRIMITIVE_MARK,
    /**
     * ## Diverge
     * Split attack / effect into multiple
     */
    CORE_PRIMITIVE_DIVERGE,

    /**
     * ## Sense
     * Read info without acting
     */
    CORE_PRIMITIVE_SENSE,
    /**
     * ## Weight
     * Modify turn order weight
     */
    CORE_PRIMITIVE_WEIGHT,

    /**
     * ## Count
     * Primitive Counter
     */
    CORE_PRIMITIVE_COUNT,

} core_primitive_type;

/**
 * ## Argument Slot Type
 * Flexible Parameter slot.
 * 
 * - interpreted by C++ based on primitive type
 * - Support int, floats, ids, raw flags, and pointers
 */
typedef union core_primitive_arg {
    core_i32 i;
    core_u32 u;
    core_f32 f;
    core_u64 raw;
    void* ptr;
} core_primitive_arg;

/**
 * ## Argument Slot
 * Max number of args per primative.
 * 16 (Make it squared amount) is enough for almost mechanic.
 */
#define CORE_PRIMITIVE_ARG_MAX 16


/**
 * ## Combat Primitive
 * Universal instructions container.
 * 
 * Everything beyond this is handled by combat executor systems.
 * 
 * C defines the stable memory layout.
 * C++ resolves behavior + meaning
 */
typedef struct core_primitive {
    /**
     * ## Primitive Type
     * What Primitive is it.
    */
    core_primitive_type type;
    /** 
     * ## Generic Parameters
     * Primitive Arguements
     */
    core_primitive_arg args[CORE_PRIMITIVE_ARG_MAX];

    /**
     * ## Scope
     * Target Definition (C++ resolves meaning)
     */
    core_scope_id scope;

    /**
     * ## Source
     * Who caused the primitive to set off
     */
    core_entity_id source;

    /**
     * ## Target 
     * Primary Target Entity
     */
    core_entity_id target;

    /**
     * ## Timer
     * Scheduling Timer (Warmup, delay, duration, etc..)
     */
    core_timer timer;

    /**
     * ## Priority
     * Priority in action queue
     */
    core_u32 priority;

    /**
     * ## Debug ID
     * Maps back to ability step or DSL line
     */
    core_u32 debug_id;

} core_primitive;

/**
 * 4. Execution Contract (C++ Side)
 */

/**
 * ## Primitive Executor
 * 
 * C++ executor function signature
 * 
 * world_context is opaque (combat context, world , ECS, etc.)
 */
typedef void (*core_primitive_executor)(const core_primitive* primative, void* world_context);

/**
 * =====================================================
 * 5. Declaration/ Registration Macros
 * ======================================================
 */

/**
 * ## Define Primitive Handler (C++ Side)
 */
#define CORE_DECLARE_PRIMITIVE_HANDLER(name) \
    void name(const core_primitive* primitive, void* world_context)

/**
 * ## Bind Primitive Type to Handler
 * (Used inside executor registry)
 */
#define CORE_BIND_PRIMITIVE(type_enum, function) \
    g_core_primitive_handlers[(type_enum)] = (function)


/**
 * =====================================================
 * 6. Default Construction Helpers
 * ====================================================
 */

/**
 * ## Create Primitive (zero-init safe :) )
 */

 #define CORE_PRIMITIVE_MAKE(type_val) \
    (core_primitive){ .type = (type_val)}; 


/**
 * ## Set Primitive Arg (raw)(Unsigned Int Variant(64bits))
 * (Most Stable for IDs, flags, packed values. Keeps values lossless and avoids
 * float precision issues )
 */
#define CORE_ARG_RAW(p, index, value) \
    (p).args[(index)].raw = (core_u64)(value)

/**
 * ## Set Primitive Arg (Unsigned Int Variant(32bits))
 */
#define CORE_ARG_U32(p, index, value) \
    (p).args[(index)].u = (core_u32)(value)


/**
 * ## Set Primitive Arg (Int Variant(32bits))
 */
#define CORE_ARG_I32(p, index, value) \
    (p).args[(index)].i = (core_i32)(value)


/**
 * ## Set Primitive Arg (Float Variant(32bits))
 */
#define CORE_ARG_F32(p, index, value) \
    (p).args[(index)].f = (core_f32)(value)


/**
 * ## Set Primitive Arg (Pointer Variant)
 */
#define CORE_ARG_PTR(p, index, value) \
    (p).args[(index)].ptr = (void*)(value)


/** 
 * =============================================
 * 7. Executor Table Declaration 
 * =============================================
*/

/**
 *  ## Global Primitive Handler Table
 * C++ must define table and fill it.
 * 
 * Example:
 * g_core_primitive_handlers[CORE_PRIMITIVE_MODIFY] = exec_modify;
 */
extern core_primitive_executor g_core_primitive_handlers[CORE_PRIMITIVE_COUNT];




#ifdef __cplusplus
}
#endif

#endif /* CORE_COMBAT_PRIMITIVES_H */