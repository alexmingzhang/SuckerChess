#ifndef SUCKER_CHESS_CHESS_PIECE_HPP
#define SUCKER_CHESS_CHESS_PIECE_HPP

#include <cassert> // for assert
#include <cstdint> // for std::uint8_t
#include <ostream> // for std::ostream


enum class PieceColor : std::uint8_t {
    NONE = 0,
    WHITE = 1,
    BLACK = 2,
}; // enum class PieceColor


enum class PieceType : std::uint8_t {
    NONE = 0,
    KING = 1,
    QUEEN = 2,
    ROOK = 3,
    BISHOP = 4,
    KNIGHT = 5,
    PAWN = 6,
}; // enum class PieceType


class ChessPiece {

    std::uint8_t data;

public: // ======================================================== CONSTRUCTORS

    constexpr ChessPiece() noexcept
        : data(0) {}

    constexpr ChessPiece(PieceColor color, PieceType type) noexcept
        : data(
              (static_cast<std::uint8_t>(color) << 4) |
              static_cast<std::uint8_t>(type)
          ) {}

public: // =========================================================== ACCESSORS

    [[nodiscard]] constexpr PieceColor get_color() const noexcept {
        const std::uint8_t value = data >> 4;
        assert(value <= 2);
        return static_cast<PieceColor>(value);
    }

    [[nodiscard]] constexpr PieceType get_type() const noexcept {
        const std::uint8_t value = data & 0x0F;
        assert(value <= 6);
        return static_cast<PieceType>(value);
    }

    [[nodiscard]] constexpr ChessPiece promote(PieceType type) const noexcept {
        assert(type != PieceType::KING);
        assert(type != PieceType::PAWN);
        if (type == PieceType::NONE) { return *this; }
        return {get_color(), type};
    }

public: // ========================================================== COMPARISON

    constexpr bool operator<=>(const ChessPiece &) const noexcept = default;

public: // ========================================================== EVALUATION

    [[nodiscard]] constexpr int get_material_value() const {
        int value;
        switch (get_type()) {
            case PieceType::NONE: return 0;
            case PieceType::KING: return 0;
            case PieceType::QUEEN: value = 9; break;
            case PieceType::ROOK: value = 5; break;
            case PieceType::BISHOP: value = 3; break;
            case PieceType::KNIGHT: value = 3; break;
            case PieceType::PAWN: value = 1; break;
        }
        if (get_color() == PieceColor::BLACK) { value *= -1; }
        return value;
    }

}; // class ChessPiece


std::ostream &operator<<(std::ostream &, const ChessPiece &);


constexpr ChessPiece WHITE_KING = {PieceColor::WHITE, PieceType::KING};
constexpr ChessPiece WHITE_QUEEN = {PieceColor::WHITE, PieceType::QUEEN};
constexpr ChessPiece WHITE_ROOK = {PieceColor::WHITE, PieceType::ROOK};
constexpr ChessPiece WHITE_BISHOP = {PieceColor::WHITE, PieceType::BISHOP};
constexpr ChessPiece WHITE_KNIGHT = {PieceColor::WHITE, PieceType::KNIGHT};
constexpr ChessPiece WHITE_PAWN = {PieceColor::WHITE, PieceType::PAWN};

constexpr ChessPiece BLACK_KING = {PieceColor::BLACK, PieceType::KING};
constexpr ChessPiece BLACK_QUEEN = {PieceColor::BLACK, PieceType::QUEEN};
constexpr ChessPiece BLACK_ROOK = {PieceColor::BLACK, PieceType::ROOK};
constexpr ChessPiece BLACK_BISHOP = {PieceColor::BLACK, PieceType::BISHOP};
constexpr ChessPiece BLACK_KNIGHT = {PieceColor::BLACK, PieceType::KNIGHT};
constexpr ChessPiece BLACK_PAWN = {PieceColor::BLACK, PieceType::PAWN};

constexpr ChessPiece EMPTY_SQUARE = {PieceColor::NONE, PieceType::NONE};


#endif // SUCKER_CHESS_CHESS_PIECE_HPP
