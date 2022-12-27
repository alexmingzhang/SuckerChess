#ifndef SUCKER_CHESS_CHESS_MOVE_HPP
#define SUCKER_CHESS_CHESS_MOVE_HPP

#include <algorithm> // for std::max
#include <cassert>   // for assert
#include <cmath>     // for std::abs
#include <cstdint>   // for std::int8_t
#include <ostream>   // for std::ostream

#include "ChessPiece.hpp"


using coord_t = int;
constexpr coord_t NUM_FILES = 8;
constexpr coord_t NUM_RANKS = 8;


struct ChessSquare {

    coord_t file;
    coord_t rank;

    [[nodiscard]] constexpr bool in_bounds() const noexcept {
        return (file >= 0) && (file < NUM_FILES) && (rank >= 0) &&
               (rank < NUM_RANKS);
    }

    constexpr auto operator<=>(const ChessSquare &) const noexcept = default;

}; // struct ChessSquare


std::ostream &operator<<(std::ostream &os, const ChessSquare &square);


struct ChessOffset {

    coord_t file_offset;
    coord_t rank_offset;

}; // struct ChessOffset


constexpr ChessSquare &
operator+=(ChessSquare &square, const ChessOffset &offset) noexcept {
    square.file += offset.file_offset;
    square.rank += offset.rank_offset;
    return square;
}


constexpr ChessSquare
operator+(const ChessSquare &square, const ChessOffset &offset) noexcept {
    return {square.file + offset.file_offset, square.rank + offset.rank_offset};
}


class ChessMove {

    ChessSquare src;
    ChessSquare dst;
    PieceType promotion_type;

public: // ======================================================== CONSTRUCTORS

    explicit constexpr ChessMove(ChessSquare source, ChessSquare destination)
        : src(source)
        , dst(destination)
        , promotion_type(PieceType::NONE) {
        assert(source.in_bounds());
        assert(destination.in_bounds());
    }

    explicit constexpr ChessMove(
        ChessSquare source, ChessSquare destination, PieceType type
    )
        : src(source)
        , dst(destination)
        , promotion_type(type) {
        assert(source.in_bounds());
        assert(destination.in_bounds());
        assert(type != PieceType::KING && type != PieceType::PAWN);
    }

public: // =========================================================== ACCESSORS

    [[nodiscard]] constexpr ChessSquare get_src() const noexcept { return src; }

    [[nodiscard]] constexpr coord_t get_src_file() const noexcept {
        return src.file;
    }

    [[nodiscard]] constexpr coord_t get_src_rank() const noexcept {
        return src.rank;
    }

    [[nodiscard]] constexpr ChessSquare get_dst() const noexcept { return dst; }

    [[nodiscard]] constexpr coord_t get_dst_file() const noexcept {
        return dst.file;
    }

    [[nodiscard]] constexpr coord_t get_dst_rank() const noexcept {
        return dst.rank;
    }

    [[nodiscard]] constexpr PieceType get_promotion_type() const noexcept {
        return promotion_type;
    }

public: // ========================================================== COMPARISON

    constexpr auto operator<=>(const ChessMove &) const noexcept = default;

public: // ======================================================= STATE TESTING

    [[nodiscard]] constexpr bool is_orthogonal() const noexcept {
        return get_src_file() == get_dst_file() ||
               get_src_rank() == get_dst_rank();
    }

    [[nodiscard]] constexpr bool is_diagonal() const noexcept {
        return std::abs(get_src_file() - get_dst_file()) ==
               std::abs(get_src_rank() - get_dst_rank());
    }

    [[nodiscard]] constexpr coord_t distance() const noexcept {
        return std::max(
            std::abs(get_src_file() - get_dst_file()),
            std::abs(get_src_rank() - get_dst_rank())
        );
    }

}; // class ChessMove


std::ostream &operator<<(std::ostream &os, const ChessMove &move);


constexpr ChessMove NULL_MOVE({0, 0}, {0, 0});


#endif // SUCKER_CHESS_CHESS_MOVE_HPP
