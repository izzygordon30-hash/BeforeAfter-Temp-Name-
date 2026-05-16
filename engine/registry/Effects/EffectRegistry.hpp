#pragma once

/** EffectRegistry.hpp 
 *
 * Effects are Registered Here. 
 * C stores effect IDS in the core_effect_pool
 * C++ defines them futher to what they mean here
 * 
 * Effect LifeStyle
 *   on_apply  → fires when effect first added to pool
 *   on_tick   → fires every tick while active
 *   on_expire → fires when duration hits zero
 *   on_stack  → fires when a new stack is added
 *   on_cleanse→ fires when removed by cleanse ability
*/

#include "RegistryContext.hpp"
#include "C:\BeforeAfter\ContractGuards.hpp"
#include <flat_map>
#include <generator>
#include <string_view>
#include <functional>

// prevents name managling
extern "C" {
    #include "Core.h"
}

/**
 * Foward Declaration
 */
class EffectRegistry;

/**
 * ### Effect Callbacks
 * C++ owns all behaviour. C only stores the IDs and Manages Allocation/Memory.
 * Every Callback Recieves:
 * an `entity*` - who has the effect.
 * an instance - the specific effect slot (stacks, duration etc)
 * registry - access to other registries if needed
 */
struct EffectCallbacks {
    using Fn = std::function<void(
        core_entity*,
        core_effect_instance&,
        EffectRegistry&)>;
    
    /**
     * ### On Apply
     * The Effect first lands on a Entity
     */
    Fn on_apply;
    /**
     * ### On Tick
     * Every Tick While Active
     */
    Fn on_tick;
    /**
     * ### On Expire
     * Duration hits 0
     */
    Fn on_expire;
    /**
     * ### On stack
     * New stack added to existing slot
     */
    Fn on_stack;
    /**
     * ### On Cleanse
     * Remove Effect(Cleanse Ability)
     */
    Fn on_cleanse;
};

/**
 * ## Effect Definition
 * Defines a Effect. What it means and its attributes.
 */

struct EffectDefinition{
    core_effect_id id;
    core_view_only_str name;
    core_view_only_str display_name;
    core_u32 description_id; /** String Table Ref */

    core_i32 min_value;
    core_i32 max_value;

    /** Classifications */
    core_bool is_buff;
    core_bool is_debuff;
    core_bool is_corruption;
    core_bool is_permanant;

    /** Stacking Rules */
    core_bool stackable;
    core_bool can_be_stolen;
    core_bool can_be_cleansed;
    core_bool can_be_reflected;

    core_u32 max_stacks;
    core_timer tick_rate;

    EffectCallbacks callbacks;
};

/**
 * ## EffectRegistry
 * Define a Effect and its Processes
 */
class EffectRegistry {
    public:
        EffectRegistry()  = default;
        ~EffectRegistry() = default;
    /* ---- register ---- */

    /**
     * @brief Register a new effect definition.   
     * @return AlreadyRegistered if id exists. 
     * 
     */
    RegResult<void> register_effect(EffectDefinition def)
        pre(def.id != CORE_EFFECT_NONE)
    {
        if (m_table.contains(def.id))
            return std::unexpected(RegistryError::AlreadyRegistered);

        m_name_index.emplace(def.name, def.id);
        m_table.emplace(def.id, std::move(def));
        return{};
    }

    [[nodiscard]]
       RegResult<const EffectDefinition*> get(core_effect_id id) const noexcept
       pre(id != CORE_EFFECT_NONE){
            auto it = m_table.find(id);
            if(it == m_table.end())
                return std::unexpected(RegistryError::NotFound);
            return &it->second;
       }

    [[nodiscard]]
       RegResult<core_effect_id> get_id(core_view_only_str name) const noexcept {
            auto it = m_name_index.find(name);
            if(it == m_name_index.end())
                return std::unexpected(RegistryError::NotFound);
            return it->second;
    }

    RegResult<void> apply(core_entity* entity, core_effect_id id, core_u32 source_index, core_i32 stacks, core_u32 duration) noexcept
        pre(entity != nullptr)
        pre(id != CORE_EFFECT_NONE)
        {
            auto def_result = get(id);
            if (!def_result) return std::unexpected(def_result.error());
            const auto* def = *def_result;

            core_effect_pool& pool = entity -> effect_pool;

            /**
             * check existing slot - stack if stackable
             */
            for (core_u32 i = 0; i < pool.count; ++i) {
                auto& slot = pool.effects[i];
                if(slot.id != id) continue;
                
                if (def -> stackable) {
                    slot.stacks = std::min(slot.stacks + stacks, static_cast<core_i32>(def -> max_stacks));
                    if (def -> callbacks.on_stack)
                        def -> callbacks.on_stack(entity, slot, *this);
                }
                return {};
            }
        

            /* pool full*/

            if (pool.count >= CORE_EFFECT_POOL_MAX)
                return std::unexpected(RegistryError::CapacityExceeded);
            
            /* new slot*/
            auto& slot = pool.effects[pool.count++];
            slot.id = id;
            slot.source = source_index;
            slot.stacks = std::min(slot.stacks + stacks, static_cast<core_i32>(def -> max_stacks));
            slot.duration = duration;

            if (def->callbacks.on_apply)
                def->callbacks.on_apply(entity, slot, *this);
                
            return {};
    }

    void tick(core_entity* entity) noexcept
        pre(entity != nullptr)
        {

            if (!entity) return;

            core_effect_pool& pool = entity -> effect_pool;
            core_u32 write = 0;

            for (core_u32 i = 0; i < pool.count; ++i) {
                auto& slot = pool.effects[i];
                
                auto def_result = get(slot.id);
                if (!def_result) continue;
                const auto* def = *def_result;

                /** Tick */
                if (def -> callbacks.on_tick) {
                    def->callbacks.on_tick(entity, slot, *this);
                }
                /* Duration Countdown*/
                bool should_remove = false;
                    if(slot.duration > 0) {
                        --slot.duration;
                        if (slot.duration == 0) {
                            if (def -> callbacks.on_expire)
                                def -> callbacks.on_expire(entity, slot, *this);
                            should_remove = true;
                        }
                    }
                    if (should_remove) {
                        continue; /* Drop Effect */
                    }

                /** keep slot */
                if (write != i){
                    pool.effects[write] = slot;
                }
                ++write;
            }
            pool.count = write;
        }
            
    RegResult<void> cleanse(core_entity* entity, core_effect_id id) noexcept
        pre(entity != nullptr)

    {
        auto def_result = get(id);
        if (!def_result) return std::unexpected(def_result.error());
        const auto* def = *def_result;
        
        if (!def->can_be_cleansed) 
            return std::unexpected(RegistryError::InvalidData);

        core_effect_pool& pool = entity -> effect_pool;
        core_u32 write = 0;

        for ( core_u32 i = 0; i < pool.count; ++i) {
            if (pool.effects[i].id == id) {
                if (def -> callbacks.on_cleanse)
                def -> callbacks.on_cleanse(entity, pool.effects[i], *this);
                continue; /* Remove */
            }
            if (write != i) 
                pool.effects[write] = pool.effects[i];
            ++write;
        }   
        pool.count = write;
        return {};
    }

    RegResult<void> cleanse_by_source(core_entity* entity, core_u32 source_index) noexcept
        pre(entity != nullptr)

    {
        core_effect_pool& pool = entity -> effect_pool;
        core_u32 write = 0;

        for ( core_u32 i = 0; i < pool.count; ++i) {
            auto& slot = pool.effects[i];
            if (slot.source != source_index) {
                if (write != i) 
                    pool.effects[write] = slot;
                ++write;
                continue; /* Drop */
            }
            auto def_result = get(slot.id);
            if (def_result && (*def_result) -> callbacks.on_cleanse) {
                (*def_result) -> callbacks.on_cleanse(entity, slot, *this);
            }
        }   
        pool.count = write;
    }

    RegResult<void> steal(core_entity* from, core_entity* to, core_effect_id id) noexcept
        pre(from != nullptr)
        pre(to != nullptr)
        pre(from != to)
    {
        auto def_result = get(id);
        if (!def_result) return std::unexpected(def_result.error());
        const auto* def = *def_result;

        if (!def -> can_be_stolen)
            return std::unexpected(RegistryError::InvalidData);

        core_effect_pool& from_pool = from->effect_pool;

        for (core_u32 i = 0; i < from_pool.count; ++i){
            if (from_pool.effects[i].id != id) continue;

            auto stolen = from_pool.effects[i];

            /* remove from source */
            for (core_u32 j = i; j < from_pool.count - 1; ++j)
                from_pool.effects[j] = from_pool.effects[j + 1];
            --from_pool.count;

            /* apply to target */
            return apply(to, stolen.id, stolen.source, stolen.stacks, stolen.duration);

        }
        return std::unexpected(RegistryError::NotFound);
    }

       [[nodiscard]]
       std::generator<const EffectDefinition&> all() const {
            for (const auto& [id, def] : m_table) co_yield def;
       }

       [[nodiscard]]
       std::generator<const EffectDefinition&> buffs() const {
            for (const auto& [id, def] : m_table)
                if(def.is_buff) co_yield def;
       }

       [[nodiscard]]
       std::generator<const EffectDefinition&> debuffs() const {
            for (const auto& [id, def] : m_table)
                if(def.is_debuff) co_yield def;
       }

       [[nodiscard]]
       std::size_t count() const noexcept {
            return m_table.size();
       }

    private:
       std::flat_map<core_u32, EffectDefinition> m_table;
       std::flat_map<core_view_only_str, core_u32> m_name_index;
};