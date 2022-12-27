#include "ChessMove.hpp"


std::ostream &operator<<(std::ostream &os, const ChessSquare &square) {
    os << static_cast<char>('a' + square.file);
    os << static_cast<char>('1' + square.rank);
    return os;
}


std::ostream &operator<<(std::ostream &os, const ChessMove &move) {
    os << move.get_src();
    os << move.get_dst();
    switch (move.get_promotion_type()) {
        case PieceType::NONE: break;
        case PieceType::KING: __builtin_unreachable();
        case PieceType::QUEEN: os << 'q'; break;
        case PieceType::ROOK: os << 'r'; break;
        case PieceType::BISHOP: os << 'b'; break;
        case PieceType::KNIGHT: os << 'n'; break;
        case PieceType::PAWN: __builtin_unreachable();
    }
    return os;
}
