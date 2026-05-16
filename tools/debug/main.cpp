#include <print>
#include "RegistryContext.hpp"
#include "Registries/Stats/StatRegistry.hpp"
#include <iostream>
#include <contracts>

void handle_contract_violation(
    const std::contracts::contract_violation& v) noexcept
{
    std::cerr << "CONTRACT VIOLATED: " << v.comment()
              << " at " << v.location().file_name()
              << ":" << v.location().line() << "\n";
    std::terminate();
}

int main() {
    std::println("=== Combat System Test ===");

    RegistryContext RC;

    // ====================== Test StatRegistry ======================
    std::println("\n--- Testing StatRegistry ---");

    StatDefinition hp_def{
        .id            = 1,
        .name          = "hp",
        .display_name  = "Hit Points",
        .default_value = 100,
        .min_value     = 0,
        .max_value     = 9999,
        .category      = StatCategory::Base,
        .is_base       = true,
        .can_corrupt   = true,
        .can_surge     = true
    };

    auto result = RC.stats().register_stat(hp_def);

    if (result) {
        std::println("✓ Successfully registered HP");
    } else {
        std::println("✗ Register failed: {}", (int)result.error());
    }

    auto found = RC.stats().get(1);
    if (found) {
        std::println("✓ Retrieved: {} (Base: {})", 
                     (*found)->display_name, (*found)->is_base);
    }

    RC.print_summary();

    std::println("\n=== Test Complete ===");
    return 0;
}