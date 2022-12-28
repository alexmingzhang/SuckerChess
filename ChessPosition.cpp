#include "ChessPosition.hpp"

#include <cassert> // for assert
#include <cctype>  // for std::toupper, std::tolower
#include <sstream> // for std::istringstream, std::ostringstream


std::string ChessPosition::get_move_name(
    const std::vector<ChessMove> &legal_moves, ChessMove move, bool suffix
) const {
    assert(is_valid(move));
    const ChessPiece piece = (*this)[move.get_src()];
    std::ostringstream result;
    if (is_castle(move)) {
        if (move.get_dst_file() == 6) {
            result << "O-O";
        } else if (move.get_dst_file() == 2) {
            result << "O-O-O";
        } else {
            __builtin_unreachable();
        }
    } else {
        switch (piece.get_type()) {
            case PieceType::NONE: __builtin_unreachable();
            case PieceType::KING: result << 'K'; break;
            case PieceType::QUEEN: result << 'Q'; break;
            case PieceType::ROOK: result << 'R'; break;
            case PieceType::BISHOP: result << 'B'; break;
            case PieceType::KNIGHT: result << 'N'; break;
            case PieceType::PAWN:
                if (is_capture(move)) {
                    result << static_cast<char>('a' + move.get_src_file());
                }
                break;
        }
        if (piece.get_type() != PieceType::PAWN) {
            bool ambiguous_file = false;
            bool ambiguous_rank = false;
            bool ambiguous_diag = false;
            for (ChessMove other : legal_moves) {
                const ChessPiece other_piece = (*this)[other.get_src()];
                if (other_piece.get_type() == piece.get_type() &&
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
                    result << move.get_src();
                }
            }
        }
        if (is_capture(move)) { result << 'x'; }
        result << move.get_dst();
    }
    if (move.get_promotion_type() != PieceType::NONE) {
        result << '=';
        switch (move.get_promotion_type()) {
            case PieceType::NONE: __builtin_unreachable();
            case PieceType::KING: __builtin_unreachable();
            case PieceType::QUEEN: result << 'Q'; break;
            case PieceType::ROOK: result << 'R'; break;
            case PieceType::BISHOP: result << 'B'; break;
            case PieceType::KNIGHT: result << 'N'; break;
            case PieceType::PAWN: __builtin_unreachable();
        }
    }
    if (suffix) {
        ChessPosition copy = *this;
        copy.make_move(move);
        if (copy.checkmated()) {
            result << '#';
        } else if (copy.in_check()) {
            result << '+';
        }
    }
    return result.str();
}


std::ostream &operator<<(std::ostream &os, const ChessPosition &pos) {
    os << "    a   b   c   d   e   f   g   h\n";
    os << "  ┌───┬───┬───┬───┬───┬───┬───┬───┐\n";
    for (coord_t rank = NUM_RANKS - 1; rank >= 0; --rank) {
        os << rank + 1 << " │";
        for (coord_t file = 0; file < NUM_FILES; ++file) {
            os << ' ' << pos(file, rank) << " │";
        }
        os << "\n";
        if (rank > 0) { os << "  ├───┼───┼───┼───┼───┼───┼───┼───┤\n"; }
    }
    os << "  └───┴───┴───┴───┴───┴───┴───┴───┘\n";
    return os;
}


// static constexpr std::array<char, 20> FEN_BOARD_CHARACTERS = {
//     'K', 'Q', 'R', 'B', 'N', 'P',           // white pieces
//     'k', 'q', 'r', 'b', 'n', 'p',           // black pieces
//     '1', '2', '3', '4', '5', '6', '7', '8', // empty squares
// };


void ChessPosition::load_fen(const std::string &fen_string) {
    std::istringstream fen(fen_string);

    { // Determine piece placement
        std::string piece_placement;
        fen >> piece_placement;

        coord_t file = 0;
        coord_t rank = NUM_RANKS - 1;
        [[maybe_unused]] bool white_king_found = false;
        [[maybe_unused]] bool black_king_found = false;
        for (const char c : piece_placement) {
            if (c >= '1' && c <= '8') {
                for (coord_t j = 0; j < c - '0'; ++j) {
                    (*this)(file + j, rank) = EMPTY_SQUARE;
                }
                file += c - '0';
            } else if (c == '/') {
                file = 0;
                rank--;
            } else {
                switch (c) {
                    case 'K':
                        assert(!white_king_found);
                        white_king_found = true;
                        white_king_location = {file, rank};
                        (*this)(file++, rank) = WHITE_KING;
                        break;
                    case 'Q': (*this)(file++, rank) = WHITE_QUEEN; break;
                    case 'R': (*this)(file++, rank) = WHITE_ROOK; break;
                    case 'B': (*this)(file++, rank) = WHITE_BISHOP; break;
                    case 'N': (*this)(file++, rank) = WHITE_KNIGHT; break;
                    case 'P': (*this)(file++, rank) = WHITE_PAWN; break;
                    case 'k':
                        assert(!black_king_found);
                        black_king_found = true;
                        black_king_location = {file, rank};
                        (*this)(file++, rank) = BLACK_KING;
                        break;
                    case 'q': (*this)(file++, rank) = BLACK_QUEEN; break;
                    case 'r': (*this)(file++, rank) = BLACK_ROOK; break;
                    case 'b': (*this)(file++, rank) = BLACK_BISHOP; break;
                    case 'n': (*this)(file++, rank) = BLACK_KNIGHT; break;
                    case 'p': (*this)(file++, rank) = BLACK_PAWN; break;
                    default: break;
                }
            }
        }
        assert(white_king_found);
        assert(black_king_found);
    }

    { // Determine active color
        char active_color;
        fen >> active_color;
        to_move = std::tolower(active_color) == 'w' ? PieceColor::WHITE
                                                    : PieceColor::BLACK;
    }

    { // Determine castling rights
        std::string castling_rights_str;
        fen >> castling_rights_str;
        castling_rights = CastlingRights(castling_rights_str);
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
                const ChessPiece piece = (*this)(file, rank);
                switch (piece.get_type()) {
                    case PieceType::KING:
                        append_spaces();
                        append_char('k', piece.get_color());
                        break;
                    case PieceType::QUEEN:
                        append_spaces();
                        append_char('q', piece.get_color());
                        break;
                    case PieceType::ROOK:
                        append_spaces();
                        append_char('r', piece.get_color());
                        break;
                    case PieceType::BISHOP:
                        append_spaces();
                        append_char('b', piece.get_color());
                        break;
                    case PieceType::KNIGHT:
                        append_spaces();
                        append_char('n', piece.get_color());
                        break;
                    case PieceType::PAWN:
                        append_spaces();
                        append_char('p', piece.get_color());
                        break;
                    case PieceType::NONE: space_counter++; break;
                }
            }
            append_spaces();
            if (rank != 0) { fen << '/'; }
        }
    }
    fen << ' ';

    fen << (to_move == PieceColor::WHITE ? 'w' : 'b') << ' ';
    fen << castling_rights << ' ';

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
            material_advantage += (*this)(file, rank).material_value();
        }
    }

    return material_advantage;
}
