#include <array>
#include <cstdint>
#include <iostream>
#include <vector>

#include "ChessPiece.hpp"


using coord_t = std::int8_t;


constexpr coord_t BOARD_WIDTH = 8;
constexpr coord_t BOARD_HEIGHT = 8;


struct ChessMove {

    coord_t src_file;
    coord_t src_rank;
    coord_t dst_file;
    coord_t dst_rank;
    PieceType promotion_type;

}; // struct ChessMove


class ChessPosition {

    std::array<std::array<ChessPiece, BOARD_HEIGHT>, BOARD_WIDTH> board;
    PieceColor to_move;
    bool white_can_short_castle;
    bool white_can_long_castle;
    bool black_can_short_castle;
    bool black_can_long_castle;
    coord_t en_passant_file;

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
          to_move(PieceColor::WHITE), white_can_short_castle(true),
          white_can_long_castle(true), black_can_short_castle(true),
          black_can_long_castle(true), en_passant_file(BOARD_WIDTH) {}

    ChessPiece &operator()(coord_t file, coord_t rank) {
        return board[file][rank];
    }

    const ChessPiece &operator()(coord_t file, coord_t rank) const {
        return board[file][rank];
    }

    static constexpr bool in_bounds(coord_t file, coord_t rank) {
        return (file >= 0) && (file < BOARD_WIDTH) && (rank >= 0) &&
               (rank < BOARD_HEIGHT);
    }

    [[nodiscard]] bool is_empty(coord_t file, coord_t rank) const {
        return in_bounds(file, rank) && board[file][rank] == EMPTY_SQUARE;
    }

    [[nodiscard]] bool is_legal_dst(coord_t file, coord_t rank) const {
        return in_bounds(file, rank) && board[file][rank].color != to_move;
    }

    void push_leaper_move(
        std::vector<ChessMove> &moves, coord_t src_file, coord_t src_rank,
        coord_t file_offset, coord_t rank_offset
    ) const {
        const auto dst_file = static_cast<coord_t>(src_file + file_offset);
        const auto dst_rank = static_cast<coord_t>(src_rank + rank_offset);
        if (is_legal_dst(dst_file, dst_rank)) {
            moves.push_back(
                {src_file, src_rank, dst_file, dst_rank, PieceType::NONE}
            );
        }
    }

    void push_slider_moves(
        std::vector<ChessMove> &moves, coord_t src_file, coord_t src_rank,
        coord_t file_offset, coord_t rank_offset
    ) const {
        auto dst_file = static_cast<coord_t>(src_file + file_offset);
        auto dst_rank = static_cast<coord_t>(src_rank + rank_offset);
        while (is_empty(dst_file, dst_rank)) {
            moves.push_back(
                {src_file, src_rank, dst_file, dst_rank, PieceType::NONE}
            );
            dst_file = static_cast<coord_t>(dst_file + file_offset);
            dst_rank = static_cast<coord_t>(dst_rank + rank_offset);
        }
        if (is_legal_dst(dst_file, dst_rank)) {
            moves.push_back(
                {src_file, src_rank, dst_file, dst_rank, PieceType::NONE}
            );
        }
    }

    void push_pawn_moves(
        std::vector<ChessMove> &moves, coord_t src_file, coord_t src_rank) const {
        const coord_t direction = (to_move == PieceColor::WHITE) ? +1 : -1;
        { // non-capturing
            const coord_t dst_file = src_file;
            const coord_t dst_rank = src_rank + direction;
            if (in_bounds(dst_file, dst_rank) && board[])
        }
    }

    std::vector<ChessMove> get_moves() const {
        std::vector<ChessMove> result;
        for (coord_t src_file = 0; src_file < BOARD_WIDTH; ++src_file) {
            for (coord_t src_rank = 0; src_rank < BOARD_HEIGHT; ++src_rank) {
                const ChessPiece piece = board[src_file][src_rank];
                if (piece.color == to_move) {
                    switch (piece.type) {
                    case PieceType::KING:
                        push_leaper_move(result, src_file, src_rank, -1, -1);
                        push_leaper_move(result, src_file, src_rank, -1, 0);
                        push_leaper_move(result, src_file, src_rank, -1, +1);
                        push_leaper_move(result, src_file, src_rank, 0, -1);
                        push_leaper_move(result, src_file, src_rank, 0, +1);
                        push_leaper_move(result, src_file, src_rank, +1, -1);
                        push_leaper_move(result, src_file, src_rank, +1, 0);
                        push_leaper_move(result, src_file, src_rank, +1, +1);
                        break;
                    case PieceType::QUEEN:
                        push_slider_moves(result, src_file, src_rank, -1, -1);
                        push_slider_moves(result, src_file, src_rank, -1, 0);
                        push_slider_moves(result, src_file, src_rank, -1, +1);
                        push_slider_moves(result, src_file, src_rank, 0, -1);
                        push_slider_moves(result, src_file, src_rank, 0, +1);
                        push_slider_moves(result, src_file, src_rank, +1, -1);
                        push_slider_moves(result, src_file, src_rank, +1, 0);
                        push_slider_moves(result, src_file, src_rank, +1, +1);
                        break;
                    case PieceType::ROOK:
                        push_slider_moves(result, src_file, src_rank, -1, 0);
                        push_slider_moves(result, src_file, src_rank, 0, -1);
                        push_slider_moves(result, src_file, src_rank, 0, +1);
                        push_slider_moves(result, src_file, src_rank, +1, 0);
                        break;
                    case PieceType::BISHOP:
                        push_slider_moves(result, src_file, src_rank, -1, -1);
                        push_slider_moves(result, src_file, src_rank, -1, +1);
                        push_slider_moves(result, src_file, src_rank, +1, -1);
                        push_slider_moves(result, src_file, src_rank, +1, +1);
                        break;
                    case PieceType::KNIGHT:
                        push_leaper_move(result, src_file, src_rank, -2, -1);
                        push_leaper_move(result, src_file, src_rank, -2, +1);
                        push_leaper_move(result, src_file, src_rank, -1, -2);
                        push_leaper_move(result, src_file, src_rank, -1, +2);
                        push_leaper_move(result, src_file, src_rank, +1, -2);
                        push_leaper_move(result, src_file, src_rank, +1, +2);
                        push_leaper_move(result, src_file, src_rank, +2, -1);
                        push_leaper_move(result, src_file, src_rank, +2, +1);
                        break;
                    }
                }
            }
        }
    }

    friend std::ostream &operator<<(std::ostream &os, const ChessPosition &b) {
        for (coord_t rank = BOARD_HEIGHT - 1; rank >= 0; --rank) {
            os << '|';
            for (coord_t file = 0; file < BOARD_WIDTH; ++file) {
                std::cout << b.board[file][rank];
                os << '|';
            }
            std::cout << "\n";
        }
    }

}; // class ChessPosition

int main() {
    ChessPosition pos;
    std::cout << pos << std::endl;
    return 0;
}
