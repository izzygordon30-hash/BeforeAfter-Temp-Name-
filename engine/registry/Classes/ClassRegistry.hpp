#pragma once

/** ClassRegistry.hpp 
 *
 * Owns all Class definitions. 
 * C stores class IDS in the core_class_pool
 * C++ defines them futher to what they mean here(What they are and do)
 * 
 * Classes Defines
 * - What the class specialize in (stat affinities)
 * - Rival class Relationship
 * - Bond tier scalinf (partial/ full member bonuses)
 * - Class-specific mechanics (AREA_51 classified, SEA_BUD tide, INTROVERTS longevity etc)
 * 
 * Bond tier system
 * All members present -> full_bonus_id applied
 * Some members present -> partial_bonus_id scaled by ratio
 * Below theshold -> no bonus
 * Member dies -> Bond Recalculates Automatically
*/

/** 
 * Note: Some Ppl are Pairs which that it self is a team mabye a trio or a duo. 
 * some slightly like they're aquantancies or just slight 
 * upgrades as they just worked together some mabye gained after a pairing without one before 
 * Hopefully we can stich this up with existing things here and mabye if need at at least 2 new things.
*/ 

#include "RegistryContext.hpp"
#include "ContractGuards.hpp"
#include <flat_map>
#include <generator>
#include <string_view>
#include <functional>
#include <inplace_vector>

// prevents name managling
extern "C" {
    #include "Core.h"
}

/** Forward Declaration */
class ClassRegistry;

/**
 * ### Relationship Tier
 * How strong the bond is between entities of this class.
 * Rust tracks progression. C++ defines the Tier Name
 */
enum class RelationshipTier : core_u8 {
    /**
     * ### None
     * No Relationship
     */
    None,
    /**
     * ### Allied
     * Acquaintances - slight bonus when together
     */
    Allied,
    /**
     * ### Paired
     * Dedicated Duo/Trio - Full team
     */
    Paired,
    /**
     * ### Organic
     * earned by working together - Rust promotes
     */
    Organic,
    /**
     * ### Rival
     * class-wide rivalry - bonus vs specific class
     */
    Rival,
};

/**
 * ### Bond Tier Result
 * Return by evaluate_bond_tier().
 */
struct BondTierResult {
    /**
     * ### Presence Ratio
     * `0.0 - 1.0`
     */
    core_f32 presence_ratio;
    /**
     * ### Bonus Id
     * Zeal or effect ID to apply
     */
    core_u32 bonus_id;
     /**
     * ### Tier
     * What Tier fired this
     */
    RelationshipTier tier;
    /**
     * ### Is Full?
     * All Members Present?
     */
    core_bool is_full;
    /**
     * ### Is Partial?
     * Only Some?
     */
    core_bool is_partial;
    /**
     * ### No Bonus?
     * Below Member Threshold?
     */
    core_bool no_bonus;
};

/**
 * ## Class Mechanic
 * Class-Specfic behavior slot
 */
struct ClassMechanic {
    /**
     * ### Id
     * Unique Identifier for ClassesMechanics
     */
    core_u32 id;
    /**
     * ### Name of Mechanic?
     * Mechanic Name
     */
    core_view_only_str name;
    /**
     * ### Execute Call for C++
     * Just a Helpful Tool
     */
    std::function<void(core_entity*, ClassRegistry&)> execute;
};

/**
 * ## StatAffinity
 * What stats does this class naturally leans on
 */
struct StatAffinity{
    /**
     * ### Stat Id
     * Which Stat?
     */
    core_u32 stat_id;
    /**
     * ### Multiplier(float)
     * It mutiplies stats amounts
     */
    core_f32 multiplier;
};

/**
 * ## Relationship Definition
 * Describes a relationship a class can have.
 */
struct RelationshipDefinition {
    RelationshipTier tier;
    /**
     * ### Partner Class Id
     * Who is this with
     */
    core_class_id  partner_class_id;
    /**
     * ### Partial Bonus Id
     * Bonus with partial presence
     */
    core_u32       partial_bonus_id;
    /**
     * ### Full Bonus Id
     * Bonus at Full Presence
     */
    core_u32       full_bonus_id;
    /**
     * ### Organic Bonus Id
     * Bonus when gained organically (through pairing)
     */
    core_u32       organic_bonus_id;
    /**
     * ### Earned Threshold
     * Rust Tracks - Times worked together to earn organic bonds
     */
    core_u32       earned_threshold;
    /**
     * ### Partial Threshold
     * Minimum Ratio for partial.
     */
    core_f32        partial_threshold;
};

/**
 * ## Class Definition
 * Defines a Classes. What it means and its attributes.
 */
struct ClassDefinition {
    core_effect_id id;
    core_view_only_str name;
    core_view_only_str display_name;
    core_u32 description_id; /** String Table Ref */

    core_class_id rival_class_id;

    core_class_id bond_trigger_class;

    core_f32        partial_threshold;
    core_u32       partial_bonus_id;
    core_u32       full_bonus_id;

    std::inplace_vector<RelationshipDefinition, CORE_BOND_PARTNER_MAX> relationships;
    std::inplace_vector<StatAffinity, 4> stat_definities;
    std::inplace_vector<ClassMechanic, 4> mechanics;
};

/**
 * ## ClassRegistry
 * Define a Class and its Processes
 */
class ClassRegistry {
    public:
        ClassRegistry() = default;
        ~ClassRegistry() = default;

/**
     * @brief Register a new effect definition.   
     * @return AlreadyRegistered if id exists. 
     * 
     */
    RegResult<void> register_effect(ClassDefinition def)
        pre(def.id != CORE_CLASS_NONE)
    {
        if (m_table.contains(def.id))
            return std::unexpected(RegistryError::AlreadyRegistered);

        m_name_index.emplace(def.name, def.id);
        m_table.emplace(def.id, std::move(def));
        return{};
    }

    [[nodiscard]]
       RegResult<const ClassDefinition&> get(core_class_id id) const noexcept
       pre(id != CORE_CLASS_NONE){
            auto it = m_table.find(id);
            if(it == m_table.end())
                return std::unexpected(RegistryError::NotFound);
            return it->second;
       }

    [[nodiscard]]
       RegResult<core_class_id> get_id(core_view_only_str name) const noexcept {
            auto it = m_name_index.find(name);
            if(it == m_name_index.end())
                return std::unexpected(RegistryError::NotFound);
            return it->second;
    }

    [[nodiscard]]
       RegResult<core_class_id> get_rival(core_class_id id) const noexcept {
            auto def = get(id);
            if (!def) return std::unexpected(def.error());
            if (def -> rival_class_id == CORE_CLASS_NONE)
                return std::unexpected(RegistryError::NotFound);
            return def -> rival_class_id;
       }
    
    [[nodiscard]]
       core_bool are_rivals(core_class_id a, core_class_id b) const noexcept {
           auto rival = get_rival(a);
           return rival.has_value() && *rival == b;
       }
    
    [[nodiscard]]
       const RelationshipDefinition* find_relationship(core_class_id from, core_class_id to) const noexcept
    {
        auto def = get(from);
        if (!def) return nullptr;

        for (const auto& rel : def -> relationships)
            if (rel.partner_class_id == to) return &rel;
        return nullptr;
    }

    [[nodiscard]]
        RelationshipTier get_tier(core_class_id a, core_class_id b) const noexcept{
            auto rel = find_relationship(a, b);
            if (!rel) return RelationshipTier::None;
            return rel->tier;
        }

    [[nodiscard]]
        BondTierResult evaluate_bond_tier(core_class_id class_id, core_class_id partner_class_id, 
            core_u32 members_present, core_u32 total_members, RelationshipTier current_tier) const noexcept
                pre(total_members > 0)
            {
                const RelationshipDefinition* rel = 
                    find_relationship(class_id, partner_class_id);
                
                core_f32 threshold = rel ? rel -> partial_threshold : 0.5f;
                core_u32 full_id   = rel ? rel -> full_bonus_id     : 0;
                core_u32 part_id   = rel ? rel -> partial_bonus_id  : 0;

                if (current_tier == RelationshipTier::Organic && rel)
                    full_id = rel -> organic_bonus_id;
                
                core_f32 ratio = static_cast<float>(members_present) / static_cast<float>(total_members);

                if (ratio >= 1.0f)
                    return {ratio, full_id, current_tier, false, true, false};

                if (ratio >= threshold)
                    return {ratio, part_id, current_tier, true, false, false};
                
                return {ratio, 0, RelationshipTier::None, false, false, true};
            }
        
        [[nodiscard]]
            BondTierResult fallen_member(
                core_class_id class_id, core_class_id partner_class_id,
                core_u32 surviving_count, core_u32 total_members,
                RelationshipTier current_tier) const noexcept {
                    return evaluate_bond_tier(class_id, partner_class_id, surviving_count, total_members, current_tier);
            }

        
        RegResult<void> execute_mechanic(core_entity* entity, core_u32 mechanic_id) noexcept
            pre(entity != nullptr)
        {
            auto def = get(entity -> class_type);
            if (!def) return std::unexpected(def.error());

            for (const auto& mech : def -> mechanics) {
                if (mech.id != mechanic_id) continue;
                if (mech.execute) mech.execute(entity, *this);
                return {};
            }
            return std::unexpected(RegistryError::NotFound);
        }

        void apply_affinities(core_entity* entity) noexcept
            pre(entity != nullptr)
        {
            auto def = get(entity -> class_type);
            if (!def) return;

            for (const auto& aff : def -> stat_affinities) {
                switch (aff.stat_id) {
                    case 3: 
                        entity -> stats.strength = static_cast<core_i32>(
                        entity -> stats.strength * aff.multiplier); break;

                    case 4: 
                        entity -> stats.defense = static_cast<core_i32>(
                        entity -> stats.defense * aff.multiplier); break;

                    case 5: 
                        entity -> stats.dodge = static_cast<core_i32>(
                        entity -> stats.dodge * aff.multiplier); break;

                    case 6: 
                        entity -> stats.tenacity = static_cast<core_i32>(
                        entity -> stats.tenacity * aff.multiplier); break;
                    default: 
                        break;
                }
            }
        }

       [[nodiscard]]
       std::generator<const ClassDefinition&> all() const {
            for (const auto& [id, def] : m_table) co_yield def;
       }

       [[nodiscard]]
       std::generator<const ClassDefinition&> with_rivals() const {
            for (const auto& [id, def] : m_table)
                if(def.rival_class_id != CORE_CLASS_NONE) 
                co_yield def;
       }

       [[nodiscard]]
       std::generator<const ClassDefinition&> with_relationships() const {
            for (const auto& [id, def] : m_table)
                if(def.relationships.empty()) co_yield def;
       }

       [[nodiscard]]
       std::size_t count() const noexcept {
            return m_table.size();
       }


    private:
       std::flat_map<core_class_id, ClassDefinition> m_table;
       std::flat_map<core_view_only_str, core_class_id> m_name_index;
};

