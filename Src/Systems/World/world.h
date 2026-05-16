#ifndef WORLD_H
#define WORLD_H

#include "Core/Core.h"

#ifdef __cplusplus
extern "C" {
#endif


core_bool world_init(core_world* world, core_u32 capacity);

void world_shutdown(core_world* world);

core_entity_id world_spawn(core_world* world, core_vec2 pos);

core_bool world_destroy(core_world* world, core_entity_id id);

core_entity* world_get(const core_world* world, core_entity_id id);

core_bool world_is_valid(const core_world* world, core_entity_id id);

#ifdef __cplusplus
}
#endif

#endif