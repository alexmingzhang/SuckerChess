#include "ChessPosition.hpp"

#include <cassert> // for assert
#include <cctype>  // for std::toupper
#include <cmath>   // for std::abs
#include <sstream> // for std::istringstream, std::ostringstream


void ChessPosition::push_leaper_move(
    std::vector<ChessMove> &moves, ChessSquare src, ChessOffset offset
) const {
    const ChessSquare dst = src + offset;
    if (is_legal_dst(dst)) { moves.emplace_back(src, dst); }
}


void ChessPosition::push_slider_moves(
    std::vector<ChessMove> &moves, ChessSquare src, ChessOffset offset
) const {
    ChessSquare dst = src + offset;
    while (is_empty(dst)) {
        moves.emplace_back(src, dst);
        dst += offset;
    }
    if (is_legal_dst(dst)) { moves.emplace_back(src, dst); }
}


void ChessPosition::push_promotion_moves(
    std::vector<ChessMove> &moves, ChessSquare src, ChessSquare dst
) const {
    using enum PieceType;
    if (dst.rank == promotion_rank()) {
        moves.emplace_back(src, dst, QUEEN);
        moves.emplace_back(src, dst, ROOK);
        moves.emplace_back(src, dst, BISHOP);
        moves.emplace_back(src, dst, KNIGHT);
    } else {
        moves.emplace_back(src, dst);
    }
}


void ChessPosition::push_pawn_moves(
    std::vector<ChessMove> &moves, ChessSquare src
) const {
    const coord_t direction = pawn_direction();
    const ChessSquare dst_move = src + ChessOffset{0, direction};

    // non-capturing
    if (is_empty(dst_move)) {
        push_promotion_moves(moves, src, dst_move);

        // non-capturing double move
        if (src.rank == pawn_origin_rank()) {
            const ChessSquare dst_double_move =
                dst_move + ChessOffset{0, direction};
            if (is_empty(dst_double_move)) {
                moves.emplace_back(src, dst_double_move);
            }
        }
    }

    // capture to src_file - 1
    const ChessSquare dst_capture_l = src + ChessOffset{-1, direction};
    if (is_legal_cap(dst_capture_l)) {
        push_promotion_moves(moves, src, dst_capture_l);
    }

    // capture to src_file + 1
    const ChessSquare dst_capture_r = src + ChessOffset{+1, direction};
    if (is_legal_cap(dst_capture_r)) {
        push_promotion_moves(moves, src, dst_capture_r);
    }

    // en-passant
    const ChessSquare dst_ep = en_passant_square();
    if (dst_ep.in_bounds() &&
        (dst_capture_l == dst_ep || dst_capture_r == dst_ep)) {
        moves.emplace_back(src, dst_ep);
    }
}


void ChessPosition::push_castling_moves(std::vector<ChessMove> &moves) const {
    if (to_move == PieceColor::WHITE) {
        if (white_can_short_castle) {
            assert(board[4][0] == WHITE_KING);
            assert(board[7][0] == WHITE_ROOK);
            if (board[5][0] == EMPTY_SQUARE && board[6][0] == EMPTY_SQUARE &&
                !is_attacked({4, 0}) && !is_attacked({5, 0}) &&
                !is_attacked({6, 0})) {
                moves.emplace_back(ChessSquare{4, 0}, ChessSquare{6, 0});
            }
        }
        if (white_can_long_castle) {
            assert(board[0][0] == WHITE_ROOK);
            assert(board[4][0] == WHITE_KING);
            if (board[1][0] == EMPTY_SQUARE && board[2][0] == EMPTY_SQUARE &&
                board[3][0] == EMPTY_SQUARE && !is_attacked({2, 0}) &&
                !is_attacked({3, 0}) && !is_attacked({4, 0})) {
                moves.emplace_back(ChessSquare{4, 0}, ChessSquare{2, 0});
            }
        }
    } else if (to_move == PieceColor::BLACK) {
        if (black_can_short_castle) {
            assert(board[4][7] == BLACK_KING);
            assert(board[7][7] == BLACK_ROOK);
            if (board[5][7] == EMPTY_SQUARE && board[6][7] == EMPTY_SQUARE &&
                !is_attacked({4, 7}) && !is_attacked({5, 7}) &&
                !is_attacked({6, 7})) {
                moves.emplace_back(ChessSquare{4, 7}, ChessSquare{6, 7});
            }
        }
        if (black_can_long_castle) {
            assert(board[0][7] == BLACK_ROOK);
            assert(board[4][7] == BLACK_KING);
            if (board[1][7] == EMPTY_SQUARE && board[2][7] == EMPTY_SQUARE &&
                board[3][7] == EMPTY_SQUARE && !is_attacked({2, 7}) &&
                !is_attacked({3, 7}) && !is_attacked({4, 7})) {
                moves.emplace_back(ChessSquare{4, 7}, ChessSquare{2, 7});
            }
        }
    } else {
        __builtin_unreachable();
    }
}


void ChessPosition::push_all_moves(
    std::vector<ChessMove> &moves, ChessSquare src
) const {
    assert(src.in_bounds());
    const ChessPiece piece = (*this)[src];
    assert(piece.color == to_move);
    assert(piece.type != PieceType::NONE);
    switch (piece.type) {
        case PieceType::NONE: __builtin_unreachable();
        case PieceType::KING:
            push_leaper_move(moves, src, {-1, -1});
            push_leaper_move(moves, src, {-1, 0});
            push_leaper_move(moves, src, {-1, +1});
            push_leaper_move(moves, src, {0, -1});
            push_leaper_move(moves, src, {0, +1});
            push_leaper_move(moves, src, {+1, -1});
            push_leaper_move(moves, src, {+1, 0});
            push_leaper_move(moves, src, {+1, +1});
            push_castling_moves(moves);
            break;
        case PieceType::QUEEN:
            push_slider_moves(moves, src, {-1, -1});
            push_slider_moves(moves, src, {-1, 0});
            push_slider_moves(moves, src, {-1, +1});
            push_slider_moves(moves, src, {0, -1});
            push_slider_moves(moves, src, {0, +1});
            push_slider_moves(moves, src, {+1, -1});
            push_slider_moves(moves, src, {+1, 0});
            push_slider_moves(moves, src, {+1, +1});
            break;
        case PieceType::ROOK:
            push_slider_moves(moves, src, {-1, 0});
            push_slider_moves(moves, src, {0, -1});
            push_slider_moves(moves, src, {0, +1});
            push_slider_moves(moves, src, {+1, 0});
            break;
        case PieceType::BISHOP:
            push_slider_moves(moves, src, {-1, -1});
            push_slider_moves(moves, src, {-1, +1});
            push_slider_moves(moves, src, {+1, -1});
            push_slider_moves(moves, src, {+1, +1});
            break;
        case PieceType::KNIGHT:
            push_leaper_move(moves, src, {-2, -1});
            push_leaper_move(moves, src, {-2, +1});
            push_leaper_move(moves, src, {-1, -2});
            push_leaper_move(moves, src, {-1, +2});
            push_leaper_move(moves, src, {+1, -2});
            push_leaper_move(moves, src, {+1, +2});
            push_leaper_move(moves, src, {+2, -1});
            push_leaper_move(moves, src, {+2, +1});
            break;
        case PieceType::PAWN: push_pawn_moves(moves, src); break;
    }
}


std::vector<ChessMove> ChessPosition::get_all_moves() const {
    std::vector<ChessMove> result;
    for (coord_t src_file = 0; src_file < NUM_FILES; ++src_file) {
        for (coord_t src_rank = 0; src_rank < NUM_RANKS; ++src_rank) {
            const ChessPiece &piece = board[src_file][src_rank];
            if (piece.color == to_move) {
                push_all_moves(result, {src_file, src_rank});
            }
        }
    }
    return result;
}


void ChessPosition::make_move(const ChessMove &move) {

    using enum PieceType;

    // get and validate moving piece
    assert(move.get_src().in_bounds());
    assert(move.get_dst().in_bounds());
    ChessPiece piece = (*this)[move.get_src()];
    assert(piece.color != PieceColor::NONE);
    assert(piece.color == to_move);

    // validate move
    switch (piece.type) {
        case NONE: __builtin_unreachable();
        case KING:
            assert(
                move.distance() == 1 ||
                (move.distance() == 2 &&
                 move.get_src_rank() == move.get_dst_rank())
            );
            break;
        case QUEEN: assert(move.is_orthogonal() || move.is_diagonal()); break;
        case ROOK: assert(move.is_orthogonal()); break;
        case BISHOP: assert(move.is_diagonal()); break;
        case KNIGHT:
            assert(move.distance() == 2);
            assert(!move.is_orthogonal());
            assert(!move.is_diagonal());
            break;
        case PAWN:
            assert(move.get_src_rank() != move.get_dst_rank());
            assert(
                move.distance() == 1 ||
                (move.distance() == 2 &&
                 move.get_src_file() == move.get_dst_file())
            );
            break;
    }

    // check for en passant capture
    const bool is_en_passant_capture =
        piece.type == PAWN && move.get_src_file() != move.get_dst_file() &&
        (*this)[move.get_dst()] == EMPTY_SQUARE;

    // record en passant file
    if (piece.type == PAWN &&
        (move.get_src_rank() - move.get_dst_rank() == 2 ||
         move.get_src_rank() - move.get_dst_rank() == -2)) {
        assert(move.get_src_file() == move.get_dst_file());
        en_passant_file = move.get_src_file();
    } else {
        en_passant_file = NUM_FILES;
    }

    // update castling rights
    constexpr ChessSquare WHITE_SHORT_ROOK_SQUARE = {NUM_FILES - 1, 0};
    constexpr ChessSquare WHITE_LONG_ROOK_SQUARE = {0, 0};
    constexpr ChessSquare BLACK_SHORT_ROOK_SQUARE = {
        NUM_FILES - 1, NUM_RANKS - 1};
    constexpr ChessSquare BLACK_LONG_ROOK_SQUARE = {0, NUM_RANKS - 1};

    if (piece == WHITE_KING) {
        white_can_short_castle = false;
        white_can_long_castle = false;
    }
    if (move.get_src() == WHITE_SHORT_ROOK_SQUARE ||
        move.get_dst() == WHITE_SHORT_ROOK_SQUARE) {
        white_can_short_castle = false;
    }
    if (move.get_src() == WHITE_LONG_ROOK_SQUARE ||
        move.get_dst() == WHITE_LONG_ROOK_SQUARE) {
        white_can_long_castle = false;
    }
    if (piece == BLACK_KING) {
        black_can_short_castle = false;
        black_can_long_castle = false;
    }
    if (move.get_src() == BLACK_SHORT_ROOK_SQUARE ||
        move.get_dst() == BLACK_SHORT_ROOK_SQUARE) {
        black_can_short_castle = false;
    }
    if (move.get_src() == BLACK_LONG_ROOK_SQUARE ||
        move.get_dst() == BLACK_LONG_ROOK_SQUARE) {
        black_can_long_castle = false;
    }

    // handle promotion
    if (move.get_promotion_type() != NONE) {
        piece.type = move.get_promotion_type();
    }

    // perform move
    (*this)[move.get_dst()] = piece;
    (*this)[move.get_src()] = EMPTY_SQUARE;

    // handle en passant capture
    if (is_en_passant_capture) {
        assert(has_enemy_piece({move.get_dst_file(), move.get_src_rank()}, PAWN)
        );
        board[move.get_dst_file()][move.get_src_rank()] = EMPTY_SQUARE;
    }

    // handle castling
    if (piece.type == KING && move.distance() != 1) {
        assert(move.distance() == 2);
        assert(move.get_src_file() == 4);
        assert(move.get_src_rank() == move.get_dst_rank());
        const coord_t rank = move.get_src_rank();
        if (move.get_dst_file() == 6) { // short castle
            assert(board[5][rank] == EMPTY_SQUARE);
            assert(board[6][rank] == EMPTY_SQUARE);
            assert(board[7][rank].color == to_move);
            assert(board[7][rank].type == ROOK);
            board[5][rank] = board[7][rank];
            board[7][rank] = EMPTY_SQUARE;
        } else if (move.get_dst_file() == 2) { // long castle
            assert(board[0][rank].color == to_move);
            assert(board[0][rank].type == ROOK);
            assert(board[1][rank] == EMPTY_SQUARE);
            assert(board[2][rank] == EMPTY_SQUARE);
            assert(board[3][rank] == EMPTY_SQUARE);
            board[3][rank] = board[0][rank];
            board[0][rank] = EMPTY_SQUARE;
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

    assert(move.get_src().in_bounds());
    const ChessPiece piece = (*this)[move.get_src()];
    assert(piece.color == to_move);
    assert(piece.type != PieceType::NONE);

    assert(move.get_dst().in_bounds());
    const ChessPiece captured = (*this)[move.get_dst()];
    assert(captured.color != to_move);
    const bool is_capture = captured != EMPTY_SQUARE;

    std::ostringstream result;

    if (piece.type == PieceType::KING && move.distance() == 2) {
        if (move.get_dst_file() == 6) {
            result << "O-O";
        } else if (move.get_dst_file() == 2) {
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
                    result << static_cast<char>('a' + move.get_src_file());
                }
                break;
        }
        if (piece.type != PieceType::PAWN) {
            bool ambiguous_file = false;
            bool ambiguous_rank = false;
            bool ambiguous_diag = false;
            for (const ChessMove &other : legal_moves) {
                const ChessPiece other_piece = (*this)[other.get_src()];
                if (other_piece.type == piece.type &&
                    other.get_dst() == move.get_dst()) {
                    const bool file_match =
                        other.get_src_file() == move.get_src_file();
                    const bool rank_match =
                        other.get_src_rank() == move.get_src_rank();
                    ambiguous_file |= file_match && !rank_match;
                    ambiguous_rank |= !file_match && rank_match;
                    ambiguous_diag |= !file_match && !rank_match;
                }
            }
            if (ambiguous_rank || ambiguous_file || ambiguous_diag) {
                if (!ambiguous_file) {
                    result << static_cast<char>('a' + move.get_src_file());
                } else if (!ambiguous_rank) {
                    result << static_cast<char>('1' + move.get_src_rank());
                } else {
                    result << static_cast<char>('a' + move.get_src_file());
                    result << static_cast<char>('1' + move.get_src_rank());
                }
            }
        }
        if (is_capture) { result << 'x'; }
        result << static_cast<char>('a' + move.get_dst_file());
        result << static_cast<char>('1' + move.get_dst_rank());
    }

    if (move.get_promotion_type() != PieceType::NONE) {
        result << '=';
        switch (move.get_promotion_type()) {
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
    if (en_passant_square().in_bounds()) {
        fen << en_passant_square();
    } else {
        fen << '-';
    }

    return fen.str();
}


int ChessPosition::get_material_advantage() const {
    int material_advantage = 0;

    for (coord_t file = 0; file < NUM_FILES; ++file) {
        for (coord_t rank = 0; rank < NUM_RANKS; ++rank) {
            material_advantage += board[file][rank].get_material_value();
        }
    }

    return material_advantage;
}