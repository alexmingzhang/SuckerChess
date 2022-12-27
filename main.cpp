#include <cstddef>
#include <iostream>
#include <string>

#include "ChessGame.hpp"
#include "ChessPlayer.hpp"


int main() {

    int num_draws = 0;
    int num_white_wins = 0;
    int num_black_wins = 0;

    ChessGame game;
    const PieceColor winner =
        game.run(new RandomPlayer(), new CCCP_Player(), true);

    switch (winner) {
        case PieceColor::NONE: ++num_draws; break;
        case PieceColor::WHITE: ++num_white_wins; break;
        case PieceColor::BLACK: ++num_black_wins; break;
    }

    std::cout << num_white_wins << " : " << num_draws << " : " << num_black_wins
              << std::endl;

    std::cout << game.get_PGN() << std::endl;


    return 0;
}
