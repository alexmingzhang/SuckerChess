#include "ChessPiece.hpp"


std::ostream &operator<<(std::ostream &os, const ChessPiece &piece) {
    switch (piece.color) {
        case PieceColor::NONE:
            os << ' ';
            break;
        case PieceColor::WHITE:
            switch (piece.type) {
                case PieceType::NONE:
                    os << ' ';
                    break;
                case PieceType::KING:
                    os << "♚";
                    break;
                case PieceType::QUEEN:
                    os << "♛";
                    break;
                case PieceType::ROOK:
                    os << "♜";
                    break;
                case PieceType::BISHOP:
                    os << "♝";
                    break;
                case PieceType::KNIGHT:
                    os << "♞";
                    break;
                case PieceType::PAWN:
                    os << "♟";
                    break;
            }
            break;
        case PieceColor::BLACK:
            switch (piece.type) {
                case PieceType::NONE:
                    os << ' ';
                    break;
                case PieceType::KING:
                    os << "♔";
                    break;
                case PieceType::QUEEN:
                    os << "♕";
                    break;
                case PieceType::ROOK:
                    os << "♖";
                    break;
                case PieceType::BISHOP:
                    os << "♗";
                    break;
                case PieceType::KNIGHT:
                    os << "♘";
                    break;
                case PieceType::PAWN:
                    os << "♙";
                    break;
            }
            break;
    }

    return os;
}
