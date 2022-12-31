#include <cstddef>  // for std::size_t
#include <iostream> // for std::cout, std::cerr, std::endl
#include <memory>   // for std::unique_ptr, std::make_unique
#include <string>   // for std::string

#include "src/ChessEngine.hpp"
#include "src/ChessGame.hpp"
#include "src/ChessPosition.hpp"
#include "src/Utilities.hpp"


static void self_test(unsigned long long num_games) {
    auto rng = properly_seeded_random_engine();
    for (unsigned long long count = 0; count < num_games; ++count) {
        ChessPosition pos;
        while (true) {
            if (!pos.check_consistency()) {
                std::cerr << "FATAL ERROR: Inconsistent chess position found."
                          << std::endl;
                std::cerr << pos << std::endl;
                std::cerr << pos.get_fen() << std::endl;
                return;
            }
            if (pos.get_board().has_insufficient_material()) { break; }
            const auto moves = pos.get_legal_moves();
            if (moves.empty()) { break; }
            const auto chosen_move = random_choice(rng, moves);
            if (!pos.is_valid(chosen_move)) {
                std::cerr << "FATAL ERROR: Invalid chess move found."
                          << std::endl;
                std::cerr << pos << std::endl;
                std::cerr << pos.get_fen() << std::endl;
                std::cerr << chosen_move << std::endl;
                return;
            }
            pos.make_move(random_choice(rng, moves));
        }
        std::cout << "Completed " << (count + 1)
                  << ((count == 0) ? " random self-test game."
                                   : " random self-test games.")
                  << std::endl;
    }
}


static void benchmark(unsigned long long num_games) {
    std::unique_ptr<ChessEngine> white = std::make_unique<Engine::Preference>();
    std::unique_ptr<ChessEngine> black = std::make_unique<Engine::Preference>();
    for (unsigned long long i = 0; i < num_games; ++i) {
        ChessGame game;
        game.run(white.get(), black.get(), false);
    }
}


int main(int argc, char **argv) {
    for (int i = 0; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--test") {
            self_test(100);
            return EXIT_SUCCESS;
        } else if (arg == "--benchmark") {
            benchmark(1000);
            return EXIT_SUCCESS;
        }
    }
    return EXIT_FAILURE;
}
