#include <cstddef>
#include <iostream>
#include <string>

#include "ChessGame.hpp"
#include "ChessPlayer.hpp"


int main() {

    ChessGame game;
    game.run(new RandomPlayer(), new RandomPlayer(), false);

    for (std::size_t i = 0; i < game.move_history.size(); ++i) {
        const std::string name = game.pos_history[i].get_move_name(
            game.pos_history[i].get_legal_moves(), game.move_history[i]
        );
        if (i % 2 == 0) {
            std::cout << (i / 2 + 1) << ". " << name;
        } else {
            std::cout << " " << name << std::endl;
        }
    }

    return 0;
}
