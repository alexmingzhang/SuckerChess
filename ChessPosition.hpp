#ifndef SUCKER_CHESS_CHESS_POSITION_HPP
#define SUCKER_CHESS_CHESS_POSITION_HPP

#include <array>   // for std::array
#include <ostream> // for std::ostream
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

public:

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

    constexpr ChessPiece &operator()(coord_t file, coord_t rank) {
        assert(in_bounds(file, rank));
        return board[file][rank];
    }

    constexpr const ChessPiece &operator()(coord_t file, coord_t rank) const {
        assert(in_bounds(file, rank));
        return board[file][rank];
    }

    [[nodiscard]] constexpr bool is_empty(coord_t file, coord_t rank) const {
        return in_bounds(file, rank) && board[file][rank] == EMPTY_SQUARE;
    }

    [[nodiscard]] constexpr bool
    is_legal_dst(coord_t file, coord_t rank) const {
        return in_bounds(file, rank) && board[file][rank].color != to_move;
    }

    [[nodiscard]] constexpr bool
    is_legal_cap(coord_t file, coord_t rank) const {
        if (!in_bounds(file, rank)) { return false; }
        const ChessPiece piece = board[file][rank];
        return piece.color != to_move && piece.color != PieceColor::NONE;
    }

    [[nodiscard]] constexpr bool
    contains_piece(coord_t file, coord_t rank, PieceType type) const {
        return in_bounds(file, rank) && board[file][rank].type == type;
    }

    [[nodiscard]] constexpr bool
    is_attacked_by_knight(coord_t file, coord_t rank) const {
        return contains_piece(file - 2, rank - 1, PieceType::KNIGHT) ||
               contains_piece(file - 2, rank + 1, PieceType::KNIGHT) ||
               contains_piece(file - 1, rank - 2, PieceType::KNIGHT) ||
               contains_piece(file - 1, rank + 2, PieceType::KNIGHT) ||
               contains_piece(file + 1, rank - 2, PieceType::KNIGHT) ||
               contains_piece(file + 1, rank + 2, PieceType::KNIGHT) ||
               contains_piece(file + 2, rank - 1, PieceType::KNIGHT) ||
               contains_piece(file + 2, rank + 1, PieceType::KNIGHT);
    }

    [[nodiscard]] bool is_attacked(coord_t file, coord_t rank) const {}

    void push_leaper_move(
        std::vector<ChessMove> &moves, coord_t src_file, coord_t src_rank,
        coord_t file_offset, coord_t rank_offset
    ) const {
        const coord_t dst_file = src_file + file_offset;
        const coord_t dst_rank = src_rank + rank_offset;
        if (is_legal_dst(dst_file, dst_rank)) {
            moves.emplace_back(src_file, src_rank, dst_file, dst_rank);
        }
    }

    void push_slider_moves(
        std::vector<ChessMove> &moves, coord_t src_file, coord_t src_rank,
        coord_t file_offset, coord_t rank_offset
    ) const {
        coord_t dst_file = src_file + file_offset;
        coord_t dst_rank = src_rank + rank_offset;
        while (is_empty(dst_file, dst_rank)) {
            moves.emplace_back(src_file, src_rank, dst_file, dst_rank);
            dst_file += file_offset;
            dst_rank += rank_offset;
        }
        if (is_legal_dst(dst_file, dst_rank)) {
            moves.emplace_back(src_file, src_rank, dst_file, dst_rank);
        }
    }

    void push_pawn_moves(
        std::vector<ChessMove> &moves, coord_t src_file, coord_t src_rank
    ) const {

        const coord_t direction = (to_move == PieceColor::WHITE) ? +1 : -1;
        const coord_t en_passant_rank =
            (to_move == PieceColor::WHITE) ? NUM_RANKS - 3 : 2;

        coord_t dst_file = src_file;
        coord_t dst_rank = src_rank + direction;

        auto push_move_with_promotes = [&] {
            if (dst_rank == 0 || dst_rank == NUM_RANKS - 1) {
                moves.emplace_back(
                    src_file, src_rank, dst_file, dst_rank, PieceType::QUEEN
                );
                moves.emplace_back(
                    src_file, src_rank, dst_file, dst_rank, PieceType::ROOK
                );
                moves.emplace_back(
                    src_file, src_rank, dst_file, dst_rank, PieceType::BISHOP
                );
                moves.emplace_back(
                    src_file, src_rank, dst_file, dst_rank, PieceType::KNIGHT
                );
            } else {
                moves.emplace_back(src_file, src_rank, dst_file, dst_rank);
            }
        };

        // non-capturing
        if (is_empty(dst_file, dst_rank)) { push_move_with_promotes(); }

        // non-capturing double move
        dst_rank = src_rank + direction * 2;
        if (((to_move == PieceColor::WHITE && src_rank == 1) ||
             (to_move == PieceColor::BLACK && src_rank == NUM_RANKS - 2)) &&
            is_empty(dst_file, dst_rank - direction) &&
            is_empty(dst_file, dst_rank)) {
            moves.emplace_back(src_file, src_rank, dst_file, dst_rank);
        }

        // capture to src_file - 1
        dst_rank = src_rank + direction;
        dst_file = src_file - 1;
        if (is_legal_cap(dst_file, dst_rank)) { push_move_with_promotes(); }

        // en-passant
        if (in_bounds(dst_file, dst_rank) &&
            (dst_file == en_passant_file && dst_rank == en_passant_rank)) {
            moves.emplace_back(src_file, src_rank, dst_file, dst_rank);
        }

        // capture to src_file + 1
        dst_file = src_file + 1;
        if (is_legal_cap(dst_file, dst_rank)) { push_move_with_promotes(); }

        // en-passant
        if (in_bounds(dst_file, dst_rank) &&
            (dst_file == en_passant_file && dst_rank == en_passant_rank)) {
            moves.emplace_back(src_file, src_rank, dst_file, dst_rank);
        }
    }

    void push_castling_moves(
        std::vector<ChessMove> &moves, coord_t src_file, coord_t src_rank
    ) {}

    void push_moves(
        std::vector<ChessMove> &moves, coord_t src_file, coord_t src_rank
    ) {
        assert(in_bounds(src_file, src_rank));
        const ChessPiece piece = board[src_file][src_rank];
        assert(piece.color == to_move);
        assert(piece.type != PieceType::NONE);
        switch (piece.type) {
            case PieceType::NONE: __builtin_unreachable();
            case PieceType::KING:
                push_leaper_move(moves, src_file, src_rank, -1, -1);
                push_leaper_move(moves, src_file, src_rank, -1, 0);
                push_leaper_move(moves, src_file, src_rank, -1, +1);
                push_leaper_move(moves, src_file, src_rank, 0, -1);
                push_leaper_move(moves, src_file, src_rank, 0, +1);
                push_leaper_move(moves, src_file, src_rank, +1, -1);
                push_leaper_move(moves, src_file, src_rank, +1, 0);
                push_leaper_move(moves, src_file, src_rank, +1, +1);
                break;
            case PieceType::QUEEN:
                push_slider_moves(moves, src_file, src_rank, -1, -1);
                push_slider_moves(moves, src_file, src_rank, -1, 0);
                push_slider_moves(moves, src_file, src_rank, -1, +1);
                push_slider_moves(moves, src_file, src_rank, 0, -1);
                push_slider_moves(moves, src_file, src_rank, 0, +1);
                push_slider_moves(moves, src_file, src_rank, +1, -1);
                push_slider_moves(moves, src_file, src_rank, +1, 0);
                push_slider_moves(moves, src_file, src_rank, +1, +1);
                break;
            case PieceType::ROOK:
                push_slider_moves(moves, src_file, src_rank, -1, 0);
                push_slider_moves(moves, src_file, src_rank, 0, -1);
                push_slider_moves(moves, src_file, src_rank, 0, +1);
                push_slider_moves(moves, src_file, src_rank, +1, 0);
                break;
            case PieceType::BISHOP:
                push_slider_moves(moves, src_file, src_rank, -1, -1);
                push_slider_moves(moves, src_file, src_rank, -1, +1);
                push_slider_moves(moves, src_file, src_rank, +1, -1);
                push_slider_moves(moves, src_file, src_rank, +1, +1);
                break;
            case PieceType::KNIGHT:
                push_leaper_move(moves, src_file, src_rank, -2, -1);
                push_leaper_move(moves, src_file, src_rank, -2, +1);
                push_leaper_move(moves, src_file, src_rank, -1, -2);
                push_leaper_move(moves, src_file, src_rank, -1, +2);
                push_leaper_move(moves, src_file, src_rank, +1, -2);
                push_leaper_move(moves, src_file, src_rank, +1, +2);
                push_leaper_move(moves, src_file, src_rank, +2, -1);
                push_leaper_move(moves, src_file, src_rank, +2, +1);
                break;
            case PieceType::PAWN:
                push_pawn_moves(moves, src_file, src_rank);
                break;
        }
    }

    std::vector<ChessMove> get_moves() {
        std::vector<ChessMove> result;
        for (coord_t src_file = 0; src_file < NUM_FILES; ++src_file) {
            for (coord_t src_rank = 0; src_rank < NUM_RANKS; ++src_rank) {
                const ChessPiece &piece = board[src_file][src_rank];
                if (piece.color == to_move) {
                    push_moves(result, src_file, src_rank);
                }
            }
        }

        return result;
    }

    void make_move(const ChessMove &move);

    friend std::ostream &operator<<(std::ostream &os, const ChessPosition &b);

}; // class ChessPosition


#endif // SUCKER_CHESS_CHESS_POSITION_HPP
