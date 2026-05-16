#pragma once

/** RegistryContext.hpp 
 *
 * All Files Registries are stored and loaded in this file.
*/
#include <memory>
#include <expected>
#include <cstdint>
#include "CplusplusCore/CorePlus.hpp"

/**
 * ## Registry Error
 * All operations return RegResult<T> - never throws exceptions.
 */
enum class RegistryError : core_u8 {
    /**
     * ### None
     * No Error, You good boi
     */
    None,
     /**
     * ### NotFound
     * The Registry was not found bruh.
     */
    NotFound, 
    /**
     * ### AlreadyRegistered
     * You Registered this already, Stop being Dumb.
    */
    AlreadyRegistered, 
    /**
    * ### InvalidData
    * Invalid Data, Check your inputs broski.
    */
    InvalidData, 
    /**
    * ### CapacityExceeded
    * My Boy the Registry is Full, No more space for new entries.
    */
    CapacityExceeded 
};

template<typename T>
/**
 * ## RegResult
 * Returns Type T if the operation was successful otherwise it returns a RegistryError.
 */
using RegResult = std::expected<T, RegistryError>;

class StatRegistry;
class ItemRegistry;
class EffectRegistry;
class ZealRegistry;
class ClassRegistry;
class SpeciesRegistry;
class AbilityRegistry;
class BondRegistry;
class SummonRegistry;



/**
 * ## RegistryContext
 * The RegistryContext is a singleton class that holds and loads all the registries in the game.
 */
class RegistryContext{
    public:
        RegistryContext();
        ~RegistryContext();

        RegistryContext(const RegistryContext&) = delete;
        RegistryContext& operator=(const RegistryContext&) = delete;
        RegistryContext(RegistryContext&&) = default;
        RegistryContext& operator=(RegistryContext&&) = default;

        [[nodiscard]] StatRegistry& stats() noexcept;
        [[nodiscard]] StatRegistry& stats() const noexcept;

        void print_summary() const noexcept;

    private:
        std::unique_ptr<StatRegistry>    m_stats;
        std::unique_ptr<EffectRegistry>  m_effects;
        std::unique_ptr<ZealRegistry>    m_zeals;
        std::unique_ptr<ClassRegistry>   m_classes;
        std::unique_ptr<SpeciesRegistry> m_species;
        std::unique_ptr<AbilityRegistry> m_abilities;
        std::unique_ptr<BondRegistry>    m_bonds;
        std::unique_ptr<SummonRegistry>  m_summons;
        std::unique_ptr<ItemRegistry>    m_items; // ← last since added later
};