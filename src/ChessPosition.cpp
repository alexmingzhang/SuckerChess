#include "ChessPosition.hpp"

#include <algorithm> // for std::sort
#include <cstdint>   // for std::uint64_t, UINT64_C
#include <sstream>   // for std::istringstream, std::ostringstream
#include <stdexcept> // for std::invalid_argument


bool ChessPosition::check_consistency() const noexcept {

    std::vector<ChessMove> generated_valid_white_moves;
    visit_valid_moves(PieceColor::WHITE, [&](ChessMove move) {
        generated_valid_white_moves.push_back(move);
    });

    std::vector<ChessMove> generated_valid_black_moves;
    visit_valid_moves(PieceColor::BLACK, [&](ChessMove move) {
        generated_valid_black_moves.push_back(move);
    });

    std::vector<ChessMove> generated_legal_white_moves;
    visit_legal_moves(
        PieceColor::WHITE,
        [&](ChessMove move, const ChessPosition &) {
            generated_legal_white_moves.push_back(move);
        }
    );

    std::vector<ChessMove> generated_legal_black_moves;
    visit_legal_moves(
        PieceColor::BLACK,
        [&](ChessMove move, const ChessPosition &) {
            generated_legal_black_moves.push_back(move);
        }
    );

    std::vector<ChessMove> filtered_valid_white_moves;
    std::vector<ChessMove> filtered_valid_black_moves;
    std::vector<ChessMove> filtered_legal_white_moves;
    std::vector<ChessMove> filtered_legal_black_moves;

    const auto push_move = [&](ChessMove move) {
        if (is_valid(move)) {
            const PieceColor color = get_moving_color(move);
            switch (color) {
                case PieceColor::NONE: __builtin_unreachable();
                case PieceColor::WHITE:
                    filtered_valid_white_moves.push_back(move);
                    break;
                case PieceColor::BLACK:
                    filtered_valid_black_moves.push_back(move);
                    break;
            }

            if (board.get_piece(move.get_dst()).get_type() != PieceType::KING) {
                ChessPosition next = *this;
                next.make_move(move);

                switch (color) {
                    case PieceColor::NONE: __builtin_unreachable();
                    case PieceColor::WHITE:
                        if (!next.in_check(color)) {
                            filtered_legal_white_moves.push_back(move);
                        }
                        break;
                    case PieceColor::BLACK:
                        if (!next.in_check(color)) {
                            filtered_legal_black_moves.push_back(move);
                        }
                        break;
                }
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
    const ChessPiece piece = board.get_piece(move.get_src());
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
                const ChessPiece other_piece = board.get_piece(other.get_src());
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
        if (copy.in_check()) {
            bool has_legal_moves = false;
            copy.visit_legal_moves([&](ChessMove, const ChessPosition &) {
                has_legal_moves = true;
            });
            result << (has_legal_moves ? '+' : '#');
        }
    }
    return result.str();
}


std::ostream &operator<<(std::ostream &os, const ChessPosition &pos) {
    os << "    a   b   c   d   e   f   g   h\n";
    os << "  ???????????????????????????????????????????????????????????????????????????????????????????????????\n";
    for (coord_t rank = NUM_RANKS - 1; rank >= 0; --rank) {
        os << rank + 1 << " ???";
        for (coord_t file = 0; file < NUM_FILES; ++file) {
            os << ' ' << pos.get_board().get_piece(file, rank) << " ???";
        }
        os << "\n";
        if (rank > 0) { os << "  ???????????????????????????????????????????????????????????????????????????????????????????????????\n"; }
    }
    os << "  ???????????????????????????????????????????????????????????????????????????????????????????????????\n";
    return os;
}


void ChessPosition::load_fen(const std::string &fen_string) {
    std::istringstream fen(fen_string);

    std::string fen_board_str;
    fen >> fen_board_str;
    board = ChessBoard(fen_board_str);

#ifdef SUCKER_CHESS_TRACK_KING_LOCATIONS
    const ChessSquare white_king_location = board.find_unique_piece(WHITE_KING);
    const ChessSquare black_king_location = board.find_unique_piece(BLACK_KING);
    assert(white_king_location.in_bounds());
    assert(black_king_location.in_bounds());
    white_king_location_data = static_cast<std::uint8_t>(
        (white_king_location.file << 4) | white_king_location.rank
    );
    black_king_location_data = static_cast<std::uint8_t>(
        (black_king_location.file << 4) | black_king_location.rank
    );
#endif

    char fen_color_char;
    fen >> fen_color_char;
    switch (fen_color_char) {
        case 'W': move_data = 0x00; break;
        case 'w': move_data = 0x00; break;
        case 'B': move_data = 0x10; break;
        case 'b': move_data = 0x10; break;
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
    if (fen_en_passant_str != "-") {
        if (fen_en_passant_str.size() != 2) {
            throw std::invalid_argument(
                "FEN en passant field is not a valid square"
            );
        }
        const char fen_en_passant_rank = fen_en_passant_str[1];
        const bool valid_rank = ((get_color_to_move() == PieceColor::WHITE) &&
                                 (fen_en_passant_rank == '6')) ||
                                ((get_color_to_move() == PieceColor::BLACK) &&
                                 (fen_en_passant_rank == '3'));
        if (!valid_rank) {
            throw std::invalid_argument("FEN en passant rank is invalid");
        }
        const char fen_en_passant_file = fen_en_passant_str[0];
        if ((fen_en_passant_file < 'a') || (fen_en_passant_file > 'h')) {
            throw std::invalid_argument("FEN en passant file is invalid");
        }
        move_data |= 0x08;
        move_data |= static_cast<std::uint8_t>(fen_en_passant_file - 'a');
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


std::string ChessPosition::get_fen() const noexcept {
    std::ostringstream fen;
    fen << board << ' ' << fen_char(get_color_to_move()) << ' '
        << castling_rights << ' ';
    if (is_en_passant_available()) {
        fen << en_passant_square();
    } else {
        fen << '-';
    }
    return fen.str();
}


std::size_t std::hash<ChessPosition>::operator()(const ChessPosition &pos
) const noexcept {
    // FNV-1a hash algorithm from:
    // https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function#FNV-1a_hash
    std::uint64_t result = UINT64_C(0xcbf29ce484222325);
    const char *ptr = reinterpret_cast<const char *>(&pos);
    for (std::size_t i = 0; i < sizeof(ChessPosition); ++i) {
        result ^= static_cast<std::uint64_t>(ptr[i]);
        result *= UINT64_C(0x00000100000001b3);
    }
    return static_cast<std::size_t>(result);
}
