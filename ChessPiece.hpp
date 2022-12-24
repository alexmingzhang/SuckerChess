#ifndef SUCKER_CHESS_CHESS_PIECE_HPP
#define SUCKER_CHESS_CHESS_PIECE_HPP

#include <cstdint> // for std::uint8_t
#include <ostream> // for std::ostream


enum class PieceColor : std::uint8_t {
    NONE,
    WHITE,
    BLACK,
}; // enum class PieceColor


enum class PieceType : std::uint8_t {
    NONE,
    KING,
    QUEEN,
    ROOK,
    BISHOP,
    KNIGHT,
    PAWN,
}; // enum class PieceType


struct ChessPiece {

    PieceColor color;
    PieceType type;

    constexpr ChessPiece() noexcept
        : color(PieceColor::NONE), type(PieceType::NONE) {}

    constexpr ChessPiece(PieceColor color, PieceType type) noexcept
        : color(color), type(type) {}

    constexpr auto operator<=>(const ChessPiece &) const noexcept = default;

}; // struct ChessPiece


std::ostream &operator<<(std::ostream &, const ChessPiece &);


constexpr ChessPiece WHITE_KING(PieceColor::WHITE, PieceType::KING);
constexpr ChessPiece WHITE_QUEEN(PieceColor::WHITE, PieceType::QUEEN);
constexpr ChessPiece WHITE_ROOK(PieceColor::WHITE, PieceType::ROOK);
constexpr ChessPiece WHITE_BISHOP(PieceColor::WHITE, PieceType::BISHOP);
constexpr ChessPiece WHITE_KNIGHT(PieceColor::WHITE, PieceType::KNIGHT);
constexpr ChessPiece WHITE_PAWN(PieceColor::WHITE, PieceType::PAWN);

constexpr ChessPiece BLACK_KING(PieceColor::BLACK, PieceType::KING);
constexpr ChessPiece BLACK_QUEEN(PieceColor::BLACK, PieceType::QUEEN);
constexpr ChessPiece BLACK_ROOK(PieceColor::BLACK, PieceType::ROOK);
constexpr ChessPiece BLACK_BISHOP(PieceColor::BLACK, PieceType::BISHOP);
constexpr ChessPiece BLACK_KNIGHT(PieceColor::BLACK, PieceType::KNIGHT);
constexpr ChessPiece BLACK_PAWN(PieceColor::BLACK, PieceType::PAWN);

constexpr ChessPiece EMPTY_SQUARE(PieceColor::NONE, PieceType::NONE);


#endif // SUCKER_CHESS_CHESS_PIECE_HPP
