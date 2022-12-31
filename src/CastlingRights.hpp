#ifndef SUCKER_CHESS_CASTLING_RIGHTS_HPP
#define SUCKER_CHESS_CASTLING_RIGHTS_HPP

#include <cassert> // for assert
#include <compare> // for operator<=>
#include <cstdint> // for std::uint8_t
#include <ostream> // for std::ostream
#include <string>  // for std::string


class CastlingRights final {

    std::uint8_t data;

    static constexpr std::uint8_t WHITE_SHORT = 0x08;
    static constexpr std::uint8_t WHITE_LONG = 0x04;
    static constexpr std::uint8_t BLACK_SHORT = 0x02;
    static constexpr std::uint8_t BLACK_LONG = 0x01;

public: // ======================================================== CONSTRUCTORS

    explicit constexpr CastlingRights(
        bool white_can_short_castle,
        bool white_can_long_castle,
        bool black_can_short_castle,
        bool black_can_long_castle
    ) noexcept
        : data(0) {
        if (white_can_short_castle) { data |= WHITE_SHORT; }
        if (white_can_long_castle) { data |= WHITE_LONG; }
        if (black_can_short_castle) { data |= BLACK_SHORT; }
        if (black_can_long_castle) { data |= BLACK_LONG; }
    }

    explicit CastlingRights(const std::string &fen_rights_str);

public: // ========================================================== COMPARISON

    constexpr auto operator<=>(const CastlingRights &) const noexcept = default;

public: // =========================================================== ACCESSORS

    [[nodiscard]] constexpr bool white_can_short_castle() const noexcept {
        return (data & WHITE_SHORT) != 0;
    }

    [[nodiscard]] constexpr bool white_can_long_castle() const noexcept {
        return (data & WHITE_LONG) != 0;
    }

    [[nodiscard]] constexpr bool black_can_short_castle() const noexcept {
        return (data & BLACK_SHORT) != 0;
    }

    [[nodiscard]] constexpr bool black_can_long_castle() const noexcept {
        return (data & BLACK_LONG) != 0;
    }

public: // ============================================================ MUTATORS

    constexpr void disallow_white_short_castle() noexcept {
        data &= ~WHITE_SHORT;
        assert(!white_can_short_castle());
    }

    constexpr void disallow_white_long_castle() noexcept {
        data &= ~WHITE_LONG;
        assert(!white_can_long_castle());
    }

    constexpr void disallow_black_short_castle() noexcept {
        data &= ~BLACK_SHORT;
        assert(!black_can_short_castle());
    }

    constexpr void disallow_black_long_castle() noexcept {
        data &= ~BLACK_LONG;
        assert(!black_can_long_castle());
    }

}; // class CastlingRights


static_assert(sizeof(CastlingRights) == 1);


std::ostream &operator<<(std::ostream &os, CastlingRights rights);


#endif // SUCKER_CHESS_CASTLING_RIGHTS_HPP
