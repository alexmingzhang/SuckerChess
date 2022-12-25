#include "ChessPosition.hpp"


std::ostream &operator<<(std::ostream &os, const ChessPosition &b) {
    os << "   a  b  c  d  e  f  g  h\n";
    for (coord_t rank = NUM_RANKS - 1; rank >= 0; --rank) {
        os << rank + 1 << " |";
        for (coord_t file = 0; file < NUM_FILES; ++file) {
            os << b.board[file][rank] << '|';
        }
        os << "\n";
    }
    return os;
}
