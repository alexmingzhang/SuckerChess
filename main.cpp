#include <iostream>

#include "src/ChessGame.hpp"
#include "src/ChessPosition.hpp"
#include "src/Utilities.hpp"


static void self_test() {
    auto rng = properly_seeded_random_engine();
    std::size_t count = 0;
    while (true) {
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
            if (!pos.is_legal(chosen_move)) {
                std::cerr << "FATAL ERROR: Illegal chess move found."
                          << std::endl;
                std::cerr << pos << std::endl;
                std::cerr << pos.get_fen() << std::endl;
                std::cerr << chosen_move << std::endl;
                return;
            }
            pos.make_move(random_choice(rng, moves));
        }
        ++count;
        std::cout << "Completed " << count
                  << ((count == 1) ? " random self-test game."
                                   : " random self-test games.")
                  << std::endl;
    }
}


static void benchmark(unsigned long num_games) {
    ChessEngine *const white = new Engine::Random();
    ChessEngine *const black = new Engine::Random();
    for (unsigned long i = 0; i < num_games; ++i) {
        ChessGame game;
        game.run(white, black, false);
    }
    delete white;
    delete black;
}


int main() { benchmark(1000); }
