#include "ChessPosition.hpp"

#include <cassert> // for assert
#include <cctype>  // for std::toupper
#include <cmath>   // for std::abs
#include <sstream> // for std::istringstream, std::ostringstream


void ChessPosition::push_leaper_move(
    std::vector<ChessMove> &moves, coord_t src_file, coord_t src_rank,
    coord_t file_offset, coord_t rank_offset
) const {
    const coord_t dst_file = src_file + file_offset;
    const coord_t dst_rank = src_rank + rank_offset;
    if (is_legal_dst(dst_file, dst_rank)) {
        moves.emplace_back(src_file, src_rank, dst_file, dst_rank);
    }
}


void ChessPosition::push_slider_moves(
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


void ChessPosition::push_promotion_moves(
    std::vector<ChessMove> &moves, coord_t src_file, coord_t src_rank,
    coord_t dst_file, coord_t dst_rank
) const {
    if (dst_rank == promotion_rank()) {
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
}


void ChessPosition::push_pawn_moves(
    std::vector<ChessMove> &moves, coord_t src_file, coord_t src_rank
) const {
    const coord_t direction = pawn_direction();
    coord_t dst_file = src_file;
    coord_t dst_rank = src_rank + direction;

    // non-capturing
    if (is_empty(dst_file, dst_rank)) {
        push_promotion_moves(moves, src_file, src_rank, dst_file, dst_rank);
    }

    // non-capturing double move
    dst_rank = src_rank + 2 * direction;
    if (((to_move == PieceColor::WHITE && src_rank == 1) ||
         (to_move == PieceColor::BLACK && src_rank == NUM_RANKS - 2)) &&
        is_empty(dst_file, dst_rank - direction) &&
        is_empty(dst_file, dst_rank)) {
        moves.emplace_back(src_file, src_rank, dst_file, dst_rank);
    }

    // capture to src_file - 1
    dst_rank = src_rank + direction;
    dst_file = src_file - 1;
    if (is_legal_cap(dst_file, dst_rank)) {
        push_promotion_moves(moves, src_file, src_rank, dst_file, dst_rank);
    }

    // en-passant
    if (in_bounds(dst_file, dst_rank) &&
        (dst_file == en_passant_file && dst_rank == en_passant_rank())) {
        moves.emplace_back(src_file, src_rank, dst_file, dst_rank);
    }

    // capture to src_file + 1
    dst_file = src_file + 1;
    if (is_legal_cap(dst_file, dst_rank)) {
        push_promotion_moves(moves, src_file, src_rank, dst_file, dst_rank);
    }

    // en-passant
    if (in_bounds(dst_file, dst_rank) &&
        (dst_file == en_passant_file && dst_rank == en_passant_rank())) {
        moves.emplace_back(src_file, src_rank, dst_file, dst_rank);
    }
}


void ChessPosition::push_castling_moves(std::vector<ChessMove> &moves) const {
    if (to_move == PieceColor::WHITE) {
        if (white_can_short_castle) {
            assert(board[4][0] == WHITE_KING);
            assert(board[7][0] == WHITE_ROOK);
            if (board[5][0] == EMPTY_SQUARE && board[6][0] == EMPTY_SQUARE &&
                !is_attacked(4, 0) && !is_attacked(5, 0) &&
                !is_attacked(6, 0)) {
                moves.emplace_back(4, 0, 6, 0);
            }
        }
        if (white_can_long_castle) {
            assert(board[0][0] == WHITE_ROOK);
            assert(board[4][0] == WHITE_KING);
            if (board[1][0] == EMPTY_SQUARE && board[2][0] == EMPTY_SQUARE &&
                board[3][0] == EMPTY_SQUARE && !is_attacked(2, 0) &&
                !is_attacked(3, 0) && !is_attacked(4, 0)) {
                moves.emplace_back(4, 0, 2, 0);
            }
        }
    } else if (to_move == PieceColor::BLACK) {
        if (black_can_short_castle) {
            assert(board[4][7] == BLACK_KING);
            assert(board[7][7] == BLACK_ROOK);
            if (board[5][7] == EMPTY_SQUARE && board[6][7] == EMPTY_SQUARE &&
                !is_attacked(4, 7) && !is_attacked(5, 7) &&
                !is_attacked(6, 7)) {
                moves.emplace_back(4, 7, 6, 7);
            }
        }
        if (black_can_long_castle) {
            assert(board[0][7] == BLACK_ROOK);
            assert(board[4][7] == BLACK_KING);
            if (board[1][7] == EMPTY_SQUARE && board[2][7] == EMPTY_SQUARE &&
                board[3][7] == EMPTY_SQUARE && !is_attacked(2, 7) &&
                !is_attacked(3, 7) && !is_attacked(4, 7)) {
                moves.emplace_back(4, 7, 2, 7);
            }
        }
    } else {
        __builtin_unreachable();
    }
}


void ChessPosition::push_all_moves(
    std::vector<ChessMove> &moves, coord_t src_file, coord_t src_rank
) const {
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
            push_castling_moves(moves);
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
        case PieceType::PAWN: push_pawn_moves(moves, src_file, src_rank); break;
    }
}


std::vector<ChessMove> ChessPosition::get_all_moves() const {
    std::vector<ChessMove> result;
    for (coord_t src_file = 0; src_file < NUM_FILES; ++src_file) {
        for (coord_t src_rank = 0; src_rank < NUM_RANKS; ++src_rank) {
            const ChessPiece &piece = board[src_file][src_rank];
            if (piece.color == to_move) {
                push_all_moves(result, src_file, src_rank);
            }
        }
    }
    return result;
}


void ChessPosition::make_move(const ChessMove &move) {

    // get and validate moving piece
    assert(in_bounds(move.src_file, move.src_rank));
    assert(in_bounds(move.dst_file, move.dst_rank));
    ChessPiece piece = board[move.src_file][move.src_rank];
    assert(piece.color != PieceColor::NONE);
    assert(piece.color == to_move);

    // validate move
    switch (piece.type) {
        case PieceType::NONE: __builtin_unreachable();
        case PieceType::KING:
            assert(move.distance() == 1 || move.distance() == 2);
            break;
        case PieceType::QUEEN:
            assert(move.is_orthogonal() || move.is_diagonal());
            break;
        case PieceType::ROOK: assert(move.is_orthogonal()); break;
        case PieceType::BISHOP: assert(move.is_diagonal()); break;
        case PieceType::KNIGHT: assert(move.distance() == 2); break;
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
    if (move.dst_file == NUM_FILES - 1 && move.dst_rank == 0) {
        white_can_short_castle = false;
    }
    if (piece == WHITE_ROOK && move.src_file == 0 && move.src_rank == 0) {
        white_can_long_castle = false;
    }
    if (move.dst_file == 0 && move.dst_rank == 0) {
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
    if (move.dst_file == NUM_FILES - 1 && move.dst_rank == NUM_RANKS - 1) {
        black_can_short_castle = false;
    }
    if (piece == BLACK_ROOK && move.src_file == 0 &&
        move.src_rank == NUM_RANKS - 1) {
        black_can_long_castle = false;
    }
    if (move.dst_file == 0 && move.dst_rank == NUM_RANKS - 1) {
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
        assert(
            contains_enemy_piece(move.dst_file, move.src_rank, PieceType::PAWN)
        );
        board[move.dst_file][move.src_rank] = EMPTY_SQUARE;
    }

    // handle castling
    if (piece.type == PieceType::KING && move.distance() != 1) {
        assert(move.distance() == 2);
        assert(move.src_file == 4);
        assert(move.src_rank == move.dst_rank);
        if (move.dst_file == 6) { // short castle
            board[5][move.src_rank] = board[7][move.src_rank];
            board[7][move.src_rank] = EMPTY_SQUARE;
        } else if (move.dst_file == 2) { // long castle
            board[3][move.src_rank] = board[0][move.src_rank];
            board[0][move.src_rank] = EMPTY_SQUARE;
        } else {
            __builtin_unreachable();
        }
    }

    // update player to move
    if (to_move == PieceColor::WHITE) {
        to_move = PieceColor::BLACK;
    } else if (to_move == PieceColor::BLACK) {
        to_move = PieceColor::WHITE;
    } else {
        __builtin_unreachable();
    }
}


std::string ChessPosition::get_move_name(
    const std::vector<ChessMove> &legal_moves, const ChessMove &move
) const {

    assert(in_bounds(move.src_file, move.src_rank));
    const ChessPiece piece = board[move.src_file][move.src_rank];
    assert(piece.color == to_move);
    assert(piece.type != PieceType::NONE);

    assert(in_bounds(move.dst_file, move.dst_rank));
    const ChessPiece captured = board[move.dst_file][move.dst_rank];
    assert(captured.color != to_move);
    const bool is_capture = captured != EMPTY_SQUARE;

    std::ostringstream result;

    if (piece.type == PieceType::KING && move.distance() == 2) {
        if (move.dst_file == 6) {
            result << "O-O";
        } else if (move.dst_file == 2) {
            result << "O-O-O";
        } else {
            __builtin_unreachable();
        }
    } else {
        switch (piece.type) {
            case PieceType::NONE: __builtin_unreachable();
            case PieceType::KING: result << 'K'; break;
            case PieceType::QUEEN: result << 'Q'; break;
            case PieceType::ROOK: result << 'R'; break;
            case PieceType::BISHOP: result << 'B'; break;
            case PieceType::KNIGHT: result << 'N'; break;
            case PieceType::PAWN:
                if (is_capture) {
                    result << static_cast<char>('a' + move.src_file);
                }
                break;
        }
        if (piece.type != PieceType::PAWN) {
            bool ambiguous_file = false;
            bool ambiguous_rank = false;
            bool ambiguous_diag = false;
            for (const ChessMove &other : legal_moves) {
                const ChessPiece other_piece =
                    board[other.src_file][other.src_rank];
                if (other_piece.type == piece.type &&
                    other.dst_file == move.dst_file &&
                    other.dst_rank == move.dst_rank) {
                    const bool file_match = other.src_file == move.src_file;
                    const bool rank_match = other.src_rank == move.src_rank;
                    ambiguous_file |= file_match && !rank_match;
                    ambiguous_rank |= !file_match && rank_match;
                    ambiguous_diag |= !file_match && !rank_match;
                }
            }
            if (ambiguous_rank || ambiguous_file || ambiguous_diag) {
                if (!ambiguous_file) {
                    result << static_cast<char>('a' + move.src_file);
                } else if (!ambiguous_rank) {
                    result << static_cast<char>('1' + move.src_rank);
                } else {
                    result << static_cast<char>('a' + move.src_file);
                    result << static_cast<char>('1' + move.src_rank);
                }
            }
        }
        if (is_capture) { result << 'x'; }
        result << static_cast<char>('a' + move.dst_file);
        result << static_cast<char>('1' + move.dst_rank);
    }

    if (move.promotion_type != PieceType::NONE) {
        result << '=';
        switch (move.promotion_type) {
            case PieceType::NONE: [[fallthrough]];
            case PieceType::KING: __builtin_unreachable();
            case PieceType::QUEEN: result << 'Q'; break;
            case PieceType::ROOK: result << 'R'; break;
            case PieceType::BISHOP: result << 'B'; break;
            case PieceType::KNIGHT: result << 'N'; break;
            case PieceType::PAWN: __builtin_unreachable();
        }
    }

    ChessPosition copy = *this;
    copy.make_move(move);
    if (copy.checkmated()) {
        result << '#';
    } else if (copy.in_check()) {
        result << '+';
    }
    return result.str();
}


std::ostream &operator<<(std::ostream &os, const ChessPosition &pos) {
    os << "    a   b   c   d   e   f   g   h\n";
    os << "  ┌───┬───┬───┬───┬───┬───┬───┬───┐\n";
    for (coord_t rank = NUM_RANKS - 1; rank >= 0; --rank) {
        os << rank + 1 << " │";
        for (coord_t file = 0; file < NUM_FILES; ++file) {
            os << ' ' << pos.board[file][rank] << " │";
        }
        os << "\n";
        if (rank > 0) { os << "  ├───┼───┼───┼───┼───┼───┼───┼───┤\n"; }
    }
    os << "  └───┴───┴───┴───┴───┴───┴───┴───┘\n";
    return os;
}


void ChessPosition::load_fen(const std::string &fen_string) {
    std::istringstream fen(fen_string);

    { // Determine piece placement
        std::string piece_placement;
        fen >> piece_placement;

        coord_t file = 0;
        coord_t rank = NUM_RANKS - 1;
        for (const char c : piece_placement) {
            if (c >= '1' && c <= '8') {
                for (coord_t j = 0; j < c - '0'; ++j) {
                    board[file + j][rank] = EMPTY_SQUARE;
                }

                file += c - '0';
            } else if (c == '/') {
                file = 0;
                rank--;
            } else {
                switch (c) {
                    case 'K': board[file++][rank] = WHITE_KING; break;
                    case 'Q': board[file++][rank] = WHITE_QUEEN; break;
                    case 'R': board[file++][rank] = WHITE_ROOK; break;
                    case 'B': board[file++][rank] = WHITE_BISHOP; break;
                    case 'N': board[file++][rank] = WHITE_KNIGHT; break;
                    case 'P': board[file++][rank] = WHITE_PAWN; break;
                    case 'k': board[file++][rank] = BLACK_KING; break;
                    case 'q': board[file++][rank] = BLACK_QUEEN; break;
                    case 'r': board[file++][rank] = BLACK_ROOK; break;
                    case 'b': board[file++][rank] = BLACK_BISHOP; break;
                    case 'n': board[file++][rank] = BLACK_KNIGHT; break;
                    case 'p': board[file++][rank] = BLACK_PAWN; break;
                    default: break;
                }
            }
        }
    }

    { // Determine active color
        char active_color;
        fen >> active_color;
        to_move = std::tolower(active_color) == 'w' ? PieceColor::WHITE
                                                    : PieceColor::BLACK;
    }

    { // Determine castling rights
        std::string castling_rights;
        fen >> castling_rights;
        white_can_short_castle = false;
        white_can_long_castle = false;
        black_can_short_castle = false;
        black_can_long_castle = false;

        for (const char c : castling_rights) {
            switch (c) {
                case 'K': white_can_short_castle = true; break;
                case 'Q': white_can_long_castle = true; break;
                case 'k': black_can_short_castle = true; break;
                case 'q': black_can_long_castle = true; break;
                default: break;
            }
        }
    }

    { // Determine en passant square
        std::string en_passant_square;
        fen >> en_passant_square;

        if (en_passant_square == "-") {
            en_passant_file = NUM_FILES;
        } else {
            en_passant_file = en_passant_square[0] - 'a';
        }
    }
}


std::string ChessPosition::get_fen() const {
    std::ostringstream fen;

    { // Get piece placement
        unsigned char space_counter = 0;
        auto append_spaces = [&fen, &space_counter]() {
            if (space_counter > 0) {
                fen << static_cast<char>(space_counter + 0x30);
                space_counter = 0;
            }
        };

        auto append_char = [&fen](char c, PieceColor color) {
            fen
                << (color == PieceColor::WHITE
                        ? static_cast<char>(std::toupper(c))
                        : c);
        };

        for (coord_t rank = NUM_RANKS - 1; rank >= 0; --rank) {
            for (coord_t file = 0; file < NUM_FILES; ++file) {
                const ChessPiece piece = board[file][rank];
                switch (piece.type) {
                    case PieceType::KING:
                        append_spaces();
                        append_char('k', piece.color);
                        break;
                    case PieceType::QUEEN:
                        append_spaces();
                        append_char('q', piece.color);
                        break;
                    case PieceType::ROOK:
                        append_spaces();
                        append_char('r', piece.color);
                        break;
                    case PieceType::BISHOP:
                        append_spaces();
                        append_char('b', piece.color);
                        break;
                    case PieceType::KNIGHT:
                        append_spaces();
                        append_char('n', piece.color);
                        break;
                    case PieceType::PAWN:
                        append_spaces();
                        append_char('p', piece.color);
                        break;
                    case PieceType::NONE: space_counter++; break;
                    default: break;
                }
            }

            append_spaces();

            if (rank != 0) { fen << '/'; }
        }
    }
    fen << ' ';

    // Get active color
    fen << (to_move == PieceColor::WHITE ? 'w' : 'b') << ' ';

    // Get castling rights
    {
        bool at_least_one_castle_right = false;

        if (white_can_short_castle) {
            fen << 'K';
            at_least_one_castle_right = true;
        }
        if (white_can_long_castle) {
            fen << 'Q';
            at_least_one_castle_right = true;
        }
        if (black_can_short_castle) {
            fen << 'k';
            at_least_one_castle_right = true;
        }
        if (black_can_long_castle) {
            fen << 'q';
            at_least_one_castle_right = true;
        }

        if (at_least_one_castle_right) { fen << ' '; }
    }

    // Get en passant square
    if (en_passant_file != NUM_FILES) {
        fen << static_cast<char>('a' + en_passant_file)
            << static_cast<char>('1' + en_passant_rank());
    } else {
        fen << '-';
    }

    return fen.str();
}