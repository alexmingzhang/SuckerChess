#include "ChessPosition.hpp"

#include <cassert> // for assert
#include <cmath>   // for std::abs


void ChessPosition::make_move(const ChessMove &move) {

    // get and validate moving piece
    assert(in_bounds(move.src_file, move.src_rank));
    assert(in_bounds(move.dst_file, move.dst_rank));
    ChessPiece piece = board[move.src_file][move.src_rank];
    assert(piece.color != PieceColor::NONE);
    assert(piece.color == to_move);

    // validate move
    switch (piece.type) {
        case PieceType::NONE:
            __builtin_unreachable();
        case PieceType::KING:
            assert(move.distance() == 1 || move.distance() == 2);
            break;
        case PieceType::QUEEN:
            assert(move.is_orthogonal() || move.is_diagonal());
            break;
        case PieceType::ROOK:
            assert(move.is_orthogonal());
            break;
        case PieceType::BISHOP:
            assert(move.is_diagonal());
            break;
        case PieceType::KNIGHT:
            assert(move.distance() == 2);
            break;
        case PieceType::PAWN:
            assert(move.distance() == 1 || move.distance() == 2);
            break;
    }

    // check for en passant capture
    const bool is_en_passant_capture =
        piece.type == PieceType::PAWN && move.src_file != move.dst_file &&
        move.src_rank != move.dst_rank &&
        board[move.dst_file][move.dst_rank] == EMPTY_SQUARE;

    // record en passant file
    if (piece.type == PieceType::PAWN &&
        (move.src_rank - move.dst_rank == 2 ||
         move.src_rank - move.dst_rank == -2)) {
        assert(move.src_file == move.dst_file);
        en_passant_file = move.src_file;
    } else {
        en_passant_file = NUM_FILES;
    }

    // update castling rights
    if (piece == WHITE_KING) {
        white_can_short_castle = false;
        white_can_long_castle = false;
    }
    if (piece == WHITE_ROOK && move.src_file == NUM_FILES - 1 &&
        move.src_rank == 0) {
        white_can_short_castle = false;
    }
    if (piece == WHITE_ROOK && move.src_file == 0 && move.src_rank == 0) {
        white_can_long_castle = false;
    }
    if (piece == BLACK_KING) {
        black_can_short_castle = false;
        black_can_long_castle = false;
    }
    if (piece == BLACK_ROOK && move.src_file == NUM_FILES - 1 &&
        move.src_rank == NUM_RANKS - 1) {
        black_can_short_castle = false;
    }
    if (piece == BLACK_ROOK && move.src_file == 0 &&
        move.src_rank == NUM_RANKS - 1) {
        black_can_long_castle = false;
    }

    // handle promotion
    if (move.promotion_type != PieceType::NONE) {
        piece.type = move.promotion_type;
    }

    // perform move
    board[move.dst_file][move.dst_rank] = piece;
    board[move.src_file][move.src_rank] = EMPTY_SQUARE;

    // handle en passant capture
    if (is_en_passant_capture) {
        board[move.dst_file][move.src_rank] = EMPTY_SQUARE;
    }

    // TODO: handle castling

    // update player to move
    if (to_move == PieceColor::WHITE) {
        to_move = PieceColor::BLACK;
    } else if (to_move == PieceColor::BLACK) {
        to_move = PieceColor::WHITE;
    } else {
        __builtin_unreachable();
    }
}


std::ostream &operator<<(std::ostream &os, const ChessPosition &pos) {
    os << "   a b c d e f g h\n";
    os << "  ┌─┬─┬─┬─┬─┬─┬─┬─┐\n";
    for (coord_t rank = NUM_RANKS - 1; rank >= 0; --rank) {
        // ensure printing as int, not char
        os << static_cast<int>(rank + 1) << " │";
        for (coord_t file = 0; file < NUM_FILES; ++file) {
            os << pos.board[file][rank] << "│";
        }
        os << "\n";
    }
    os << "  └─┴─┴─┴─┴─┴─┴─┴─┘\n";
    return os;
}
