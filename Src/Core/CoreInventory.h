#ifndef CORE_INVENTORY_H
#define CORE_INVENTORY_H

#include "Core.h"

#ifdef __cplusplus
extern "C" {
#endif

/**=================================================================
*  Inventory Slot + Inventory
* ------------------------------------------------------------------
* Inventory Slot - Holds an Item and its stack count.
* Inventory - Holds a fixed array of Inventory Slots, total count and weight.
==================================================================*/

/**
 * ## Inventory Slot
 * --------------------------------------------------------------------
 * Holds an Item and its stack count.
 */
typedef struct {
    /** ## Item ID
     *  The ID of the Item in the Slot
     */
    core_item_id item_id;
    /** ## Stack
     *  The Amount of the Item in the Slot
     */
    core_u32     stack;
} core_inventory_slot;

/*
 * ## Inventory Max
 * Max Amount of Items in the Inventory
 */
#define CORE_INVENTORY_MAX 32

/**
 * ## Inventory
 * Items a Character/Entity has. 32 Slots, Total Count and Weight.
 */
typedef struct {
    /**
     * ## Slots
     * Inventory Slots
     */
    core_inventory_slot slots[CORE_INVENTORY_MAX];
    /** ## Count
     *  The Number of Items in the Inventory
     */
    core_u32 count;
    /** ## Weight
     *  The Total Weight of the Items in the Inventory
     *  slows you down.
     */
    core_f32 weight; 
} core_inventory;




#ifdef __cplusplus
}
#endif

#endif