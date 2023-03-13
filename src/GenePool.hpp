#ifndef SUCKER_CHESS_GENE_POOL_HPP
#define SUCKER_CHESS_GENE_POOL_HPP

#include <array>   // for std::array
#include <cstddef> // for std::size_t
#include <random>  // for std::mt19937
#include <vector>  // for std::vector

#include "Engine/PreferenceChain.hpp"


class Organism {
public:

    std::size_t num_wins;
    std::size_t num_draws;
    std::size_t num_losses;
    std::vector<PreferenceToken> genome;

    explicit Organism() noexcept;
    explicit Organism(std::vector<PreferenceToken>) noexcept;

    const std::vector<PreferenceToken> &get_genome() const;
    void versus(Organism &enemy);

}; // class Organism


class GenePool final {
private:

    using enum PreferenceToken;

    static constexpr std::array<PreferenceToken, 26> PREFERENCE_POOL = {
        MATE_IN_ONE,   PREVENT_MATE_IN_ONE,
        PREVENT_DRAW,  CHECK,
        CAPTURE,       CAPTURE_HANGING,
        SMART_CAPTURE, CASTLE,
        FIRST,         LAST,
        EXTEND,        REDUCE,
        GREEDY,        GENEROUS,
        SWARM,         HUDDLE,
        SNIPER,        SLOTH,
        CONQUEROR,     CONSTRICTOR,
        REINFORCED,    OUTPOST,
        GAMBIT,        EXPLORE,
        COWARD,        HERO,
    };

    std::mt19937 rng;
    std::vector<Organism> organisms;

public: // ========================================================= CONSTRUCTOR

    explicit GenePool() noexcept;

public: // =========================================================== ACCESSORS

    [[nodiscard]] constexpr const std::vector<Organism> &
    get_organisms() const noexcept {
        return organisms;
    }

public: // ============================================================ MUTATORS

    void add_organism(std::vector<PreferenceToken> genome) noexcept;

private: // =================================================== MUTATION HELPERS

    /**
     * @brief Returns a PreferenceToken that isn't in the specified genome
     *
     * @param genome
     * @return PreferenceToken
     */
    PreferenceToken find_new_gene(const std::vector<PreferenceToken> &genome
    ) noexcept;

    /**
     * @brief Mutates a genome by either inserting, deleting, swapping, or
     * replacing a PreferenceToken
     *
     * @param genome
     */
    void mutate(std::vector<PreferenceToken> &genome) noexcept;

public: // =========================================================== EVOLUTION

    /**
     * @brief Runs tournament where each round, every organism plays every other
     * organism once as white and once as black
     *
     * @param num_rounds
     */
    void evaluate_fitness(std::size_t num_rounds) noexcept;

    /**
     * @brief Sorts the organisms vector by win-loss-ratio
     *
     */
    void sort_by_fitness() noexcept;

    /**
     * @brief Removes the last num_deaths organisms
     *
     * @param num_deaths
     */
    void cull(std::size_t num_deaths) noexcept;

    /**
     * @brief Creates new mutated offspring
     *
     * @param num_children_per_organism
     */
    void breed(std::size_t num_children_per_organism) noexcept;

}; // class GenePool


#endif // SUCKER_CHESS_GENE_POOL_HPP
