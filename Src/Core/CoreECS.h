#ifndef CORE_ECS_H
#define CORE_ECS_H

#include "Core.h"

#ifdef __cplusplus
extern "C" {
#endif

/**=================================================================
* Entity C System(ECS) View
* --------------------------------------------------------------------
* Optional flattened view for batch processing.
* Populated by C++ ECS layer from core_entity*.
* Does not own memory - points to world storage.
==================================================================*/

// to comment
typedef struct {
    core_entity_id id;
    core_state_flags state;
    core_i32 hp;
    core_vec3 position;
    core_vec3 velocity;
    core_species_id species;
    core_class_id class_type;
    core_component_mask mask;
} core_ecs_entity;


#ifdef __cplusplus
}
#endif

#endif