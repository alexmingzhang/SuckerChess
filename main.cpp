#include <cstddef>
#include <iostream>
#include <string>

#include "ChessGame.hpp"
#include "ChessPlayer.hpp"


int main() {

    int num_draws = 0;
    int num_white_wins = 0;
    int num_black_wins = 0;

    for (int i = 0; i < 10000; ++i) {
        ChessGame game;
        const PieceColor winner =
            game.run(new RandomPlayer(), new RandomPlayer(), false);
        switch (winner) {
            case PieceColor::NONE: ++num_draws; break;
            case PieceColor::WHITE: ++num_white_wins; break;
            case PieceColor::BLACK: ++num_black_wins; break;
        }
    }

    std::cout << num_white_wins << " : " << num_draws << " : " << num_black_wins
              << std::endl;

    //    for (std::size_t i = 0; i < game.move_history.size(); ++i) {
    //        const std::string name = game.pos_history[i].get_move_name(
    //            game.pos_history[i].get_legal_moves(), game.move_history[i]
    //        );
    //        if (i % 2 == 0) {
    //            std::cout << (i / 2 + 1) << ". " << name;
    //        } else {
    //            std::cout << " " << name << std::endl;
    //        }
    //    }

    return 0;
}
