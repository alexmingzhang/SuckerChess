#include "GenePool.hpp"

#include <algorithm> // for std::sort
#include <cstdint>   // for std::uint8_t
#include <random>    // for std::discrete_distribution
#include <utility>   // for std::move

#include "ChessGame.hpp"
#include "ChessPiece.hpp"
#include "Utilities.hpp"


GenePool::GenePool() noexcept
    : rng(properly_seeded_random_engine())
    , organisms() {}


void GenePool::add_organism(std::vector<PreferenceToken> genome) noexcept {
    organisms.push_back({std::move(genome), 0, 0, 0});
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


std::vector<PreferenceToken>
GenePool::mutate(const std::vector<PreferenceToken> &genome) noexcept {

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
            return random_insert(rng, genome, find_new_gene(genome));
        case MutationToken::DELETE: return random_delete(rng, genome);
        case MutationToken::SWAP: return random_swap(rng, genome);
        case MutationToken::REPLACE:
            return random_replace(rng, genome, find_new_gene(genome));
    }
}


void GenePool::evaluate_fitness(unsigned long long num_rounds) noexcept {
    for (unsigned long long i = 0; i < num_rounds; ++i) {
        for (Organism &white : organisms) {
            for (Organism &black : organisms) {
                if (&white != &black) {
                    Engine::PreferenceChain white_engine(white.genome);
                    Engine::PreferenceChain black_engine(black.genome);
                    ChessGame game;
                    const PieceColor winner =
                        game.run(&white_engine, &black_engine, false);
                    switch (winner) {
                        case PieceColor::NONE:
                            ++white.num_draws;
                            ++black.num_draws;
                            break;
                        case PieceColor::WHITE:
                            ++white.num_wins;
                            ++black.num_losses;
                            break;
                        case PieceColor::BLACK:
                            ++white.num_losses;
                            ++black.num_wins;
                            break;
                    }
                }
            }
        }
    }
}


void GenePool::sort_by_fitness() noexcept {
    std::sort(
        organisms.begin(),
        organisms.end(),
        [&](const auto &a, const auto &b) {
            const unsigned long long a_wins = a.num_wins;
            const unsigned long long a_losses = a.num_losses;
            const unsigned long long b_wins = b.num_wins;
            const unsigned long long b_losses = b.num_losses;

            const double a_ratio =
                static_cast<double>(a_wins) / static_cast<double>(a_losses);
            const double b_ratio =
                static_cast<double>(b_wins) / static_cast<double>(b_losses);

            if (a_ratio == b_ratio) {
                if (a_wins == b_wins) { return a_losses < b_losses; }
                return a_wins > b_wins;
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


void GenePool::breed(unsigned long long num_children_per_organism) noexcept {
    const std::size_t original_size = organisms.size();
    for (std::size_t i = 0; i < original_size; ++i) {
        const Organism &parent = organisms[i];
        for (unsigned long long j = 0; j < num_children_per_organism; ++j) {
            organisms.push_back({mutate(parent.genome), 0, 0, 0});
        }
    }
}
