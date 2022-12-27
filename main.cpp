#include <cstddef>
#include <iostream>
#include <string>

#include "ChessEngine.hpp"
#include "ChessGame.hpp"


int main() {
    int num_draws = 0;
    int num_white_wins = 0;
    int num_black_wins = 0;
    for (int i = 0; i < 1000; ++i) {
        ChessGame game;
        const PieceColor winner =
            game.run(new RandomEngine(), new RandomEngine(), false);
        switch (winner) {
            case PieceColor::NONE: ++num_draws; break;
            case PieceColor::WHITE: ++num_white_wins; break;
            case PieceColor::BLACK: ++num_black_wins; break;
        }
    }
    std::cout << num_white_wins << " : " << num_draws << " : " << num_black_wins
              << std::endl;
    return 0;
}
