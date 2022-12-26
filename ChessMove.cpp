#include "ChessMove.hpp"


std::ostream &operator<<(std::ostream &os, const ChessMove &move) {
    os << static_cast<char>('a' + move.src_file);
    os << static_cast<char>('1' + move.src_rank);
    os << static_cast<char>('a' + move.dst_file);
    os << static_cast<char>('1' + move.dst_rank);
    switch (move.promotion_type) {
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
