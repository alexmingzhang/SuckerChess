#ifndef SUCKER_CHESS_GENE_POOL_HPP
#define SUCKER_CHESS_GENE_POOL_HPP

#include <array>   // for std::array
#include <cstddef> // for std::size_t
#include <random>  // for std::mt19937
#include <vector>  // for std::vector

#include "Engine/PreferenceChain.hpp"


struct Organism {

    std::vector<PreferenceToken> genome;
    unsigned long long num_wins;
    unsigned long long num_draws;
    unsigned long long num_losses;

}; // struct Organism


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

    PreferenceToken find_new_gene(const std::vector<PreferenceToken> &genome
    ) noexcept;

    std::vector<PreferenceToken>
    mutate(const std::vector<PreferenceToken> &genome) noexcept;

public: // =========================================================== EVOLUTION

    void evaluate_fitness(unsigned long long num_rounds) noexcept;

    void sort_by_fitness() noexcept;

    void cull(std::size_t num_deaths) noexcept;

    void breed(unsigned long long num_children_per_organism) noexcept;

}; // class GenePool


#endif // SUCKER_CHESS_GENE_POOL_HPP
