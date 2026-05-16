#pragma once

/** ZealRegistry.hpp 
 *
 * Zeals are Registered Here. 
 * C stores zeal IDS in the core_zeal_pool ( Max 8 per entity)
 * C++ defines them futher to what they mean here
 * 
 * Zeals are Passive(No Trigger)
 * They can check a condition every tick silently
 * Condition != Trigger:
 *  Trigger = smth that FIRES an ability
 *  Condition = something a passive CHECKS constantly
 * 
 *  Cross-Entity Zeals (is_cross_entity = true):
 * Affects other entities in the world - not just self.
 * Example "I Wanna Go Home" fires on all introverts
 * in range, tanks taunt, attackers buff up
*/

// Reminder Entity still has not been wrapped nor do eneities have types like attack tank...etcc reminder to fix this

#include "RegistryContext.hpp"
#include "ContractGuards.hpp"
#include <flat_map>
#include <generator>
#include <string_view>
#include <functional>

// prevents name managling
extern "C" {
    #include "Core.h"
}

/**
 * ### Zeal Scope
 * Who a Zeal affects when its condition is met.
*/
enum class ZealScope : core_u8 {
    /**
     * ### Self
     *  only the entity that owns it 
     */
    Self, 
    /**
     *  ### Class
     *  all same-class peeps gets it 
    */
    Class,
    /**
     * ### Bond
     *  bonded partners only */
    Bond, 
    /**
     * ### Rival
     *  Triggers against rival class */
    Rival, 
    /**
     * ### All Allies
     *  every ally regardless of class */
    AllAllies, 
    /**
     * ### All Enemies
     *  every enemy regardless of class */
    AllEnemies, 
    /**
     * ### Conditional
     * scope determined by condition result so go nuts */
    Conditional, 
};

/** Forward Declaration */
class ZealRegistry; 

struct ZealCallbacks {
    /* condition - checked every tick 
        returns true = zeal is active this tick
        returns false = zeal is dormant this tick 
        null = always active 
    */

    using ConditionFn = std::function<bool(
        const core_entity*, const core_zeal_instance&, ZealRegistry&)>;

    using ActionFn = std::function<void(
        core_entity*, const core_zeal_instance&, ZealRegistry&)>;
    

    /**
     * ### On Condition
     * checked every tick silently
     */
    ConditionFn condition;
    /**
     * ### On Apply
     * The Effect first lands on a Entity
     */
    ActionFn on_apply;
    /**
     * ### On Tick
     * Every Tick While Active
     */
    ActionFn on_tick;
    /**
     * ### Trigger
     * When the Condition returns true.
     */
    ActionFn on_trigger;
    /**
     * ### On Remove
     * Remove Zeal From Entity
     */
    ActionFn on_remove;
};

/**
 * ## Zeal Definition
 * Defines a Zeal. What it means and its attributes.
 */

struct ZealDefinition {
    core_zeal_id id;
    core_view_only_str name;
    core_view_only_str display_name;
    core_u32 description_id; /** String Table Ref */

    ZealScope scope;

    core_bool is_cross_entity; /** Affects Other Entities */
    core_bool requires_condition; /** False = always active */
    core_bool can_be_stolen; /** MOVE primitive applies */
    core_bool can_stack;
    core_u32 max_stacks;

    ZealCallbacks callbacks;
};

/**
 * ## ZealRegistry
 * Define a Zeal and its Processes
 */
class ZealRegistry {
    public:
        /* ---- register ---- */
 
        /**
         * @brief Register a new zeal definition.   
         * @return AlreadyRegistered if id exists.
         */
        RegResult<void> register_zeal(ZealDefinition def)
            pre(def.id != CORE_ZEAL_NONE)
        {
            if (m_table.contains(def.id))
                return std::unexpected(RegistryError::AlreadyRegistered);

            m_name_index.emplace(def.name, def.id);
            m_table.emplace(def.id, std::move(def));
            return{};
        }

        /* lookup */
        [[nodiscard]]
        RegResult<const ZealDefinition*> get(core_zeal_id id) const noexcept
        pre(id != CORE_ZEAL_NONE){
            auto it = m_table.find(id);
            if(it == m_table.end())
                return std::unexpected(RegistryError::NotFound);
            return &it->second;
        }

        [[nodiscard]]
        RegResult<core_zeal_id> get_id(core_view_only_str name) const noexcept {
            auto it = m_name_index.find(name);
            if(it == m_name_index.end())
                return std::unexpected(RegistryError::NotFound);
            return it->second;
        }
        
        RegResult<void> apply(core_entity* entity, core_zeal_id id, core_u32 source_index, core_i32 stacks, core_u32 duration) noexcept
            pre(entity != nullptr)
            pre(id != CORE_ZEAL_NONE)
            {
                auto def_result = get(id);
                if (!def_result) return std::unexpected(def_result.error());
                const auto* def = *def_result;

                core_zeal_pool& pool = entity -> zeal_pool;

                /**
                 * check existing slot - stack if stackable
                 */
                for (core_u32 i = 0; i < pool.count; ++i) {
                    auto& slot = pool.slots[i];
                    if(slot.id != id) continue;
                    
                    if (def -> can_stack) {
                        slot.stacks = std::min(slot.stacks + stacks, static_cast<core_i32>(def -> max_stacks));
                    }
                    return {};
                }
            

                /* pool full*/

                if (pool.count >= CORE_ZEAL_POOL_MAX)
                    return std::unexpected(RegistryError::CapacityExceeded);
                
                /* new slot */
                auto& slot = pool.slots[pool.count++];
                slot.id = id;
                slot.source = source_index;
                slot.stacks = std::min(stacks, static_cast<core_i32>(def -> max_stacks));
                slot.duration = duration;

                if (def->callbacks.on_apply)
                    def->callbacks.on_apply(entity, slot, *this);
                    
                return {};
        }

        
    void tick(core_entity* entity) noexcept
        pre(entity != nullptr)
        {

            if (!entity) return;

            core_zeal_pool& pool = entity -> zeal_pool;
            core_u32 write = 0;

            for (core_u32 i = 0; i < pool.count; ++i) {
                auto& slot = pool.slots[i];
                
                auto def_result = get(slot.id);
                if (!def_result) continue;
                const auto* def = *def_result;

                core_bool active = !def -> requires_condition;
                    if (def -> callbacks.condition) {
                        active = def -> callbacks.condition(entity, slot, *this);
                }

                if (active) {
                    if (def -> callbacks.on_tick)
                        def -> callbacks.on_tick(entity, slot, *this);
                    if (def -> callbacks.on_trigger)
                        def -> callbacks.on_trigger(entity, slot, *this);
                }
                /* Duration Countdown*/
                bool should_remove = false;
                    if(slot.duration > 0) {
                        --slot.duration;
                        if (slot.duration == 0) {
                            if (def -> callbacks.on_remove)
                            def -> callbacks.on_remove(entity, slot, *this);
                        should_remove = true;
                        }
                        
                    }
                    if (should_remove) {
                        continue; /* Drop Effect */
                    }

                /** keep slot */
                if (write != i){
                    pool.slots[write] = slot;
                }
                write++;
            }

            pool.count = write;
        }
            

        RegResult<void> remove(core_entity* entity, core_zeal_id id) noexcept 
            pre(entity != nullptr)
            pre(id != CORE_ZEAL_NONE)
        {
            core_zeal_pool& pool = entity -> zeal_pool;
            core_u32 write = 0;
            core_bool found = false;

            for (core_u32 i = 0; i < pool.count; ++i) {
                if (pool.slots[i].id == id) {
                    auto def_result = get(id);
                    if (def_result && (*def_result) -> callbacks.on_remove) {
                        (*def_result) -> callbacks.on_remove(entity, pool.slots[i], *this);
                    }
                    found = true;
                    continue;
                }
                if (write != i) pool.slots[write] = pool.slots[i];
                ++write;
            }

            pool.count = write;
            return found
                ? RegResult<void>{} : 
                std::unexpected(RegistryError::NotFound);
        }

        RegResult<void> steal(core_entity* from, core_entity* to, core_zeal_id id) noexcept
        pre(from != nullptr)
        pre(to != nullptr)
        pre(from != to)
        pre(id != CORE_ZEAL_NONE)
    {
        auto def_result = get(id);
        if (!def_result) return std::unexpected(def_result.error());
        if (!(*def_result) -> can_be_stolen)
            return std::unexpected(RegistryError::InvalidData);

        core_zeal_pool& pool = from->zeal_pool;

        for (core_u32 i = 0; i < pool.count; ++i){
            if (pool.slots[i].id != id) continue;

            auto stolen = pool.slots[i];

            /* remove from source */
            for (core_u32 j = i; j < pool.count - 1; ++j)
                pool.slots[j] = pool.slots[j + 1];
            --pool.count;

            /* give to target */
            return apply(to, stolen.id, stolen.source, stolen.stacks, stolen.duration);

        }
        return std::unexpected(RegistryError::NotFound);
    }

       [[nodiscard]]
       std::generator<const ZealDefinition&> all() const {
            for (const auto& [id, def] : m_table) co_yield def;
       }

       [[nodiscard]]
       std::generator<const ZealDefinition&> cross_entity() const {
            for (const auto& [id, def] : m_table)
                if(def.is_cross_entity) co_yield def;
       }

       [[nodiscard]]
       std::generator<const ZealDefinition&> by_scope(ZealScope s)const {
            for (const auto& [id, def] : m_table)
                if(def.scope == s) co_yield def;
       }

       [[nodiscard]]
       std::size_t count() const noexcept {
            return m_table.size();
       }

    private:
       std::flat_map<core_u32, ZealDefinition> m_table;
       std::flat_map<core_view_only_str, core_u32> m_name_index;
};
