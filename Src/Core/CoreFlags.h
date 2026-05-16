#ifndef COREFLAGS_H
#define COREFLAGS_H

#include "CoreTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

/**=================================================================
* ECS Conponent Mask
* -----------------------------------------------------------------
* Hint Layer - Not Ownership or Logic.
* Tell C++ ECS which compontents are active on an entity.
* Used for batch processing and optimization only.
==================================================================*/

/** 64 bit mask storing which compontents an entity has */
typedef core_u64 core_component_mask;


/**
 * ## Rarity Flags
 * BitFlags for Rarity
 * BitFlags mean they can be combined using bitwise OR to represent multiple rarities (e.g., common + rare).
 */
typedef core_u32 core_rarity_flags;

/**
 * ## Gender Flags
 * BitFlags for Gender
 * Check Rarity for example of bitflag use.
 */
typedef core_u32 core_gender_flags;

/**
 * ## Size Flags
 * BitFlags for Size
 * Check Rarity for example of bitflag use.
 */
typedef core_u32 core_size_flags;

/**=================================================================
*  State Flags
* --------------------------------------------------------------------
* Single Unified Bitfields for runtime entity state.
* C++ Systems read and write these each tick.
==================================================================*/

/**
 * ## State Flags
 * 32 bit for storing states for an entity.
*/
typedef core_u32 core_state_flags;









/** ## Component Bits */
/* BitFlag Flags for Entity Components*/
typedef enum {
    /** ## Entity Position */
    CORE_CMP_POSITION = 1ull << 0,
    /** ## Entity Velocity */
    CORE_CMP_VELOCITY = 1ull << 1,
    /** ## Entity Stats */
    CORE_CMP_STATS    = 1ull << 2,
    /** ## Entity Effects */
    CORE_CMP_EFFECTS  = 1ull << 3,
    /** ## Entity Zeals(Passives) */
    CORE_CMP_ZEALS    = 1ull << 4,
    /** ## Entity Bonds(Class/Relationships) */
    CORE_CMP_BONDS    = 1ull << 5,
    /** ## Entity Attack/Defense */
    CORE_CMP_COMBAT   = 1ull << 6,
    /** ## Entity-Items Connection(Equip, Use...) */
    CORE_CMP_ITEM     = 1ull << 7,
} core_component_bits;

/**=================================================================
* Rarity + Type
* --------------------------------------------------------------------
* Characters Rarity + Type (Attacker, Tank, Support)
==================================================================*/

/**
 * ## Rarity 
 * ---------------------------------------------
 * Rarity of the General thing Applied to it.
 * Common, Uncommon, Rare, Legendary and Superb.
 */
typedef enum {
    /** ### Common 
     * Almost Guaranteed
    */
    CORE_RARITY_COMMON = 1u << 0,
    /** ### Uncommon 
     * Okay You might get this pretty easily
    */
    CORE_RARITY_UNCOMMON = 1u << 1,
    /** ### Rare
     * Medium Difficulty to recieve
    */
    CORE_RARITY_RARE = 1u << 2,
    /** ### Lengendary
     * Hard to get*/
    CORE_RARITY_LEGENDARY = 1u << 3,
    /** ### Superb 
     * Highest Rarity in the Game (Yea hope you get one)
    */
    CORE_RARITY_SUPERB = 1u << 4,
} core_rarity_bits;

/**=================================================================
* Gender
* --------------------------------------------------------------------
* None
* Male
* Female
* Unknown
* Other
==================================================================*/



/**
 * ### Gender of the Character
 * --------------------------
 * None, Male, Female, Other.
 */
typedef enum {
    /** No Gender Assigned at all*/
    CORE_GENDER_NONE    = 1u << 0,
    /** No Gender Assigned at all*/
    CORE_GENDER_MALE    = 1u << 1,
    /** No Gender Assigned at all*/
    CORE_GENDER_FEMALE  = 1u << 2,
    /** No Gender Assigned at all*/
    CORE_GENDER_OTHER   = 1u << 3,
} core_gender_bits;

/**=================================================================
* Size
* --------------------------------------------------------------------
* Used by world rules: collision, hitbox, AI targeting, 
* Things regarding their weight/size etc..
*
==================================================================*/


/**
 * ## Size of the Character
 * Small, medium, large, bigback
 */
typedef enum {
    /** ## None 
     * Size at all / Ghost*/
    CORE_SIZE_NONE     = 1u << 0,
    /** ## Small 
     * can fit into creases*/
    CORE_SIZE_SMALL    = 1u << 1,
    /** ## Medium Size 
     * Normal size
    */
    CORE_SIZE_MEDIUM    = 1u << 2,
    /** ## Large
     * Bigger
    */
    CORE_SIZE_LARGE    = 1u << 3,
    /** ## BigBack
     *  Very Large Size in the game
    */
    CORE_SIZE_BIGBACK  = 1u << 4,
    /** ## Giant
     *  Biggest Size in the Game
    */
    CORE_SIZE_GIANT = 1u << 5,
} core_size_bits;


typedef enum {
    /**
     * ## Can Move
     * Can the Character Move
    */
    CORE_CAN_MOVE       = 1 << 0,
    /**
     * ## Can Act
     * Can the Character Act
    */
    CORE_CAN_ACT        = 1 << 1,
    /**
     * ## Can Attack
     * Can the Character Attack
    */
    CORE_CAN_ATTACK     = 1 << 2,
    /**
     * ## Can Defend
     * Can the Character Defend
    */
    CORE_CAN_DEFEND     = 1 << 3,

    /**
     * ## Is Frozen
     * Is the Character Frozen
    */
    CORE_IS_FROZEN      = 1 << 4,
    /**
     * ## Is Stunned
     * Is the Character Stunned
    */
    CORE_IS_STUNNED     = 1 << 5,
    /**
     * ## On Fire
     * Is the Character on Fire
    */
    CORE_IS_ON_FIRE     = 1 << 6,

    /**
     * ## Silenced
     * Is the Character Silenced
    */
    CORE_SILENCED       = 1 << 7,
    /**
     * ## Locked
     * Is the Character Locked
    */
    CORE_LOCKED         = 1 << 8,
    /**
     * ## Is Invulnerable
     * Is the Character Invulnerable
    */
    CORE_INVULNERABLE   = 1 << 9,
    /**
     * ## Is Dead
     * Is the Character Dead
    */
    CORE_IS_DEAD        = 1 << 10,
} core_state_flag_bits;

/**=================================================================
* Item
* --------------------------------------------------------------------
* LightWeight Item Data. No Logic.
*
*
==================================================================*/

typedef core_u32 core_item_type_flag;
/**
 * ## Item Type
 *  What Kind of Item is it
*/
typedef enum {
    /**
     * ## None (Item)
     * Not an Item.
     */
    CORE_ITEM_NONE = 1u << 0,
    /**
     * ## Consumable
     * Food Item.
     */
    CORE_ITEM_CONSUMABLE = 1u << 1,
    /**
     * ## Equipment
     * Armour, Sword (any Equipable Item)
     */
    CORE_ITEM_EQUIPMENT = 1u << 2,
    /**
     * ## Item Key
     * 
     */
    CORE_ITEM_KEY =  1u << 3,
    /**
     * ## Item Material
     * Used to Make Something.
     */
    CORE_ITEM_MATERIAL = 1u << 4,
    /**
     * ## Item Card
     * 
     */
    CORE_ITEM_CARD = 1u << 5,
} core_item_type;



#ifdef __cplusplus
}
#endif

#endif