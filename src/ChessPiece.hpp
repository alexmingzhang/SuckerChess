#ifndef SUCKER_CHESS_CHESS_PIECE_HPP
#define SUCKER_CHESS_CHESS_PIECE_HPP

#include <cassert> // for assert
#include <compare> // for operator<=>
#include <cstdint> // for std::uint8_t
#include <ostream> // for std::ostream


enum class PieceColor : std::uint8_t {
    NONE = 0,
    WHITE = 1,
    BLACK = 2,
}; // enum class PieceColor


[[nodiscard]] constexpr PieceColor operator!(PieceColor color) noexcept {
    switch (color) {
        case PieceColor::NONE: return PieceColor::NONE;
        case PieceColor::WHITE: return PieceColor::BLACK;
        case PieceColor::BLACK: return PieceColor::WHITE;
    }
    __builtin_unreachable();
}


enum class PieceType : std::uint8_t {
    NONE = 0,
    KING = 1,
    QUEEN = 2,
    ROOK = 3,
    BISHOP = 4,
    KNIGHT = 5,
    PAWN = 6,
}; // enum class PieceType


class ChessPiece final {

#ifdef SUCKER_CHESS_USE_COMPRESSED_CHESS_PIECE
    std::uint8_t data;
#else
    PieceColor m_color;
    PieceType m_type;
#endif

public: // ======================================================== CONSTRUCTORS

#ifdef SUCKER_CHESS_USE_COMPRESSED_CHESS_PIECE

    constexpr ChessPiece() noexcept
        : data(0) {}

    constexpr ChessPiece(PieceColor color, PieceType type) noexcept
        : data(static_cast<std::uint8_t>(
              (static_cast<std::uint8_t>(color) << 3) |
              static_cast<std::uint8_t>(type)
          )) {
        assert((color == PieceColor::NONE) == (type == PieceType::NONE));
    }

#else

    constexpr ChessPiece() noexcept
        : m_color(PieceColor::NONE)
        , m_type(PieceType::NONE) {}

    constexpr ChessPiece(PieceColor color, PieceType type) noexcept
        : m_color(color)
        , m_type(type) {
        assert((color == PieceColor::NONE) == (type == PieceType::NONE));
    }

#endif

public: // =========================================================== ACCESSORS

#ifdef SUCKER_CHESS_USE_COMPRESSED_CHESS_PIECE

    [[nodiscard]] constexpr PieceColor get_color() const noexcept {
        const std::uint8_t value = data >> 3;
        assert(value <= 2);
        return static_cast<PieceColor>(value);
    }

    [[nodiscard]] constexpr PieceType get_type() const noexcept {
        const std::uint8_t value = data & 0x07;
        assert(value <= 6);
        return static_cast<PieceType>(value);
    }

    [[nodiscard]] constexpr std::uint8_t get_id() const noexcept {
        return data;
    }

#else

    [[nodiscard]] constexpr PieceColor get_color() const noexcept {
        return m_color;
    }

    [[nodiscard]] constexpr PieceType get_type() const noexcept {
        return m_type;
    }

    [[nodiscard]] constexpr std::uint8_t get_id() const noexcept {
        return static_cast<std::uint8_t>(
            (static_cast<std::uint8_t>(m_color) << 3) |
            static_cast<std::uint8_t>(m_type)
        );
    }

#endif

public: // ========================================================== COMPARISON

    constexpr bool operator<=>(const ChessPiece &) const noexcept = default;

public: // =========================================================== PROMOTION

    [[nodiscard]] constexpr ChessPiece promote(PieceType promotion_type
    ) const noexcept {
        assert(promotion_type != PieceType::KING);
        assert(promotion_type != PieceType::PAWN);
        if (promotion_type == PieceType::NONE) { return *this; }
        return {get_color(), promotion_type};
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
