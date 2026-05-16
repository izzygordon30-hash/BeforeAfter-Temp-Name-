/**
 * ## Addition Machine
 * A class handling the addition of entries to a registry and resolveing them through ids.
*/
template<typename TDefinition>
class AdditionMachine {
    public:
    /**
     *  ## Resgister Entry
     *  @brief Registers an entry in the resgistry with the given ID and definition.
     *  @param id The ID of the entry to register.
     *  @param def The definition of the entry to register.
     */
    RegResult<void> register_entry(core_ext_id id, const TDefinition& def);

    /**
     * ## Resolve ID
     * @brief Resolves an ID to its corresponding entry in the registry.
     * @param id The ID to resolve.
     */
    RegResult<const TDefinition&> resolve(core_ext_id id) const noexcept;

    /**
     * ## Knows ID?
     * @brief Check if an ID has been registered before.
     * @param id The ID to check;
     */
    bool knows(core_ext_id id) const noexcept;

    /**
     * ## All
     * @brief Returns a generator of all the known registered IDs in the registry.
     */
    std::generator<core_ext_id> all() const noexcept;

private:
    /**
     * ## Table
     * @brief The underlying data structure that holds the registered entries, mapping IDs to their defined data.
     */
    std::flat_map<core_ext_id, TDefinition> m_table;
};