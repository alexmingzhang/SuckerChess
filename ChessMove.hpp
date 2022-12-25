#ifndef SUCKER_CHESS_CHESS_MOVE_HPP
#define SUCKER_CHESS_CHESS_MOVE_HPP

#include <algorithm> // for std::max
#include <cassert>   // for assert
#include <cmath>     // for std::abs
#include <cstdint>   // for std::int8_t
#include <ostream>   // for std::ostream

#include "ChessPiece.hpp"


using coord_t = std::int8_t;
constexpr coord_t NUM_FILES = 8;
constexpr coord_t NUM_RANKS = 8;


constexpr bool in_bounds(coord_t file, coord_t rank) {
    return (file >= 0) && (file < NUM_FILES) && (rank >= 0) &&
           (rank < NUM_RANKS);
}


struct ChessMove {

    coord_t src_file;
    coord_t src_rank;
    coord_t dst_file;
    coord_t dst_rank;
    PieceType promotion_type;

    explicit constexpr ChessMove(coord_t sf, coord_t sr, coord_t df, coord_t dr)
        : src_file(sf)
        , src_rank(sr)
        , dst_file(df)
        , dst_rank(dr)
        , promotion_type(PieceType::NONE) {
        assert(in_bounds(sf, sr));
        assert(in_bounds(df, dr));
    }

    explicit constexpr ChessMove(
        coord_t sf, coord_t sr, coord_t df, coord_t dr, PieceType pt
    )
        : src_file(sf)
        , src_rank(sr)
        , dst_file(df)
        , dst_rank(dr)
        , promotion_type(pt) {
        assert(in_bounds(sf, sr));
        assert(in_bounds(df, dr));
        assert(pt != PieceType::KING && pt != PieceType::PAWN);
    }

    constexpr auto operator<=>(const ChessMove &) const noexcept = default;

    [[nodiscard]] constexpr bool is_orthogonal() const noexcept {
        return src_file == dst_file || src_rank == dst_rank;
    }

    [[nodiscard]] constexpr bool is_diagonal() const noexcept {
        return std::abs(src_file - dst_file) == std::abs(src_rank - dst_rank);
    }

    [[nodiscard]] constexpr coord_t distance() const noexcept {
        return static_cast<coord_t>(std::max(
            std::abs(src_file - dst_file), std::abs(src_rank - dst_rank)
        ));
    }

}; // struct ChessMove


std::ostream &operator<<(std::ostream &os, const ChessMove &move);


#endif // SUCKER_CHESS_CHESS_MOVE_HPP
