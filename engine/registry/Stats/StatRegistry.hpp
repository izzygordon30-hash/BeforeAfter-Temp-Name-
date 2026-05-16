#pragma once


/** StatRegistry.hpp 
 *
 * Stats are Registered Here. 
 * C defined what data they have and memory
 * C++ defines them futher to what they mean here
*/

#include "RegistryContext.hpp"
#include "ContractGuards.hpp"
#include <flat_map>
#include <generator>
#include <string_view>
#include <algorithm>

/**
 * ## Stat Category
 * Defines what kind of stat the Statistic defined is.
 */
enum StatCategory : core_u8 {
    /**
     * ### Base
     * This Stat is a base stat it loaded always. (hp, max_hp, strength, defense..etc)
     */
    Base, 
    /**
     * ### Combat
     * This Stat is a Combat stat. (crit, accuracy - via additions_id)
     */
    Combat,
    /**
     * ### Social (Bond Between Characters)
     * This Stat is a Bond stat only achieved as a result of a bond. (crit, accuracy - via additions_id)
     */
    Social,
    /**
     * ### Hidden
     * Internal (Not shown to players)
     */
    Hidden,
};
 
/**
 * ## Stat Definition
 * Defines a Stat. What it means and its attributes.
 */

struct StatDefinition {
    /**
     * ## Stat ID
     * Unique Indetifier for a Statisic 
     */
    core_u32                id; 
    /**
     * ## Stat Name
     * Name of the Statisic  (Internal)
     */
    core_view_only_str      name;
    /**
     * ## Stat Display Name
     * Name of the Statisic  (Shown;External)
     */
    core_view_only_str      display_name;
     /**
     * ## Stat Default Value
     * Default Value of the Statisic 
     */
    core_i32                default_value;
     /**
     * ## Stat Min Value
     * Minimum Value of the Statisic 
     */
    core_i32                min_value;

     /**
     * ## Stat Max Value
     * Maximum Value of the Statisic 
     */
    core_i32                max_value;

    /**
     * ## Stat Category
     * Type of Statisic (Base, Combat, Social, Hidden)
     */
    StatCategory            category;

    /**
     * ## Stat Base Check
     * Is this a Base Statisic
     */
    core_bool               is_base;
    /**
     * ## Stat Corruption Check
     * Can this Statisic be Corrupted 
     */
    core_bool               can_corrupt;
    /**
     * ## Stat Surge Check
     * Can this Statisic be Surged
     */
    core_bool               can_surge;
};

/**
 * ## StatisticRegistry
 * Define a Stat and its Processes
 */
class StatRegistry {
    public:
        StatRegistry() = default;
        ~StatRegistry() = default;

        /** Register A New Stat Here Buddy 
         * Fails If Id already Exists or min > max
        */
       RegResult<void> register_stat(StatDefinition def)
       pre(def.id != 0)
       pre(def.min_value <= def.max_value) {
            if(m_table.contains(def.id))
                return std::unexpected(RegistryError::AlreadyRegistered);

            m_name_index.emplace(def.name, def.id);
            m_table.emplace(def.id, std::move(def));
            return{};
       }

       /* Lookup */

       [[nodiscard]]
       RegResult<const StatDefinition*> get(core_u32 stat_id) const noexcept
       pre(stat_id != 0){
            auto it = m_table.find(stat_id);
            if(it == m_table.end())
                return std::unexpected(RegistryError::NotFound);
            return &it->second;
       }

       [[nodiscard]]
       RegResult<core_u32> get_id(core_view_only_str name) const noexcept {
            auto it = m_name_index.find(name);
            if(it == m_name_index.end())
                return std::unexpected(RegistryError::NotFound);
            return it->second;
       }

       [[nodiscard]]
       core_bool is_base(core_u32 stat_id) const noexcept{
            auto r = get(stat_id);
            return r.has_value() && (*r)->is_base;
       }

       [[nodiscard]]
       core_bool can_corrupt(core_u32 stat_id) const noexcept{
            auto r = get(stat_id);
            return r.has_value() && (*r)->can_corrupt;
       }

       [[nodiscard]]
       core_bool can_surge(core_u32 stat_id) const noexcept{
            auto r = get(stat_id);
            return r.has_value() && (*r)->can_surge;
       }

       [[nodiscard]]
       std::generator<const StatDefinition&> all() const {
            for (const auto& [id, def] : m_table)
                co_yield def;
       }

       [[nodiscard]]
       std::generator<const StatDefinition&> by_category(StatCategory cat) const {
            for (const auto& [id, def] : m_table)
                if(def.category == cat) co_yield def;
       }

       [[nodiscard]]
       std::size_t count() const noexcept {
            return m_table.size();
       }
    private:
       /**
        * Flat Map - sorted contiguous storage and cache friendly
        */
       std::flat_map<core_u32, StatDefinition> m_table;
       std::flat_map<core_view_only_str, core_u32> m_name_index;
};