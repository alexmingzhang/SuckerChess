#ifndef SUCKER_CHESS_CHESS_MOVE_HPP
#define SUCKER_CHESS_CHESS_MOVE_HPP

#include <algorithm> // for std::max
#include <cassert>   // for assert
#include <cmath>     // for std::abs
#include <compare>   // for operator<=>
#include <cstdint>   // for std::uint16_t
#include <ostream>   // for std::ostream

#include "ChessPiece.hpp"


using coord_t = int;
constexpr coord_t NUM_FILES = 8;
constexpr coord_t NUM_RANKS = 8;


struct ChessOffset {

    coord_t file_offset;
    coord_t rank_offset;

}; // struct ChessOffset


struct ChessSquare final {

    coord_t file;
    coord_t rank;

    [[nodiscard]] constexpr bool in_bounds() const noexcept {
        return (file >= 0) && (file < NUM_FILES) && (rank >= 0) &&
               (rank < NUM_RANKS);
    }

    constexpr auto operator<=>(const ChessSquare &) const noexcept = default;

    constexpr ChessSquare &operator+=(ChessOffset offset) noexcept {
        file += offset.file_offset;
        rank += offset.rank_offset;
        return *this;
    }

    constexpr ChessSquare operator+(ChessOffset offset) const noexcept {
        return {file + offset.file_offset, rank + offset.rank_offset};
    }

}; // struct ChessSquare


std::ostream &operator<<(std::ostream &os, const ChessSquare &square);


class ChessMove final {

#ifdef SUCKER_CHESS_USE_COMPRESSED_CHESS_MOVE
    std::uint16_t data;
#else
    ChessSquare src;
    ChessSquare dst;
    PieceType promotion_type;
#endif

public: // ======================================================== CONSTRUCTORS

#ifdef SUCKER_CHESS_USE_COMPRESSED_CHESS_MOVE

    constexpr ChessMove(ChessSquare source, ChessSquare destination)
        : data(static_cast<std::uint16_t>(
              (source.file << 12) | (source.rank << 9) |
              (destination.file << 6) | (destination.rank << 3)
          )) {
        assert(source.in_bounds());
        assert(destination.in_bounds());
    }

    constexpr ChessMove(
        ChessSquare source, ChessSquare destination, PieceType type
    )
        : data(static_cast<std::uint16_t>(
              (source.file << 12) | (source.rank << 9) |
              (destination.file << 6) | (destination.rank << 3) |
              static_cast<std::uint16_t>(type)
          )) {
        assert(source.in_bounds());
        assert(destination.in_bounds());
        assert(type != PieceType::KING && type != PieceType::PAWN);
    }

#else

    constexpr ChessMove(ChessSquare source, ChessSquare destination)
        : src(source)
        , dst(destination)
        , promotion_type(PieceType::NONE) {
        assert(source.in_bounds());
        assert(destination.in_bounds());
    }

    constexpr ChessMove(
        ChessSquare source, ChessSquare destination, PieceType type
    )
        : src(source)
        , dst(destination)
        , promotion_type(type) {
        assert(source.in_bounds());
        assert(destination.in_bounds());
        assert(type != PieceType::KING && type != PieceType::PAWN);
    }

#endif

public: // =========================================================== ACCESSORS

#ifdef SUCKER_CHESS_USE_COMPRESSED_CHESS_MOVE

    [[nodiscard]] constexpr coord_t get_src_file() const noexcept {
        const auto result = static_cast<coord_t>(data >> 12);
        assert(result >= 0 && result < NUM_FILES);
        return result;
    }

    [[nodiscard]] constexpr coord_t get_src_rank() const noexcept {
        const auto result = static_cast<coord_t>((data >> 9) & 7);
        assert(result >= 0 && result < NUM_RANKS);
        return result;
    }

    [[nodiscard]] constexpr ChessSquare get_src() const noexcept {
        return {get_src_file(), get_src_rank()};
    }

    [[nodiscard]] constexpr coord_t get_dst_file() const noexcept {
        const auto result = static_cast<coord_t>((data >> 6) & 7);
        assert(result >= 0 && result < NUM_FILES);
        return result;
    }

    [[nodiscard]] constexpr coord_t get_dst_rank() const noexcept {
        const auto result = static_cast<coord_t>((data >> 3) & 7);
        assert(result >= 0 && result < NUM_RANKS);
        return result;
    }

    [[nodiscard]] constexpr ChessSquare get_dst() const noexcept {
        return {get_dst_file(), get_dst_rank()};
    }

    [[nodiscard]] constexpr PieceType get_promotion_type() const noexcept {
        using enum PieceType;
        const auto result = static_cast<PieceType>(data & 7);
        assert(
            result == NONE || result == QUEEN || result == ROOK ||
            result == BISHOP || result == KNIGHT
        );
        return result;
    }

#else

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

#endif

public: // ========================================================== COMPARISON

    constexpr auto operator<=>(const ChessMove &) const noexcept = default;

public: // ======================================================= STATE TESTING

    [[nodiscard]] constexpr bool affects(ChessSquare square) const noexcept {
        return get_src() == square || get_dst() == square;
    }

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
