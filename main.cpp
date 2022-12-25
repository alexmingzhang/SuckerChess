#include <cstddef>
#include <iostream>
#include <vector>

#include "ChessMove.hpp"
#include "ChessPosition.hpp"


int main() {
    constexpr ChessPosition START_POSITION;
    constexpr auto x = START_POSITION(0, 0);
    constexpr char LETTERS[8] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
    ChessPosition pos;

    while (1) {
        std::cout << pos << std::endl;

        std::vector<ChessMove> moves = pos.get_moves();
        for (std::size_t i = 0; i < moves.size(); ++i) {
            const ChessMove &move = moves[i];
            std::cout << i << ": " << LETTERS[move.src_file]
                      << (int)move.src_rank + 1 << " to "
                      << LETTERS[move.dst_file] << (int)move.dst_rank + 1
                      << std::endl;
        }

        std::cout << "> ";
        std::size_t move_index;
        std::cin >> move_index;

        ChessMove move = moves[move_index];
        pos.make_move(move);

        std::cout << LETTERS[move.src_file] << (int)move.src_rank + 1 << " to "
                  << LETTERS[move.dst_file] << (int)move.dst_rank + 1
                  << std::endl;
    }
    return 0;
}
