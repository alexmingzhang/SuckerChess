#include "ChessPiece.hpp"


std::ostream &operator<<(std::ostream &os, const ChessPiece &piece) {
    switch (piece.color) {
    case PieceColor::NONE:
        os << ' ';
        break;
    case PieceColor::WHITE:
        os << 'W';
        break;
    case PieceColor::BLACK:
        os << 'B';
        break;
    }
    switch (piece.type) {
    case PieceType::NONE:
        os << ' ';
        break;
    case PieceType::KING:
        os << 'K';
        break;
    case PieceType::QUEEN:
        os << 'Q';
        break;
    case PieceType::ROOK:
        os << 'R';
        break;
    case PieceType::BISHOP:
        os << 'B';
        break;
    case PieceType::KNIGHT:
        os << 'N';
        break;
    case PieceType::PAWN:
        os << 'P';
        break;
    }
    return os;
}
