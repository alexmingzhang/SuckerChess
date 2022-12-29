#ifndef SUCKER_CHESS_CHESS_BOARD_HPP
#define SUCKER_CHESS_CHESS_BOARD_HPP

#include <array>   // for std::array
#include <cassert> // for assert
#include <cstddef> // for std::size_t
#include <ostream> // for std::ostream
#include <string>  // for std::string

#include "src/ChessMove.hpp"
#include "src/ChessPiece.hpp"


class ChessBoard {

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

public: // ====================================================== INDEX OPERATOR

    constexpr ChessPiece operator[](ChessSquare square) const noexcept {
        assert(square.in_bounds());
        return data[static_cast<std::size_t>(square.file)]
                   [static_cast<std::size_t>(square.rank)];
    }

    constexpr ChessPiece &operator[](ChessSquare square) noexcept {
        assert(square.in_bounds());
        return data[static_cast<std::size_t>(square.file)]
                   [static_cast<std::size_t>(square.rank)];
    }

    constexpr ChessPiece operator()(coord_t file, coord_t rank) const noexcept {
        return (*this)[{file, rank}];
    }

    constexpr ChessPiece &operator()(coord_t file, coord_t rank) noexcept {
        return (*this)[{file, rank}];
    }

public: // ========================================================== COMPARISON

    constexpr bool operator==(const ChessBoard &) const noexcept = default;

public: // ======================================================= STATE TESTING

    [[nodiscard]] constexpr bool is_empty(ChessSquare square) const noexcept {
        return square.in_bounds() && ((*this)[square] == EMPTY_SQUARE);
    }

    [[nodiscard]] constexpr bool
    has_piece(ChessSquare square, ChessPiece piece) const noexcept {
        return square.in_bounds() && ((*this)[square] == piece);
    }

    [[nodiscard]] ChessSquare find_unique_piece(ChessPiece piece) const;

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
        return has_piece(square + ChessOffset{-1, -1}, king) ||
               has_piece(square + ChessOffset{-1, 0}, king) ||
               has_piece(square + ChessOffset{-1, +1}, king) ||
               has_piece(square + ChessOffset{0, -1}, king) ||
               has_piece(square + ChessOffset{0, +1}, king) ||
               has_piece(square + ChessOffset{+1, -1}, king) ||
               has_piece(square + ChessOffset{+1, 0}, king) ||
               has_piece(square + ChessOffset{+1, +1}, king);
    }

    [[nodiscard]] constexpr int
    count_king_attacks(PieceColor color, ChessSquare square) const noexcept {
        const ChessPiece king = {color, PieceType::KING};
        int result = 0;
        if (has_piece(square + ChessOffset{-1, -1}, king)) { ++result; }
        if (has_piece(square + ChessOffset{-1, 0}, king)) { ++result; }
        if (has_piece(square + ChessOffset{-1, +1}, king)) { ++result; }
        if (has_piece(square + ChessOffset{0, -1}, king)) { ++result; }
        if (has_piece(square + ChessOffset{0, +1}, king)) { ++result; }
        if (has_piece(square + ChessOffset{+1, -1}, king)) { ++result; }
        if (has_piece(square + ChessOffset{+1, 0}, king)) { ++result; }
        if (has_piece(square + ChessOffset{+1, +1}, king)) { ++result; }
        return result;
    }

    [[nodiscard]] constexpr bool
    is_attacked_by_knight(PieceColor color, ChessSquare square) const noexcept {
        const ChessPiece knight = {color, PieceType::KNIGHT};
        return has_piece(square + ChessOffset{-2, -1}, knight) ||
               has_piece(square + ChessOffset{-2, +1}, knight) ||
               has_piece(square + ChessOffset{-1, -2}, knight) ||
               has_piece(square + ChessOffset{-1, +2}, knight) ||
               has_piece(square + ChessOffset{+1, -2}, knight) ||
               has_piece(square + ChessOffset{+1, +2}, knight) ||
               has_piece(square + ChessOffset{+2, -1}, knight) ||
               has_piece(square + ChessOffset{+2, +1}, knight);
    }

    [[nodiscard]] constexpr int
    count_knight_attacks(PieceColor color, ChessSquare square) const noexcept {
        const ChessPiece knight = {color, PieceType::KNIGHT};
        int result = 0;
        if (has_piece(square + ChessOffset{-2, -1}, knight)) { ++result; }
        if (has_piece(square + ChessOffset{-2, +1}, knight)) { ++result; }
        if (has_piece(square + ChessOffset{-1, -2}, knight)) { ++result; }
        if (has_piece(square + ChessOffset{-1, +2}, knight)) { ++result; }
        if (has_piece(square + ChessOffset{+1, -2}, knight)) { ++result; }
        if (has_piece(square + ChessOffset{+1, +2}, knight)) { ++result; }
        if (has_piece(square + ChessOffset{+2, -1}, knight)) { ++result; }
        if (has_piece(square + ChessOffset{+2, +1}, knight)) { ++result; }
        return result;
    }

    [[nodiscard]] constexpr bool
    is_attacked_by_pawn(PieceColor color, ChessSquare square) const noexcept {
        const ChessPiece pawn = {color, PieceType::PAWN};
        const coord_t direction = pawn_direction(color);
        return has_piece(square + ChessOffset{-1, -direction}, pawn) ||
               has_piece(square + ChessOffset{+1, -direction}, pawn);
    }

    [[nodiscard]] constexpr bool
    count_pawn_attacks(PieceColor color, ChessSquare square) const noexcept {
        const ChessPiece pawn = {color, PieceType::PAWN};
        const coord_t direction = pawn_direction(color);
        return has_piece(square + ChessOffset{-1, -direction}, pawn) ||
               has_piece(square + ChessOffset{+1, -direction}, pawn);
    }

private: // ============================================== SLIDER ATTACK HELPERS

    [[nodiscard]] constexpr ChessPiece find_slider(
        PieceColor color, ChessSquare square, ChessOffset offset
    ) const noexcept {
        ChessSquare current = square + offset;
        while (is_empty(current)) { current += offset; }
        if (current.in_bounds()) {
            const ChessPiece piece = (*this)[current];
            if (piece.get_color() == color) { return piece; }
        }
        return EMPTY_SQUARE;
    }

private: // ===================================================== SLIDER ATTACKS

    [[nodiscard]] constexpr bool is_attacked_orthogonally(
        PieceColor color, ChessSquare square
    ) const noexcept {
        using enum PieceType;
        const ChessPiece a = find_slider(color, square, {-1, 0});
        if (a.get_type() == QUEEN || a.get_type() == ROOK) { return true; }
        const ChessPiece b = find_slider(color, square, {0, -1});
        if (b.get_type() == QUEEN || b.get_type() == ROOK) { return true; }
        const ChessPiece c = find_slider(color, square, {0, +1});
        if (c.get_type() == QUEEN || c.get_type() == ROOK) { return true; }
        const ChessPiece d = find_slider(color, square, {+1, 0});
        if (d.get_type() == QUEEN || d.get_type() == ROOK) { return true; }
        return false;
    }

    [[nodiscard]] constexpr int count_orthogonal_attacks(
        PieceColor color, ChessSquare square
    ) const noexcept {
        using enum PieceType;
        int result = 0;
        const ChessPiece a = find_slider(color, square, {-1, 0});
        if (a.get_type() == QUEEN || a.get_type() == ROOK) { ++result; }
        const ChessPiece b = find_slider(color, square, {0, -1});
        if (b.get_type() == QUEEN || b.get_type() == ROOK) { ++result; }
        const ChessPiece c = find_slider(color, square, {0, +1});
        if (c.get_type() == QUEEN || c.get_type() == ROOK) { ++result; }
        const ChessPiece d = find_slider(color, square, {+1, 0});
        if (d.get_type() == QUEEN || d.get_type() == ROOK) { ++result; }
        return result;
    }

    [[nodiscard]] constexpr bool is_attacked_diagonally(
        PieceColor color, ChessSquare square
    ) const noexcept {
        using enum PieceType;
        const ChessPiece a = find_slider(color, square, {-1, -1});
        if (a.get_type() == QUEEN || a.get_type() == BISHOP) { return true; }
        const ChessPiece b = find_slider(color, square, {-1, +1});
        if (b.get_type() == QUEEN || b.get_type() == BISHOP) { return true; }
        const ChessPiece c = find_slider(color, square, {+1, -1});
        if (c.get_type() == QUEEN || c.get_type() == BISHOP) { return true; }
        const ChessPiece d = find_slider(color, square, {+1, +1});
        if (d.get_type() == QUEEN || d.get_type() == BISHOP) { return true; }
        return false;
    }

    [[nodiscard]] constexpr int count_diagonal_attacks(
        PieceColor color, ChessSquare square
    ) const noexcept {
        using enum PieceType;
        int result = 0;
        const ChessPiece a = find_slider(color, square, {-1, -1});
        if (a.get_type() == QUEEN || a.get_type() == BISHOP) { ++result; }
        const ChessPiece b = find_slider(color, square, {-1, +1});
        if (b.get_type() == QUEEN || b.get_type() == BISHOP) { ++result; }
        const ChessPiece c = find_slider(color, square, {+1, -1});
        if (c.get_type() == QUEEN || c.get_type() == BISHOP) { ++result; }
        const ChessPiece d = find_slider(color, square, {+1, +1});
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
