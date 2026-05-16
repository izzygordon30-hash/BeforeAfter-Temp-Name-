#ifndef CORE_H
#define CORE_H

#if !defined(CORE_TYPES_H)
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "CoreTypes.h"
#include "CoreIDs.h"
#include "CoreMath.h"
#include "CoreFlags.h"
#include "CoreStats.h"
#include "CoreCombatTypes.h"
#include "CoreInventory.h"
#include "CoreSummons.h"
#include "CoreRelations.h"
#include "CoreResistances.h"
#include "CoreECS.h"
#include "CoreEntity.h"

/** Core.h 
 * -------------------------------------------------------------
 * One Source of Truth. Describes what Exists.
 * Slight behaviour. No Gameplay logic. No Systems(Like Combat)
 * 
 * Rule:
 * - "Describes What something is?" -> Belongs Here
 * - "Describes How it behaves?" -> Very Slightly
 * 
 * Depends on Nothing
 * Used by: Combat.h, World.h, C++ Registries, Rust FFI
 */


#ifdef __cplusplus
}
#endif

#endif