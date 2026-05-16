#ifndef COREMETA_H
#define COREMETA_H

#include "CoreTypes.h"

#ifdef __cplusplus
extern "C" {
#endif





/**=================================================================
*  Info Entry
* --------------------------------------------------------------------
* Universal MetaData Container - Not Character-Only. 
* Works for: entities, items, abilities, UI, Lore - In general.
* All IDs point into the string table - no raw strings.
==================================================================*/

/**
 * Info entry
 * --------------------------------------------------------------------------
 * Generic Info Data for any Compontent!
 */
typedef struct {
    /**
     * ### Name
     * String Table ID for the Display Name of this Object (Entity, Item, Ability, etc.)
     */
    core_string_id name;
    /**
     * ### Description
     * String Table ID for the Display Description of this Object (Entity, Item, Ability, etc.)
     */
    core_string_id description;
    /**
     * ## Extra
     * String Table ID for any Extra Information about this Object (Entity, Item, Ability, etc.) - Optional
     */
    core_string_id extra;
} core_info_block;

#ifdef __cplusplus
}
#endif

#endif /* CORE_TYPES_H */