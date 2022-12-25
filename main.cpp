#include <array>
#include <cstdint>
#include <iostream>
#include <vector>

#include "ChessPiece.hpp"


using coord_t = std::int8_t;


constexpr coord_t NUM_FILES = 8;
constexpr coord_t NUM_RANKS = 8;


struct ChessMove {

    coord_t src_file;
    coord_t src_rank;
    coord_t dst_file;
    coord_t dst_rank;
    PieceType promotion_type;

}; // struct ChessMove


class ChessPosition {

    std::array<std::array<ChessPiece, NUM_RANKS>, NUM_FILES> board;
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
          black_can_long_castle(true), en_passant_file(NUM_FILES) {}

    ChessPiece &operator()(coord_t file, coord_t rank) {
        return board[file][rank];
    }

    const ChessPiece &operator()(coord_t file, coord_t rank) const {
        return board[file][rank];
    }

    static constexpr bool in_bounds(coord_t file, coord_t rank) {
        return (file >= 0) && (file < NUM_FILES) && (rank >= 0) &&
               (rank < NUM_RANKS);
    }

    [[nodiscard]] bool is_empty(coord_t file, coord_t rank) const {
        return in_bounds(file, rank) && board[file][rank] == EMPTY_SQUARE;
    }

    [[nodiscard]] bool is_legal_dst_can_cap(coord_t file, coord_t rank) const {
        return in_bounds(file, rank) && board[file][rank].color != to_move;
    }

    [[nodiscard]] bool is_legal_dst_no_cap(coord_t file, coord_t rank) const {
        return in_bounds(file, rank) &&
               board[file][rank].color == PieceColor::NONE;
    }

    [[nodiscard]] bool is_legal_dst_must_cap(coord_t file, coord_t rank) const {
        return in_bounds(file, rank) && board[file][rank].color != to_move &&
               board[file][rank].color != PieceColor::NONE;
    }

    void push_leaper_move(
        std::vector<ChessMove> &moves, coord_t src_file, coord_t src_rank,
        coord_t file_offset, coord_t rank_offset
    ) const {
        const auto dst_file = static_cast<coord_t>(src_file + file_offset);
        const auto dst_rank = static_cast<coord_t>(src_rank + rank_offset);
        if (is_legal_dst_can_cap(dst_file, dst_rank)) {
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
        if (is_legal_dst_can_cap(dst_file, dst_rank)) {
            moves.push_back(
                {src_file, src_rank, dst_file, dst_rank, PieceType::NONE}
            );
        }
    }

    void push_pawn_moves(
        std::vector<ChessMove> &moves, coord_t src_file, coord_t src_rank
    ) {

        const coord_t direction = (to_move == PieceColor::WHITE) ? +1 : -1;
        const coord_t en_passant_rank =
            (to_move == PieceColor::WHITE) ? NUM_RANKS - 3 : 2;

        coord_t dst_file = src_file;
        coord_t dst_rank = src_rank + direction;

        auto push_move_with_promotes = [&] {
            if (dst_rank == 0 || dst_rank == NUM_RANKS - 1) {
                moves.push_back(
                    {src_file, src_rank, dst_file, dst_rank, PieceType::KNIGHT}
                );
                moves.push_back(
                    {src_file, src_rank, dst_file, dst_rank, PieceType::BISHOP}
                );
                moves.push_back(
                    {src_file, src_rank, dst_file, dst_rank, PieceType::ROOK}
                );
                moves.push_back(
                    {src_file, src_rank, dst_file, dst_rank, PieceType::QUEEN}
                );
            } else {
                moves.push_back(
                    {src_file, src_rank, dst_file, dst_rank, PieceType::NONE}
                );
            }
        };

        // non-capturing
        if (is_legal_dst_no_cap(dst_file, dst_rank)) {
            push_move_with_promotes();
        }

        // non-capturing double move
        dst_rank = src_rank + direction * 2;
        if (((to_move == PieceColor::WHITE && src_rank == 1) ||
             (to_move == PieceColor::BLACK && src_rank == NUM_RANKS - 2)) &&
            is_legal_dst_no_cap(dst_file, dst_rank - direction) &&
            is_legal_dst_no_cap(dst_file, dst_rank)) {
            moves.push_back(
                {src_file, src_rank, dst_file, dst_rank, PieceType::NONE}
            );
        }

        // capture to src_file - 1
        dst_rank = src_rank + direction;
        dst_file = src_file - 1;
        if (is_legal_dst_must_cap(dst_file, dst_rank)) {
            push_move_with_promotes();
        }

        // en-passant
        if (is_legal_dst_no_cap(dst_file, dst_rank) &&
            (dst_file == en_passant_file && dst_rank == en_passant_rank)) {
            moves.push_back(
                {src_file, src_rank, dst_file, dst_rank, PieceType::NONE}
            );
            board[dst_file][dst_rank - direction] = EMPTY_SQUARE;
        }

        // capture to src_file + 1
        dst_file = src_file + 1;
        if (is_legal_dst_must_cap(dst_file, dst_rank)) {
            push_move_with_promotes();
        }

        // en-passant
        if (is_legal_dst_no_cap(dst_file, dst_rank) &&
            (dst_file == en_passant_file && dst_rank == en_passant_rank)) {
            moves.push_back(
                {src_file, src_rank, dst_file, dst_rank, PieceType::NONE}
            );
            board[dst_file][dst_rank - direction] = EMPTY_SQUARE;
        }
    }

    std::vector<ChessMove> get_moves() {
        std::vector<ChessMove> result;
        for (coord_t src_file = 0; src_file < NUM_FILES; ++src_file) {
            for (coord_t src_rank = 0; src_rank < NUM_RANKS; ++src_rank) {
                const ChessPiece &piece = board[src_file][src_rank];
                if (piece.color == to_move) {
                    switch (piece.type) {
                    case PieceType::NONE:
                        __builtin_unreachable();
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
                    case PieceType::PAWN:
                        push_pawn_moves(result, src_file, src_rank);
                        break;
                    }
                }
            }
        }

        return result;
    }

    void make_move(ChessMove &move) {
        // TODO: add promotion stuff

        ChessPiece piece = board[move.src_file][move.src_rank];

        if (piece.type == PieceType::PAWN) {
            if (move.dst_rank - move.src_rank == 2) {
                en_passant_file = move.dst_file;
            } else if (move.dst_rank - move.src_rank == -2) {
                en_passant_file = move.dst_file;
            }
        }

        board[move.dst_file][move.dst_rank] =
            board[move.src_file][move.src_rank];
        board[move.src_file][move.src_rank] = EMPTY_SQUARE;

        to_move = (to_move == PieceColor::WHITE) ? PieceColor::BLACK
                                                 : PieceColor::WHITE;
    }

    friend std::ostream &operator<<(std::ostream &os, const ChessPosition &b) {
        std::cout << "   a  b  c  d  e  f  g  h\n";
        for (coord_t rank = NUM_RANKS - 1; rank >= 0; --rank) {
            os << rank + 1 << " |";
            for (coord_t file = 0; file < NUM_FILES; ++file) {
                os << b.board[file][rank] << '|';
            }
            os << "\n";
        }

        return os;
    }

}; // class ChessPosition

int main() {
    constexpr char LETTERS[8] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
    ChessPosition pos;

    while (1) {
        std::cout << pos << std::endl;

        std::vector<ChessMove> moves = pos.get_moves();
        for (std::size_t i = 0; i < moves.size(); ++i) {
            const ChessMove &move = moves[i];
            std::cout << i << ": " << LETTERS[move.src_file]
                      << (int)move.src_rank + 1 << " to "
                      << LETTERS[move.dst_file] << (int)move.dst_rank + 1
                      << std::endl;
        }

        std::cout << "> ";
        std::size_t move_index;
        std::cin >> move_index;

        ChessMove move = moves[move_index];
        pos.make_move(move);

        std::cout << LETTERS[move.src_file] << (int)move.src_rank + 1 << " to "
                  << LETTERS[move.dst_file] << (int)move.dst_rank + 1
                  << std::endl;
    }
    return 0;
}
