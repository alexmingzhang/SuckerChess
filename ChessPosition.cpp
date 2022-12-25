#include "ChessPosition.hpp"

#include <cassert> // for assert


void ChessPosition::make_move(const ChessMove &move) {

    ChessPiece piece = board[move.src_file][move.src_rank];
    assert(piece.color != PieceColor::NONE);
    assert(piece.color == to_move);

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
    to_move =
        (to_move == PieceColor::WHITE) ? PieceColor::BLACK : PieceColor::WHITE;
}


std::ostream &operator<<(std::ostream &os, const ChessPosition &pos) {
    os << "   a  b  c  d  e  f  g  h\n";
    for (coord_t rank = NUM_RANKS - 1; rank >= 0; --rank) {
        // ensure printing as int, not char
        os << static_cast<int>(rank + 1) << " |";
        for (coord_t file = 0; file < NUM_FILES; ++file) {
            os << pos.board[file][rank] << '|';
        }
        os << "\n";
    }
    return os;
}
