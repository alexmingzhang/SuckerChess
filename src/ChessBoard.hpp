#ifndef SUCKER_CHESS_CHESS_BOARD_HPP
#define SUCKER_CHESS_CHESS_BOARD_HPP

#include <array>   // for std::array
#include <cassert> // for assert
#include <cstddef> // for std::size_t
#include <cstdint> // for std::uint8_t
#include <ostream> // for std::ostream
#include <string>  // for std::string

#include "ChessMove.hpp"
#include "ChessPiece.hpp"


class ChessBoard final {

#ifdef SUCKER_CHESS_USE_COMPRESSED_CHESS_BOARD
    static_assert(NUM_RANKS % 2 == 0);
    std::array<std::array<std::uint8_t, NUM_RANKS / 2>, NUM_FILES> data;
#else
    std::array<std::array<ChessPiece, NUM_RANKS>, NUM_FILES> data;
#endif

public: // ========================================================= CONSTRUCTOR

    explicit constexpr ChessBoard() noexcept {

        // begin with all squares empty
        for (coord_t file = 0; file < NUM_FILES; ++file) {
            for (coord_t rank = 0; rank < NUM_RANKS; ++rank) {
                set_piece(file, rank, EMPTY_SQUARE);
            }
        }

        // set up white pieces
        set_piece(0, 0, WHITE_ROOK);
        set_piece(1, 0, WHITE_KNIGHT);
        set_piece(2, 0, WHITE_BISHOP);
        set_piece(3, 0, WHITE_QUEEN);
        set_piece(4, 0, WHITE_KING);
        set_piece(5, 0, WHITE_BISHOP);
        set_piece(6, 0, WHITE_KNIGHT);
        set_piece(7, 0, WHITE_ROOK);

        // set up white pawns
        set_piece(0, 1, WHITE_PAWN);
        set_piece(1, 1, WHITE_PAWN);
        set_piece(2, 1, WHITE_PAWN);
        set_piece(3, 1, WHITE_PAWN);
        set_piece(4, 1, WHITE_PAWN);
        set_piece(5, 1, WHITE_PAWN);
        set_piece(6, 1, WHITE_PAWN);
        set_piece(7, 1, WHITE_PAWN);

        // set up black pieces
        set_piece(0, 7, BLACK_ROOK);
        set_piece(1, 7, BLACK_KNIGHT);
        set_piece(2, 7, BLACK_BISHOP);
        set_piece(3, 7, BLACK_QUEEN);
        set_piece(4, 7, BLACK_KING);
        set_piece(5, 7, BLACK_BISHOP);
        set_piece(6, 7, BLACK_KNIGHT);
        set_piece(7, 7, BLACK_ROOK);

        // set up black pawns
        set_piece(0, 6, BLACK_PAWN);
        set_piece(1, 6, BLACK_PAWN);
        set_piece(2, 6, BLACK_PAWN);
        set_piece(3, 6, BLACK_PAWN);
        set_piece(4, 6, BLACK_PAWN);
        set_piece(5, 6, BLACK_PAWN);
        set_piece(6, 6, BLACK_PAWN);
        set_piece(7, 6, BLACK_PAWN);
    }

    explicit ChessBoard(const std::string &fen_board_str);

private: // ================================================ COMPRESSION HELPERS

#ifdef SUCKER_CHESS_USE_COMPRESSED_CHESS_BOARD

    static constexpr ChessPiece piece_from_cell_value(std::uint8_t value
    ) noexcept {
        assert(value < 0x0F);
        assert(value != 0x07);
        assert(value != 0x08);
        if (value == 0x00) { return EMPTY_SQUARE; }
        const PieceColor color =
            (value & 0x08) ? PieceColor::BLACK : PieceColor::WHITE;
        const auto type = static_cast<PieceType>(value & 0x07);
        return {color, type};
    }

    static constexpr std::uint8_t cell_value_from_piece(ChessPiece piece
    ) noexcept {
        std::uint8_t result =
            (piece.get_color() == PieceColor::BLACK) ? 0x08 : 0x00;
        result |= static_cast<std::uint8_t>(piece.get_type());
        return result;
    }

#endif

public: // =========================================================== ACCESSORS

    [[nodiscard]] constexpr ChessPiece get_piece(ChessSquare square
    ) const noexcept {
        assert(square.in_bounds());
#ifdef SUCKER_CHESS_USE_COMPRESSED_CHESS_BOARD
        const std::uint8_t cell =
            data[static_cast<std::size_t>(square.file)]
                [static_cast<std::size_t>(square.rank / 2)];
        if (square.rank & 1) {
            return piece_from_cell_value(cell >> 4);
        } else {
            return piece_from_cell_value(cell & 0x0F);
        }
#else
        return data[static_cast<std::size_t>(square.file)]
                   [static_cast<std::size_t>(square.rank)];
#endif
    }

    [[nodiscard]] constexpr ChessPiece
    get_piece(coord_t file, coord_t rank) const noexcept {
        return get_piece({file, rank});
    }

public: // ============================================================ MUTATORS

    constexpr void set_piece(ChessSquare square, ChessPiece piece) noexcept {
        assert(square.in_bounds());
#ifdef SUCKER_CHESS_USE_COMPRESSED_CHESS_BOARD
        const auto file = static_cast<std::size_t>(square.file);
        const auto rank = static_cast<std::size_t>(square.rank);
        std::uint8_t cell = data[file][rank / 2];
        if (square.rank & 1) {
            cell &= 0x0F;
            cell |= cell_value_from_piece(piece) << 4;
        } else {
            cell &= 0xF0;
            cell |= cell_value_from_piece(piece);
        }
        data[file][rank / 2] = cell;
#else
        data[static_cast<std::size_t>(square.file)]
            [static_cast<std::size_t>(square.rank)] = piece;
#endif
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

    [[nodiscard]] constexpr bool in_bounds_and_is_valid_dst(
        PieceColor moving_color, ChessSquare square
    ) const noexcept {
        assert(moving_color != PieceColor::NONE);
        if (!square.in_bounds()) { return false; }
        const ChessPiece target = get_piece(square);
        return (target.get_color() != moving_color) &&
               (target.get_type() != PieceType::KING);
    }

    [[nodiscard]] constexpr bool in_bounds_and_is_valid_cap(
        PieceColor moving_color, ChessSquare square
    ) const noexcept {
        assert(moving_color != PieceColor::NONE);
        if (!square.in_bounds()) { return false; }
        const ChessPiece target = get_piece(square);
        const PieceColor target_color = target.get_color();
        return (target_color != moving_color) &&
               (target_color != PieceColor::NONE) &&
               (target.get_type() != PieceType::KING);
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
        assert(color != PieceColor::NONE);
        switch (color) {
            case PieceColor::NONE: __builtin_unreachable();
            case PieceColor::WHITE: return +1;
            case PieceColor::BLACK: return -1;
        }
        __builtin_unreachable();
    }

    [[nodiscard]] static constexpr coord_t pawn_origin_rank(PieceColor color
    ) noexcept {
        assert(color != PieceColor::NONE);
        switch (color) {
            case PieceColor::NONE: __builtin_unreachable();
            case PieceColor::WHITE: return 1;
            case PieceColor::BLACK: return NUM_RANKS - 2;
        }
        __builtin_unreachable();
    }

    [[nodiscard]] static constexpr coord_t promotion_rank(PieceColor color
    ) noexcept {
        assert(color != PieceColor::NONE);
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
        assert(color != PieceColor::NONE);
        assert(square.in_bounds());
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
        assert(color != PieceColor::NONE);
        assert(square.in_bounds());
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
        assert(color != PieceColor::NONE);
        assert(square.in_bounds());
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
        assert(color != PieceColor::NONE);
        assert(square.in_bounds());
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
        assert(color != PieceColor::NONE);
        assert(square.in_bounds());
        const ChessPiece pawn = {color, PieceType::PAWN};
        const coord_t direction = pawn_direction(color);
        return in_bounds_and_has_piece(square.shift(-1, -direction), pawn) ||
               in_bounds_and_has_piece(square.shift(+1, -direction), pawn);
    }

    [[nodiscard]] constexpr int
    count_pawn_attacks(PieceColor color, ChessSquare square) const noexcept {
        assert(color != PieceColor::NONE);
        assert(square.in_bounds());
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
        assert(color != PieceColor::NONE);
        assert(square.in_bounds());
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
        assert(color != PieceColor::NONE);
        assert(square.in_bounds());
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
        assert(color != PieceColor::NONE);
        assert(square.in_bounds());
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
        assert(color != PieceColor::NONE);
        assert(square.in_bounds());
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
        assert(color != PieceColor::NONE);
        assert(square.in_bounds());
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
        return is_attacked_by_pawn(color, square) ||
               is_attacked_by_knight(color, square) ||
               is_attacked_diagonally(color, square) ||
               is_attacked_orthogonally(color, square) ||
               is_attacked_by_king(color, square);
    }

    [[nodiscard]] constexpr int
    count_attacks_by(PieceColor color, ChessSquare square) const noexcept {
        assert(color != PieceColor::NONE);
        assert(square.in_bounds());
        return count_pawn_attacks(color, square) +
               count_knight_attacks(color, square) +
               count_diagonal_attacks(color, square) +
               count_orthogonal_attacks(color, square) +
               count_king_attacks(color, square);
    }

}; // class ChessBoard


std::ostream &operator<<(std::ostream &os, const ChessBoard &board);


#endif // SUCKER_CHESS_CHESS_BOARD_HPP
