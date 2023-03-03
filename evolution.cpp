#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>

#include "src/ChessGame.hpp"
#include "src/GenePool.hpp"
#include "src/Utilities.hpp"

int main() {
    GenePool evo_tourney;

    // Add 20 organisms to the tournament
    // Empty curly braces denotes no preferences (defaults to random move)
    for (int i = 0; i < 20; ++i) { evo_tourney.add_organism({}); }

    std::size_t round_count = 0;
    while (true) {
        std::cout << "Round " << round_count++ << '\n';

        // Let each engine play every other engine as black and white 5 times
        evo_tourney.evaluate_fitness(5);
        evo_tourney.sort_by_fitness();

        // Output round results
        const std::vector<Organism> &organisms = evo_tourney.get_organisms();
        std::vector<std::string> names;
        std::size_t max_name_w = 10;

        for (std::size_t i = 0; i < organisms.size(); ++i) {
            names.push_back(
                Engine::PreferenceChain(organisms[i].genome).get_name()
            );
            if (names[i].size() > max_name_w) { max_name_w = names[i].size(); }
        }

        std::cout << "    " << std::right << std::setw(max_name_w) << "Organism"
                  << ' ' << std::left << std::setw(6) << "Wins" << ' '
                  << std::setw(6) << "Draws" << ' ' << std::setw(6) << "Losses"
                  << ' ' << std::setw(6) << "W/L" << '\n';

        for (std::size_t i = 0; i < organisms.size(); ++i) {
            const Organism &org = organisms[i];
            std::cout << std::right << std::setw(2) << i << ". "
                      << std::setw(max_name_w) << names[i] << ' ' << std::left
                      << std::setw(6) << org.num_wins << ' ' << std::setw(6)
                      << org.num_draws << ' ' << std::setw(6) << org.num_losses
                      << ' ' << std::setw(6) << std::fixed
                      << std::setprecision(2)
                      << static_cast<double>(org.num_wins) /
                             static_cast<double>(org.num_losses)
                      << '\n';
        }
        std::cout << std::endl;

        // Kill off the bottom 10
        evo_tourney.cull(10);

        // Let each survivor create 1 offspring
        // Offspring can insert, delete, swap, or replace preference tokens
        evo_tourney.breed(1);
    }

    return EXIT_SUCCESS;
}
