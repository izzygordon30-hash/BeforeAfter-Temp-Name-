#ifndef CORE_RESISTANCES_H
#define CORE_RESISTANCES_H

#include "Core.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Max Resiatances
 * Max Amount of Resistances an Entity can have
 */
#define CORE_RESISTANCE_MAX 8


/**
 * ## Resistance
 * Represents a single resistance type and its properties.
 */
typedef struct {
    core_resistance_id resistance_id;   
} core_resistances;

/**
 * ## Resistance Pool
 * Represents a collection of resistances an entity has.
 */
typedef struct {
    core_resistance_id resistances[CORE_RESISTANCE_MAX];   
} core_resistances_pool;

#ifdef __cplusplus
}
#endif

#endif