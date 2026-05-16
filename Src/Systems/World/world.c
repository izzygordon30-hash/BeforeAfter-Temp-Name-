#include "world.h"
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

typedef struct {
    core_entity* entities;
    core_u32* generations;
} world_storage;

/**
 * 
 * @brief Initializes World Storage [Array]
 * @param world world object.
 * 
 * 
 * @return storage
 * 
 */
static world_storage world_storage_get(core_world* world){
    world_storage storage;
    storage.entities = world -> entities;
    storage.generations = (core_u32*)(world -> entities + world -> capacity); //Casts the merged array to type int u32
    return storage;
}

// World API

core_bool world_init(core_world* world, core_u32 capacity) {

    if (!world || capacity == 0) return false;

    world->tick = 0;
    world->entity_count = 0;
    world->capacity = capacity;

    size_t entity_bytes = sizeof(core_entity) * capacity;
    size_t gen_bytes    = sizeof(core_u32)    * capacity;

    
}