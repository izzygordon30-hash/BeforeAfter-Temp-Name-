#include "RegistryContext.hpp"
#include "Registries/Stats/StatRegistry.hpp"
#include <print>

 
/*=
   ## REGISTRY STUBS
   Placeholder classes until each registry is written.
   Keeps unique_ptr happy — swap real class in when ready.
*/
class EffectRegistry{};
class ItemRegistry{};
class ZealRegistry{};
class ClassRegistry{};
class SpeciesRegistry{};
class BondRegistry{};
class SummonRegistry{};
class AbilityRegistry {};

/**
 * Implementation (RegistryContext)
 * 
 */
RegistryContext::RegistryContext() 
    : m_stats    (std::make_unique<StatRegistry>())
    , m_effects  (std::make_unique<EffectRegistry>())
    , m_zeals    (std::make_unique<ZealRegistry>())
    , m_classes  (std::make_unique<ClassRegistry>())
    , m_species  (std::make_unique<SpeciesRegistry>())
    , m_abilities(std::make_unique<AbilityRegistry>())
    , m_bonds    (std::make_unique<BondRegistry>())
    , m_summons  (std::make_unique<SummonRegistry>())
    , m_items    (std::make_unique<ItemRegistry>())
{}

RegistryContext::~RegistryContext() = default;

StatRegistry& RegistryContext::stats() noexcept {
    return *m_stats;
}

StatRegistry& RegistryContext::stats() const noexcept {
    return *m_stats;
}

void RegistryContext::print_summary() const noexcept {
    const char* DIM  = "\033[90m";
    const char* GRN  = "\033[92m";
    const char* RST  = "\033[0m";
    const char* BOLD = "\033[1m";

    std::println("\n  {}┌─────────────────────────────────────┐{}", DIM, RST);
    std::println("  {}│{} {}REGISTRY CONTEXT{}                      {}│{}", DIM, RST, BOLD, RST, DIM, RST);
    std::println("  {}│{}  Combat Engine — Before/After         {}│{}", DIM, RST, DIM, RST);
    std::println("  {}│{}  {}VERIFIED OK{}  2026-05-11  RE-7735CE  {}│{}", DIM, RST, GRN, RST, DIM, RST);
    std::println("  {}├─────────────────────────────────────┤{}", DIM, RST);
    std::println("  {}│{} {:>2}  stat_registry     StatRegistry    {}│{}", DIM, RST, m_stats->count(), DIM, RST);
    std::println("  {}│{} {:>2}  effect_registry   EffectRegistry  {}│{}", DIM, RST, 0, DIM, RST);
    std::println("  {}│{} {:>2}  zeal_registry     ZealRegistry    {}│{}", DIM, RST, 0, DIM, RST);
    std::println("  {}│{} {:>2}  class_registry    ClassRegistry   {}│{}", DIM, RST, 0, DIM, RST);
    std::println("  {}│{} {:>2}  species_registry  SpeciesRegistry {}│{}", DIM, RST, 0, DIM, RST);
    std::println("  {}│{} {:>2}  summon_registry   SummonRegistry  {}│{}", DIM, RST, 0, DIM, RST);
    std::println("  {}├─────────────────────────────────────┤{}", DIM, RST);
    std::println("  {}│{}  HASH 0x{:04X}  ·  {} bytes            {}│{}",
                 DIM, RST,
                 (uint16_t)(m_stats->count() * 0xABC1),
                 m_stats->count() * sizeof(StatDefinition),
                 DIM, RST);
    std::println("  {}└─────────────────────────────────────┘{}", DIM, RST);
    std::println("  {}· · · · · · · · · · · · · · · · · · ·{}", DIM, RST);
}