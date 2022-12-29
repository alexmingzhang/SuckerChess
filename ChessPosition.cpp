#include "ChessPosition.hpp"

#include <algorithm> // for std::sort
#include <iostream>  // for std::cout, std::endl
#include <sstream>   // for std::istringstream, std::ostringstream


bool ChessPosition::check_consistency() const noexcept {
    std::vector<ChessMove> generated_valid_white_moves =
        get_valid_moves(PieceColor::WHITE);
    std::vector<ChessMove> generated_valid_black_moves =
        get_valid_moves(PieceColor::BLACK);
    std::vector<ChessMove> generated_legal_white_moves =
        get_legal_moves(PieceColor::WHITE);
    std::vector<ChessMove> generated_legal_black_moves =
        get_legal_moves(PieceColor::BLACK);

    std::vector<ChessMove> filtered_valid_white_moves;
    std::vector<ChessMove> filtered_valid_black_moves;
    std::vector<ChessMove> filtered_legal_white_moves;
    std::vector<ChessMove> filtered_legal_black_moves;

    const auto push_move = [&](ChessMove move) {
        if (is_valid(move)) {
            switch (get_moving_color(move)) {
                case PieceColor::NONE: __builtin_unreachable();
                case PieceColor::WHITE:
                    filtered_valid_white_moves.push_back(move);
                    break;
                case PieceColor::BLACK:
                    filtered_valid_black_moves.push_back(move);
                    break;
            }
        }
        if (is_legal(move)) {
            switch (get_moving_color(move)) {
                case PieceColor::NONE: __builtin_unreachable();
                case PieceColor::WHITE:
                    filtered_legal_white_moves.push_back(move);
                    break;
                case PieceColor::BLACK:
                    filtered_legal_black_moves.push_back(move);
                    break;
            }
        }
    };

    for (coord_t src_file = 0; src_file < NUM_FILES; ++src_file) {
        for (coord_t src_rank = 0; src_rank < NUM_RANKS; ++src_rank) {
            const ChessSquare src = {src_file, src_rank};
            for (coord_t dst_file = 0; dst_file < NUM_FILES; ++dst_file) {
                for (coord_t dst_rank = 0; dst_rank < NUM_RANKS; ++dst_rank) {
                    const ChessSquare dst = {dst_file, dst_rank};
                    push_move({src, dst});
                    push_move({src, dst, PieceType::QUEEN});
                    push_move({src, dst, PieceType::ROOK});
                    push_move({src, dst, PieceType::BISHOP});
                    push_move({src, dst, PieceType::KNIGHT});
                }
            }
        }
    }

    std::sort(
        generated_valid_white_moves.begin(), generated_valid_white_moves.end()
    );
    std::sort(
        filtered_valid_white_moves.begin(), filtered_valid_white_moves.end()
    );
    if (generated_valid_white_moves != filtered_valid_white_moves) {
        return false;
    }

    std::sort(
        generated_valid_black_moves.begin(), generated_valid_black_moves.end()
    );
    std::sort(
        filtered_valid_black_moves.begin(), filtered_valid_black_moves.end()
    );
    if (generated_valid_black_moves != filtered_valid_black_moves) {
        return false;
    }

    std::sort(
        generated_legal_white_moves.begin(), generated_legal_white_moves.end()
    );
    std::sort(
        filtered_legal_white_moves.begin(), filtered_legal_white_moves.end()
    );
    if (generated_legal_white_moves != filtered_legal_white_moves) {
        return false;
    }

    std::sort(
        generated_legal_black_moves.begin(), generated_legal_black_moves.end()
    );
    std::sort(
        filtered_legal_black_moves.begin(), filtered_legal_black_moves.end()
    );
    if (generated_legal_black_moves != filtered_legal_black_moves) {
        return false;
    }

    ChessPosition fen_round_trip(get_fen());
    return (*this) == fen_round_trip;
}


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
        } else if (puts_opponent_in_check(move)) {
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


void ChessPosition::load_fen(const std::string &fen_string) {
    std::istringstream fen(fen_string);

    std::string fen_board_str;
    fen >> fen_board_str;
    board = ChessBoard(fen_board_str);
    white_king_location = board.find_unique_piece(WHITE_KING);
    black_king_location = board.find_unique_piece(BLACK_KING);

    char fen_color_char;
    fen >> fen_color_char;
    switch (fen_color_char) {
        case 'W': to_move = PieceColor::WHITE; break;
        case 'w': to_move = PieceColor::WHITE; break;
        case 'B': to_move = PieceColor::BLACK; break;
        case 'b': to_move = PieceColor::BLACK; break;
        default:
            throw std::invalid_argument(
                "FEN active color field contains invalid character: " +
                std::string{fen_color_char}
            );
    }

    std::string fen_rights_str;
    fen >> fen_rights_str;
    castling_rights = CastlingRights(fen_rights_str);

    std::string fen_en_passant_str;
    fen >> fen_en_passant_str;
    if (fen_en_passant_str == "-") {
        en_passant_file = NUM_FILES;
    } else {
        if (fen_en_passant_str.size() != 2) {
            throw std::invalid_argument(
                "FEN en passant field is not a valid square"
            );
        }
        const char fen_en_passant_rank = fen_en_passant_str[1];
        const bool valid_rank =
            ((to_move == PieceColor::WHITE) && (fen_en_passant_rank == '6')) ||
            ((to_move == PieceColor::BLACK) && (fen_en_passant_rank == '3'));
        if (!valid_rank) {
            throw std::invalid_argument("FEN en passant rank is invalid");
        }
        const char fen_en_passant_file = fen_en_passant_str[0];
        if ((fen_en_passant_file < 'a') || (fen_en_passant_file > 'h')) {
            throw std::invalid_argument("FEN en passant file is invalid");
        }
        en_passant_file = static_cast<coord_t>(fen_en_passant_file - 'a');
    }
}


static constexpr char fen_char(PieceColor color) noexcept {
    switch (color) {
        case PieceColor::NONE: __builtin_unreachable();
        case PieceColor::WHITE: return 'w';
        case PieceColor::BLACK: return 'b';
    }
    __builtin_unreachable();
}


std::string ChessPosition::get_fen() const {
    std::ostringstream fen;
    fen << board << ' ' << fen_char(to_move) << ' ' << castling_rights << ' ';
    if (en_passant_square().in_bounds()) {
        fen << en_passant_square();
    } else {
        fen << '-';
    }
    return fen.str();
}


int ChessPosition::get_material_advantage() const {
    int result = 0;
    for (coord_t file = 0; file < NUM_FILES; ++file) {
        for (coord_t rank = 0; rank < NUM_RANKS; ++rank) {
            result += (*this)(file, rank).material_value();
        }
    }
    return result;
}
