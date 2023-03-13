#include "GenePool.hpp"

#include <algorithm> // for std::sort
#include <cstdint>   // for std::uint8_t
#include <iostream>
#include <random>  // for std::discrete_distribution
#include <utility> // for std::move

#include "ChessGame.hpp"
#include "ChessPiece.hpp"
#include "Utilities.hpp"

Organism::Organism() noexcept
    : num_wins(0)
    , num_draws(0)
    , num_losses(0)
    , genome({}){};

Organism::Organism(std::vector<PreferenceToken> _genome) noexcept
    : num_wins(0)
    , num_draws(0)
    , num_losses(0)
    , genome(std::move(_genome)){};

void Organism::versus(Organism &enemy) {
    Organism &self = *this;
    Engine::PreferenceChain white_engine(self.genome);
    Engine::PreferenceChain black_engine(enemy.genome);
    ChessGame game;

    switch (game.run(&white_engine, &black_engine, false)) {
        case PieceColor::NONE:
            ++self.num_draws;
            ++enemy.num_draws;
            break;
        case PieceColor::WHITE:
            ++self.num_wins;
            ++enemy.num_losses;
            break;
        case PieceColor::BLACK:
            ++self.num_losses;
            ++enemy.num_wins;
            break;
    }
}

GenePool::GenePool() noexcept
    : rng(properly_seeded_random_engine())
    , organisms() {}


void GenePool::add_organism(std::vector<PreferenceToken> genome) noexcept {
    organisms.emplace_back(std::move(genome));
}


PreferenceToken
GenePool::find_new_gene(const std::vector<PreferenceToken> &genome) noexcept {
    while (true) {
        const PreferenceToken token = random_choice(rng, PREFERENCE_POOL);
        if (!contains(genome, token)) { return token; }
    }
}


enum class MutationToken : std::uint8_t {
    INSERT, // insert distinct random preference
    DELETE, // delete random preference
    SWAP,   // swap two random preferences
    REPLACE // replace random preference with a new distinct random preference
};


void GenePool::mutate(std::vector<PreferenceToken> &genome) noexcept {
    // Create distribution of possible mutations
    std::discrete_distribution<MutationToken> mutation_dist(
        {static_cast<double>(genome.size() < PREFERENCE_POOL.size()),
         static_cast<double>(!genome.empty()),
         static_cast<double>(genome.size() > 1),
         static_cast<double>(
             (!genome.empty()) && (genome.size() < PREFERENCE_POOL.size())
         )}
    );

    // Choose and execute a random mutation
    const MutationToken token = mutation_dist(rng);
    switch (token) {
        case MutationToken::INSERT:
            random_insert(rng, genome, find_new_gene(genome));
            break;
        case MutationToken::DELETE: random_delete(rng, genome); break;
        case MutationToken::SWAP: random_swap(rng, genome); break;
        case MutationToken::REPLACE:
            random_replace(rng, genome, find_new_gene(genome));
            break;
    }
}


void GenePool::evaluate_fitness(std::size_t num_rounds) noexcept {
    for (std::size_t i = 0; i < num_rounds; ++i) {
        for (auto it1 = organisms.begin(); it1 != organisms.end(); ++it1) {
            for (auto it2 = it1 + 1; it2 != organisms.end(); ++it2) {
                it1->versus(*it2);
                it2->versus(*it1);
            }
        }
    }
}


void GenePool::sort_by_fitness() noexcept {
    std::sort(
        organisms.begin(),
        organisms.end(),
        [&](const auto &a, const auto &b) {
            const double a_ratio = static_cast<double>(a.num_wins) /
                                   static_cast<double>(a.num_losses);
            const double b_ratio = static_cast<double>(b.num_wins) /
                                   static_cast<double>(b.num_losses);
            if (a_ratio == b_ratio) {
                if (a.num_wins == b.num_wins) {
                    return a.num_draws < b.num_draws;
                }
                return a.num_wins > b.num_wins;
            }
            return a_ratio > b_ratio;
        }
    );
}


void GenePool::cull(std::size_t num_deaths) noexcept {
    sort_by_fitness();
    organisms.erase(
        organisms.end() -
            static_cast<std::vector<Organism>::iterator::difference_type>(
                num_deaths
            ),
        organisms.end()
    );
}


void GenePool::breed(std::size_t num_children_per_organism) noexcept {
    const std::size_t original_size = organisms.size();
    for (std::size_t i = 0; i < original_size; ++i) {
        const Organism &parent = organisms[i];
        for (std::size_t j = 0; j < num_children_per_organism; ++j) {
            organisms.emplace_back(parent.genome);
            mutate(organisms.rbegin()->genome);
        }
    }
}
