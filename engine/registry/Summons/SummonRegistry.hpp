#pragma once
/** SummonRegistry.hpp 
 *
 * Summons are Registered Here. 
 * C stores summon IDS in the core_summon_pool
 * C++ defines them futher to what they mean here
 * 
 * Every summon is a real entity in the battle/world.
 * 
 * Summons can:
 * - Inherit stats from summoner (scaled by potency)
 * - Have their own passive zeals
 * - Inherit back with summoner (DONUTS WELCOME AND SEA BUD's bulldogs)
 * - Scale with summoner level (random summon mechanic)
 * 
 * Examples of Summons from Class (DONUTS WELCOME):
 * - Bulldog -> aggressive, charges nearest enemy
 * - Car -> area denial, blocks movement zone
 * - Backup -> random cops spawned
*/

#include "RegistryContext.hpp"
#include "C:\BeforeAfter\ContractGuards.hpp"
#include <flat_map>
#include <generator>
#include <string_view>
#include <functional>
 
extern "C" {
    #include "Combat.h"
}

class SummonRegistry;

/** 
 * ## Summon Callbacks
 * C++ owns all behavior.
 */
struct SummonCallbacks {
    using Fn = std::function<void(core_entity*, core_entity*, SummonRegistry&)>;

    /**
     * ### On Spawn
     * Summon First enters the battle/world
     */
    Fn on_spawn;
    /**
     * ### On Tick
     * Every tick while summon is alive
     */
    Fn on_tick;
    /**
     * ### On Death
     * Summmon is Destroyed - notify summoner/caller
     */
    Fn on_death;
    /**
     * ### On Dismiss
     * Summoner Manually dismisses summon
     */
    Fn on_dismiss; 
};

struct SummonerDefinition {
    core_summon_id id;
    core_view_only_str name;
    core_u32 info_id;
    core_u32 description_id;
    core_u32 sprite_id;

    core_stats base_stats;
    core_f32 stat_scale;
};