#ifndef SUCKER_CHESS_CHESS_BOARD_HPP
#define SUCKER_CHESS_CHESS_BOARD_HPP

#include <array>   // for std::array
#include <cassert> // for assert
#include <cstddef> // for std::size_t
#include <ostream> // for std::ostream
#include <string>  // for std::string

#include "ChessMove.hpp"
#include "ChessPiece.hpp"


class ChessBoard final {

    std::array<std::array<ChessPiece, NUM_RANKS>, NUM_FILES> data;

public: // ========================================================= CONSTRUCTOR

    // clang-format off
    explicit constexpr ChessBoard() noexcept
        : data{{{WHITE_ROOK,   WHITE_PAWN, EMPTY_SQUARE, EMPTY_SQUARE,
                 EMPTY_SQUARE, EMPTY_SQUARE, BLACK_PAWN, BLACK_ROOK},
                {WHITE_KNIGHT, WHITE_PAWN, EMPTY_SQUARE, EMPTY_SQUARE,
                 EMPTY_SQUARE, EMPTY_SQUARE, BLACK_PAWN, BLACK_KNIGHT},
                {WHITE_BISHOP, WHITE_PAWN, EMPTY_SQUARE, EMPTY_SQUARE,
                 EMPTY_SQUARE, EMPTY_SQUARE, BLACK_PAWN, BLACK_BISHOP},
                {WHITE_QUEEN,  WHITE_PAWN, EMPTY_SQUARE, EMPTY_SQUARE,
                 EMPTY_SQUARE, EMPTY_SQUARE, BLACK_PAWN, BLACK_QUEEN},
                {WHITE_KING,   WHITE_PAWN, EMPTY_SQUARE, EMPTY_SQUARE,
                 EMPTY_SQUARE, EMPTY_SQUARE, BLACK_PAWN, BLACK_KING},
                {WHITE_BISHOP, WHITE_PAWN, EMPTY_SQUARE, EMPTY_SQUARE,
                 EMPTY_SQUARE, EMPTY_SQUARE, BLACK_PAWN, BLACK_BISHOP},
                {WHITE_KNIGHT, WHITE_PAWN, EMPTY_SQUARE, EMPTY_SQUARE,
                 EMPTY_SQUARE, EMPTY_SQUARE, BLACK_PAWN, BLACK_KNIGHT},
                {WHITE_ROOK,   WHITE_PAWN, EMPTY_SQUARE, EMPTY_SQUARE,
                 EMPTY_SQUARE, EMPTY_SQUARE, BLACK_PAWN, BLACK_ROOK}}} {}
    // clang-format on

    explicit ChessBoard(const std::string &fen_board_str);

public: // =========================================================== ACCESSORS

    [[nodiscard]] constexpr ChessPiece get_piece(ChessSquare square
    ) const noexcept {
        assert(square.in_bounds());
        return data[static_cast<std::size_t>(square.file)]
                   [static_cast<std::size_t>(square.rank)];
    }

    [[nodiscard]] constexpr ChessPiece
    get_piece(coord_t file, coord_t rank) const noexcept {
        return get_piece({file, rank});
    }

public: // ============================================================ MUTATORS

    constexpr void set_piece(ChessSquare square, ChessPiece piece) noexcept {
        assert(square.in_bounds());
        data[static_cast<std::size_t>(square.file)]
            [static_cast<std::size_t>(square.rank)] = piece;
    }

    constexpr void
    set_piece(coord_t file, coord_t rank, ChessPiece piece) noexcept {
        set_piece({file, rank}, piece);
    }

public: // ========================================================== COMPARISON

    constexpr bool operator==(const ChessBoard &) const noexcept = default;

public: // ======================================================= STATE TESTING

    [[nodiscard]] constexpr bool in_bounds_and_empty(ChessSquare square
    ) const noexcept {
        return square.in_bounds() && (get_piece(square) == EMPTY_SQUARE);
    }

    [[nodiscard]] constexpr bool in_bounds_and_has_piece(
        ChessSquare square, ChessPiece piece
    ) const noexcept {
        return square.in_bounds() && (get_piece(square) == piece);
    }

public: // =========================================================== SEARCHING

    [[nodiscard]] constexpr ChessSquare find_first_piece(ChessPiece piece
    ) const noexcept {
        for (coord_t file = 0; file < NUM_FILES; ++file) {
            for (coord_t rank = 0; rank < NUM_RANKS; ++rank) {
                const ChessSquare square = {file, rank};
                if (get_piece(square) == piece) { return square; }
            }
        }
        __builtin_unreachable();
    }

    [[nodiscard]] ChessSquare find_unique_piece(ChessPiece piece) const;

public: // ============================================================ COUNTING

    [[nodiscard]] constexpr int count(ChessPiece piece) const noexcept {
        int result = 0;
        for (coord_t file = 0; file < NUM_FILES; ++file) {
            for (coord_t rank = 0; rank < NUM_RANKS; ++rank) {
                if (get_piece(file, rank) == piece) { ++result; }
            }
        }
        return result;
    }

    [[nodiscard]] constexpr bool has_insufficient_material() const noexcept {
        // If either side has a queen, rook, or pawn,
        // then checkmate is possible.
        if (count(WHITE_QUEEN) != 0) { return false; }
        if (count(WHITE_ROOK) != 0) { return false; }
        if (count(WHITE_PAWN) != 0) { return false; }
        if (count(BLACK_QUEEN) != 0) { return false; }
        if (count(BLACK_ROOK) != 0) { return false; }
        if (count(BLACK_PAWN) != 0) { return false; }
        // If there is only one minor piece remaining on the board,
        // then checkmate is impossible.
        const int white_piece_count = count(WHITE_BISHOP) + count(WHITE_KNIGHT);
        const int black_piece_count = count(BLACK_BISHOP) + count(BLACK_KNIGHT);
        return (white_piece_count + black_piece_count <= 1);
    }

public: // ====================================================== PAWN UTILITIES

    [[nodiscard]] static constexpr coord_t pawn_direction(PieceColor color
    ) noexcept {
        switch (color) {
            case PieceColor::NONE: __builtin_unreachable();
            case PieceColor::WHITE: return +1;
            case PieceColor::BLACK: return -1;
        }
        __builtin_unreachable();
    }

    [[nodiscard]] static constexpr coord_t pawn_origin_rank(PieceColor color
    ) noexcept {
        switch (color) {
            case PieceColor::NONE: __builtin_unreachable();
            case PieceColor::WHITE: return 1;
            case PieceColor::BLACK: return NUM_RANKS - 2;
        }
        __builtin_unreachable();
    }

    [[nodiscard]] static constexpr coord_t promotion_rank(PieceColor color
    ) noexcept {
        switch (color) {
            case PieceColor::NONE: __builtin_unreachable();
            case PieceColor::WHITE: return NUM_RANKS - 1;
            case PieceColor::BLACK: return 0;
        }
        __builtin_unreachable();
    }

public: // ====================================================== LEAPER ATTACKS

    [[nodiscard]] constexpr bool
    is_attacked_by_king(PieceColor color, ChessSquare square) const noexcept {
        const ChessPiece king = {color, PieceType::KING};
        return in_bounds_and_has_piece(square.shift(-1, -1), king) ||
               in_bounds_and_has_piece(square.shift(-1, 0), king) ||
               in_bounds_and_has_piece(square.shift(-1, +1), king) ||
               in_bounds_and_has_piece(square.shift(0, -1), king) ||
               in_bounds_and_has_piece(square.shift(0, +1), king) ||
               in_bounds_and_has_piece(square.shift(+1, -1), king) ||
               in_bounds_and_has_piece(square.shift(+1, 0), king) ||
               in_bounds_and_has_piece(square.shift(+1, +1), king);
    }

    [[nodiscard]] constexpr int
    count_king_attacks(PieceColor color, ChessSquare square) const noexcept {
        const ChessPiece king = {color, PieceType::KING};
        int result = 0;
        if (in_bounds_and_has_piece(square.shift(-1, -1), king)) { ++result; }
        if (in_bounds_and_has_piece(square.shift(-1, 0), king)) { ++result; }
        if (in_bounds_and_has_piece(square.shift(-1, +1), king)) { ++result; }
        if (in_bounds_and_has_piece(square.shift(0, -1), king)) { ++result; }
        if (in_bounds_and_has_piece(square.shift(0, +1), king)) { ++result; }
        if (in_bounds_and_has_piece(square.shift(+1, -1), king)) { ++result; }
        if (in_bounds_and_has_piece(square.shift(+1, 0), king)) { ++result; }
        if (in_bounds_and_has_piece(square.shift(+1, +1), king)) { ++result; }
        return result;
    }

    [[nodiscard]] constexpr bool
    is_attacked_by_knight(PieceColor color, ChessSquare square) const noexcept {
        const ChessPiece knight = {color, PieceType::KNIGHT};
        return in_bounds_and_has_piece(square.shift(-2, -1), knight) ||
               in_bounds_and_has_piece(square.shift(-2, +1), knight) ||
               in_bounds_and_has_piece(square.shift(-1, -2), knight) ||
               in_bounds_and_has_piece(square.shift(-1, +2), knight) ||
               in_bounds_and_has_piece(square.shift(+1, -2), knight) ||
               in_bounds_and_has_piece(square.shift(+1, +2), knight) ||
               in_bounds_and_has_piece(square.shift(+2, -1), knight) ||
               in_bounds_and_has_piece(square.shift(+2, +1), knight);
    }

    [[nodiscard]] constexpr int
    count_knight_attacks(PieceColor color, ChessSquare square) const noexcept {
        const ChessPiece knight = {color, PieceType::KNIGHT};
        int result = 0;
        if (in_bounds_and_has_piece(square.shift(-2, -1), knight)) { ++result; }
        if (in_bounds_and_has_piece(square.shift(-2, +1), knight)) { ++result; }
        if (in_bounds_and_has_piece(square.shift(-1, -2), knight)) { ++result; }
        if (in_bounds_and_has_piece(square.shift(-1, +2), knight)) { ++result; }
        if (in_bounds_and_has_piece(square.shift(+1, -2), knight)) { ++result; }
        if (in_bounds_and_has_piece(square.shift(+1, +2), knight)) { ++result; }
        if (in_bounds_and_has_piece(square.shift(+2, -1), knight)) { ++result; }
        if (in_bounds_and_has_piece(square.shift(+2, +1), knight)) { ++result; }
        return result;
    }

    [[nodiscard]] constexpr bool
    is_attacked_by_pawn(PieceColor color, ChessSquare square) const noexcept {
        const ChessPiece pawn = {color, PieceType::PAWN};
        const coord_t direction = pawn_direction(color);
        return in_bounds_and_has_piece(square.shift(-1, -direction), pawn) ||
               in_bounds_and_has_piece(square.shift(+1, -direction), pawn);
    }

    [[nodiscard]] constexpr int
    count_pawn_attacks(PieceColor color, ChessSquare square) const noexcept {
        const ChessPiece pawn = {color, PieceType::PAWN};
        const coord_t direction = pawn_direction(color);
        return in_bounds_and_has_piece(square.shift(-1, -direction), pawn) +
               in_bounds_and_has_piece(square.shift(+1, -direction), pawn);
    }

private: // ============================================== SLIDER ATTACK HELPERS

    [[nodiscard]] constexpr ChessPiece find_slider(
        PieceColor color,
        ChessSquare square,
        coord_t file_offset,
        coord_t rank_offset
    ) const noexcept {
        ChessSquare current = square.shift(file_offset, rank_offset);
        while (in_bounds_and_empty(current)) {
            current = current.shift(file_offset, rank_offset);
        }
        if (current.in_bounds()) {
            const ChessPiece piece = get_piece(current);
            if (piece.get_color() == color) { return piece; }
        }
        return EMPTY_SQUARE;
    }

private: // ===================================================== SLIDER ATTACKS

    [[nodiscard]] constexpr bool is_attacked_orthogonally(
        PieceColor color, ChessSquare square
    ) const noexcept {
        using enum PieceType;
        const ChessPiece a = find_slider(color, square, -1, 0);
        if (a.get_type() == QUEEN || a.get_type() == ROOK) { return true; }
        const ChessPiece b = find_slider(color, square, 0, -1);
        if (b.get_type() == QUEEN || b.get_type() == ROOK) { return true; }
        const ChessPiece c = find_slider(color, square, 0, +1);
        if (c.get_type() == QUEEN || c.get_type() == ROOK) { return true; }
        const ChessPiece d = find_slider(color, square, +1, 0);
        if (d.get_type() == QUEEN || d.get_type() == ROOK) { return true; }
        return false;
    }

    [[nodiscard]] constexpr int count_orthogonal_attacks(
        PieceColor color, ChessSquare square
    ) const noexcept {
        using enum PieceType;
        int result = 0;
        const ChessPiece a = find_slider(color, square, -1, 0);
        if (a.get_type() == QUEEN || a.get_type() == ROOK) { ++result; }
        const ChessPiece b = find_slider(color, square, 0, -1);
        if (b.get_type() == QUEEN || b.get_type() == ROOK) { ++result; }
        const ChessPiece c = find_slider(color, square, 0, +1);
        if (c.get_type() == QUEEN || c.get_type() == ROOK) { ++result; }
        const ChessPiece d = find_slider(color, square, +1, 0);
        if (d.get_type() == QUEEN || d.get_type() == ROOK) { ++result; }
        return result;
    }

    [[nodiscard]] constexpr bool is_attacked_diagonally(
        PieceColor color, ChessSquare square
    ) const noexcept {
        using enum PieceType;
        const ChessPiece a = find_slider(color, square, -1, -1);
        if (a.get_type() == QUEEN || a.get_type() == BISHOP) { return true; }
        const ChessPiece b = find_slider(color, square, -1, +1);
        if (b.get_type() == QUEEN || b.get_type() == BISHOP) { return true; }
        const ChessPiece c = find_slider(color, square, +1, -1);
        if (c.get_type() == QUEEN || c.get_type() == BISHOP) { return true; }
        const ChessPiece d = find_slider(color, square, +1, +1);
        if (d.get_type() == QUEEN || d.get_type() == BISHOP) { return true; }
        return false;
    }

    [[nodiscard]] constexpr int count_diagonal_attacks(
        PieceColor color, ChessSquare square
    ) const noexcept {
        using enum PieceType;
        int result = 0;
        const ChessPiece a = find_slider(color, square, -1, -1);
        if (a.get_type() == QUEEN || a.get_type() == BISHOP) { ++result; }
        const ChessPiece b = find_slider(color, square, -1, +1);
        if (b.get_type() == QUEEN || b.get_type() == BISHOP) { ++result; }
        const ChessPiece c = find_slider(color, square, +1, -1);
        if (c.get_type() == QUEEN || c.get_type() == BISHOP) { ++result; }
        const ChessPiece d = find_slider(color, square, +1, +1);
        if (d.get_type() == QUEEN || d.get_type() == BISHOP) { ++result; }
        return result;
    }

public: // ====================================================== ATTACK TESTING

    [[nodiscard]] constexpr bool
    is_attacked_by(PieceColor color, ChessSquare square) const noexcept {
        assert(color != PieceColor::NONE);
        assert(square.in_bounds());
        // TODO: optimal ordering of branches
        return is_attacked_by_king(color, square) ||
               is_attacked_orthogonally(color, square) ||
               is_attacked_diagonally(color, square) ||
               is_attacked_by_knight(color, square) ||
               is_attacked_by_pawn(color, square);
    }

    [[nodiscard]] constexpr int
    count_attacks_by(PieceColor color, ChessSquare square) const noexcept {
        assert(color != PieceColor::NONE);
        assert(square.in_bounds());
        return count_king_attacks(color, square) +
               count_orthogonal_attacks(color, square) +
               count_diagonal_attacks(color, square) +
               count_knight_attacks(color, square) +
               count_pawn_attacks(color, square);
    }

}; // class ChessBoard


std::ostream &operator<<(std::ostream &os, const ChessBoard &board);


#endif // SUCKER_CHESS_CHESS_BOARD_HPP
