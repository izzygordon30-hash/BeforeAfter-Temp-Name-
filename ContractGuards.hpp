#ifndef __INTELLISENSE__
// Contracts are active — GCC 16 enforces pre() and post() at runtime.
#else

/**
 * @def pre(condition)
 * @brief Contract precondition guard — disabled for IntelliSense.
 *
 * @details
 * C++26 contracts (P2900) introduce @c pre() and @c post() as language-level
 * correctness assertions. GCC 16 enforces these at runtime via @c -fcontracts.
 *
 * IntelliSense does not yet parse contract specifiers, causing false
 * diagnostics. This macro suppresses them without affecting compilation.
 *
 * @note Remove this guard once the VS Code C/C++ extension gains C++26
 *       contracts support.
 *
 * @param condition The boolean expression that must hold at function entry.
 *
 * @par Example
 * @code
 * RegResult<void> register_stat(StatDefinition def)
 *     pre(def.id != 0)
 *     pre(def.min_value <= def.max_value)
 * { ... }
 * @endcode
 */
#define pre(...)

/**
 * @def post(condition)
 * @brief Contract postcondition guard — disabled for IntelliSense.
 *
 * @details
 * Postconditions assert correctness of a function's return value or
 * side effects. Enforced by GCC 16 at runtime under @c -fcontracts.
 * Suppressed here for IntelliSense compatibility only.
 *
 * @param condition The boolean expression that must hold at function exit.
 */
#define post(...)

#endif


#ifdef __INTELLISENSE__
namespace std::contracts {
    struct _CVLoc {
        const char* file_name()     const noexcept { return ""; }
        int         line()          const noexcept { return 0;  }
        const char* function_name() const noexcept { return ""; }
    };
    struct contract_violation {
        const char* comment()  const noexcept { return "";      }
        _CVLoc      location() const noexcept { return {};      }
    };
}
#endif