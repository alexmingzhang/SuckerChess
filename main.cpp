#include <cstddef>
#include <iostream>
#include <vector>

#include "ChessMove.hpp"
#include "ChessPosition.hpp"


int main() {
    constexpr ChessPosition START_POSITION;
    ChessPosition pos;

    while (1) {
        std::cout << pos << std::endl;

        std::vector<ChessMove> moves = pos.get_moves();
        for (std::size_t i = 0; i < moves.size(); ++i) {
            const ChessMove &move = moves[i];
            std::cout << i << ": " << move << std::endl;
        }

        std::cout << "> ";
        std::size_t move_index;
        std::cin >> move_index;

        ChessMove move = moves[move_index];
        pos.make_move(move);

        std::cout << move << std::endl;
    }
    return 0;
}
