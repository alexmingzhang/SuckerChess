#ifndef SUCKER_CHESS_CHESS_POSITION_HPP
#define SUCKER_CHESS_CHESS_POSITION_HPP

#include <array>   // for std::array
#include <ostream> // for std::ostream
#include <string>  // for std::string
#include <vector>  // for std::vector

#include "ChessMove.hpp"
#include "ChessPiece.hpp"


class ChessPosition {

    std::array<std::array<ChessPiece, NUM_RANKS>, NUM_FILES> board;
    PieceColor to_move;
    coord_t en_passant_file;
    bool white_can_short_castle;
    bool white_can_long_castle;
    bool black_can_short_castle;
    bool black_can_long_castle;

public: // ======================================================== CONSTRUCTORS

    explicit constexpr ChessPosition() noexcept
        : board{{{WHITE_ROOK, WHITE_PAWN, EMPTY_SQUARE, EMPTY_SQUARE,
                EMPTY_SQUARE, EMPTY_SQUARE, BLACK_PAWN, BLACK_ROOK},
               {WHITE_KNIGHT, WHITE_PAWN, EMPTY_SQUARE, EMPTY_SQUARE,
                EMPTY_SQUARE, EMPTY_SQUARE, BLACK_PAWN, BLACK_KNIGHT},
               {WHITE_BISHOP, WHITE_PAWN, EMPTY_SQUARE, EMPTY_SQUARE,
                EMPTY_SQUARE, EMPTY_SQUARE, BLACK_PAWN, BLACK_BISHOP},
               {WHITE_QUEEN, WHITE_PAWN, EMPTY_SQUARE, EMPTY_SQUARE,
                EMPTY_SQUARE, EMPTY_SQUARE, BLACK_PAWN, BLACK_QUEEN},
               {WHITE_KING, WHITE_PAWN, EMPTY_SQUARE, EMPTY_SQUARE,
                EMPTY_SQUARE, EMPTY_SQUARE, BLACK_PAWN, BLACK_KING},
               {WHITE_BISHOP, WHITE_PAWN, EMPTY_SQUARE, EMPTY_SQUARE,
                EMPTY_SQUARE, EMPTY_SQUARE, BLACK_PAWN, BLACK_BISHOP},
               {WHITE_KNIGHT, WHITE_PAWN, EMPTY_SQUARE, EMPTY_SQUARE,
                EMPTY_SQUARE, EMPTY_SQUARE, BLACK_PAWN, BLACK_KNIGHT},
               {WHITE_ROOK, WHITE_PAWN, EMPTY_SQUARE, EMPTY_SQUARE,
                EMPTY_SQUARE, EMPTY_SQUARE, BLACK_PAWN, BLACK_ROOK}}},
          to_move(PieceColor::WHITE), en_passant_file(NUM_FILES),
          white_can_short_castle(true), white_can_long_castle(true),
          black_can_short_castle(true), black_can_long_castle(true) {}

public: // =========================================================== ACCESSORS

    [[nodiscard]] constexpr PieceColor get_color_to_move() const noexcept {
        return to_move;
    }

public: // ====================================================== INDEX OPERATOR

    constexpr ChessPiece &operator()(coord_t file, coord_t rank) {
        assert(in_bounds(file, rank));
        return board[file][rank];
    }

    constexpr const ChessPiece &operator()(coord_t file, coord_t rank) const {
        assert(in_bounds(file, rank));
        return board[file][rank];
    }

public: // ====================================================== SQUARE TESTING

    [[nodiscard]] constexpr bool
    is_empty(coord_t file, coord_t rank) const noexcept {
        return in_bounds(file, rank) && board[file][rank] == EMPTY_SQUARE;
    }

    [[nodiscard]] constexpr bool
    is_legal_dst(coord_t file, coord_t rank) const noexcept {
        return in_bounds(file, rank) && board[file][rank].color != to_move;
    }

    [[nodiscard]] constexpr bool
    is_legal_cap(coord_t file, coord_t rank) const noexcept {
        if (!in_bounds(file, rank)) { return false; }
        const ChessPiece piece = board[file][rank];
        return piece.color != to_move && piece.color != PieceColor::NONE;
    }

    [[nodiscard]] constexpr bool contains_enemy_piece(
        coord_t file, coord_t rank, PieceType type
    ) const noexcept {
        if (!in_bounds(file, rank)) { return false; }
        const ChessPiece piece = board[file][rank];
        return piece.color != to_move && piece.color != PieceColor::NONE &&
               piece.type == type;
    }

public: // =========================================================== UTILITIES

    [[nodiscard]] constexpr coord_t pawn_direction() const {
        switch (to_move) {
            case PieceColor::NONE: __builtin_unreachable();
            case PieceColor::WHITE: return +1;
            case PieceColor::BLACK: return -1;
        }
        __builtin_unreachable();
    }

    [[nodiscard]] constexpr coord_t promotion_rank() const {
        switch (to_move) {
            case PieceColor::NONE: __builtin_unreachable();
            case PieceColor::WHITE: return NUM_RANKS - 1;
            case PieceColor::BLACK: return 0;
        }
        __builtin_unreachable();
    }

    [[nodiscard]] constexpr coord_t en_passant_rank() const {
        switch (to_move) {
            case PieceColor::NONE: __builtin_unreachable();
            case PieceColor::WHITE: return NUM_RANKS - 3;
            case PieceColor::BLACK: return 2;
        }
        __builtin_unreachable();
    }

public: // ====================================================== ATTACK TESTING

    [[nodiscard]] constexpr ChessPiece get_slider_attacker(
        coord_t file, coord_t rank, coord_t file_offset, coord_t rank_offset
    ) const {
        coord_t cur_file = file + file_offset;
        coord_t cur_rank = rank + rank_offset;
        while (is_empty(cur_file, cur_rank)) {
            cur_file += file_offset;
            cur_rank += rank_offset;
        }
        if (in_bounds(cur_file, cur_rank)) {
            const ChessPiece piece = board[cur_file][cur_rank];
            if (piece.color != to_move) { return piece; }
        }
        return EMPTY_SQUARE;
    }

    [[nodiscard]] constexpr bool
    is_attacked_by_king(coord_t file, coord_t rank) const {
        return contains_enemy_piece(file - 1, rank - 1, PieceType::KING) ||
               contains_enemy_piece(file - 1, rank, PieceType::KING) ||
               contains_enemy_piece(file - 1, rank + 1, PieceType::KING) ||
               contains_enemy_piece(file, rank - 1, PieceType::KING) ||
               contains_enemy_piece(file, rank + 1, PieceType::KING) ||
               contains_enemy_piece(file + 1, rank - 1, PieceType::KING) ||
               contains_enemy_piece(file + 1, rank, PieceType::KING) ||
               contains_enemy_piece(file + 1, rank + 1, PieceType::KING);
    }

    [[nodiscard]] constexpr bool
    is_attacked_orthogonally(coord_t file, coord_t rank) const {
        const ChessPiece a = get_slider_attacker(file, rank, -1, 0);
        if (a.type == PieceType::QUEEN || a.type == PieceType::ROOK) {
            return true;
        }
        const ChessPiece b = get_slider_attacker(file, rank, 0, -1);
        if (b.type == PieceType::QUEEN || b.type == PieceType::ROOK) {
            return true;
        }
        const ChessPiece c = get_slider_attacker(file, rank, 0, +1);
        if (c.type == PieceType::QUEEN || c.type == PieceType::ROOK) {
            return true;
        }
        const ChessPiece d = get_slider_attacker(file, rank, +1, 0);
        if (d.type == PieceType::QUEEN || d.type == PieceType::ROOK) {
            return true;
        }
        return false;
    }

    [[nodiscard]] constexpr bool
    is_attacked_diagonally(coord_t file, coord_t rank) const {
        const ChessPiece a = get_slider_attacker(file, rank, +1, +1);
        if (a.type == PieceType::QUEEN || a.type == PieceType::BISHOP) {
            return true;
        }
        const ChessPiece b = get_slider_attacker(file, rank, +1, -1);
        if (b.type == PieceType::QUEEN || b.type == PieceType::BISHOP) {
            return true;
        }
        const ChessPiece c = get_slider_attacker(file, rank, -1, +1);
        if (c.type == PieceType::QUEEN || c.type == PieceType::BISHOP) {
            return true;
        }
        const ChessPiece d = get_slider_attacker(file, rank, -1, -1);
        if (d.type == PieceType::QUEEN || d.type == PieceType::BISHOP) {
            return true;
        }
        return false;
    }

    [[nodiscard]] constexpr bool
    is_attacked_by_knight(coord_t file, coord_t rank) const {
        return contains_enemy_piece(file - 2, rank - 1, PieceType::KNIGHT) ||
               contains_enemy_piece(file - 2, rank + 1, PieceType::KNIGHT) ||
               contains_enemy_piece(file - 1, rank - 2, PieceType::KNIGHT) ||
               contains_enemy_piece(file - 1, rank + 2, PieceType::KNIGHT) ||
               contains_enemy_piece(file + 1, rank - 2, PieceType::KNIGHT) ||
               contains_enemy_piece(file + 1, rank + 2, PieceType::KNIGHT) ||
               contains_enemy_piece(file + 2, rank - 1, PieceType::KNIGHT) ||
               contains_enemy_piece(file + 2, rank + 1, PieceType::KNIGHT);
    }

    [[nodiscard]] constexpr bool
    is_attacked_by_pawn(coord_t file, coord_t rank) const {
        const coord_t direction = pawn_direction();
        return contains_enemy_piece(
                   file - 1, rank + direction, PieceType::PAWN
               ) ||
               contains_enemy_piece(
                   file + 1, rank + direction, PieceType::PAWN
               );
    }

    [[nodiscard]] constexpr bool is_attacked(coord_t file, coord_t rank) const {
        assert(in_bounds(file, rank));
        return is_attacked_by_king(file, rank) ||
               is_attacked_orthogonally(file, rank) ||
               is_attacked_diagonally(file, rank) ||
               is_attacked_by_knight(file, rank) ||
               is_attacked_by_pawn(file, rank);
    }

    [[nodiscard]] constexpr bool in_check() const {
        for (coord_t file = 0; file < NUM_FILES; ++file) {
            for (coord_t rank = 0; rank < NUM_RANKS; ++rank) {
                const ChessPiece &piece = board[file][rank];
                if (piece.type == PieceType::KING && piece.color == to_move) {
                    return is_attacked(file, rank);
                }
            }
        }
        return false;
    }

public: // ===================================================== MOVE GENERATION

    void push_leaper_move(
        std::vector<ChessMove> &moves, coord_t src_file, coord_t src_rank,
        coord_t file_offset, coord_t rank_offset
    ) const;

    void push_slider_moves(
        std::vector<ChessMove> &moves, coord_t src_file, coord_t src_rank,
        coord_t file_offset, coord_t rank_offset
    ) const;

    void push_promotion_moves(
        std::vector<ChessMove> &moves, coord_t src_file, coord_t src_rank,
        coord_t dst_file, coord_t dst_rank
    ) const;

    void push_pawn_moves(
        std::vector<ChessMove> &moves, coord_t src_file, coord_t src_rank
    ) const;

    void push_castling_moves(std::vector<ChessMove> &moves) const;

    void push_all_moves(
        std::vector<ChessMove> &moves, coord_t src_file, coord_t src_rank
    ) const;

    [[nodiscard]] std::vector<ChessMove> get_all_moves() const;

public: // ====================================================== MOVE EXECUTION

    void make_move(const ChessMove &move);

public: // ======================================================= CHECK TESTING

    [[nodiscard]] bool puts_self_in_check(const ChessMove &move) const {
        ChessPosition copy = *this;
        copy.make_move(move);
        copy.to_move = to_move;
        return copy.in_check();
    }

    [[nodiscard]] std::vector<ChessMove> get_legal_moves() const {
        const std::vector<ChessMove> all_moves = get_all_moves();
        std::vector<ChessMove> result;
        for (const ChessMove &move : all_moves) {
            if (!puts_self_in_check(move)) { result.push_back(move); }
        }
        return result;
    }

    [[nodiscard]] bool checkmated() const {
        return in_check() && get_legal_moves().empty();
    }

    [[nodiscard]] bool stalemated() const {
        return in_check() && get_legal_moves().empty();
    }

public: // ============================================================ PRINTING

    [[nodiscard]] std::string get_move_name(
        const std::vector<ChessMove> &legal_moves, const ChessMove &move
    ) const;

    friend std::ostream &operator<<(std::ostream &os, const ChessPosition &b);

}; // class ChessPosition


#endif // SUCKER_CHESS_CHESS_POSITION_HPP
