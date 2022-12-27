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
        : color(PieceColor::NONE)
        , type(PieceType::NONE) {}

    constexpr ChessPiece(PieceColor color, PieceType type) noexcept
        : color(color)
        , type(type) {}

    constexpr bool operator==(const ChessPiece &) const noexcept = default;

    constexpr int get_material_value() const {
        int value;

        switch (type) {
            case PieceType::NONE: [[fallthrough]];
            case PieceType::KING: return 0;
            case PieceType::QUEEN: value = 10;
            case PieceType::ROOK: value = 5;
            case PieceType::BISHOP: value = 3;
            case PieceType::KNIGHT: value = 3;
            case PieceType::PAWN: value = 1;
        }

        if (color == PieceColor::BLACK) { value *= -1; }
        return value;
    }

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
