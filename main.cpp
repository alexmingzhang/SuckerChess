#include <cstddef>
#include <iostream>
#include <string>

#include "ChessEngine.hpp"
#include "ChessGame.hpp"


static void benchmark(std::size_t num_games) {
    std::size_t num_draws = 0;
    std::size_t num_white_wins = 0;
    std::size_t num_black_wins = 0;
    for (std::size_t i = 0; i < num_games; ++i) {
        ChessGame game;
        ChessEngine *white = new RandomEngine();
        ChessEngine *black = new RandomEngine();
        const PieceColor winner = game.run(white, black, false);
        delete white;
        delete black;
        switch (winner) {
            case PieceColor::NONE: ++num_draws; break;
            case PieceColor::WHITE: ++num_white_wins; break;
            case PieceColor::BLACK: ++num_black_wins; break;
        }
    }
    std::cout << num_white_wins << " : " << num_draws << " : " << num_black_wins
              << std::endl;
}


int main() {
    benchmark(1000);
    return 0;
}
