#include "ChessBoard.hpp"

#include <stdexcept> // for std::invalid_argument


ChessBoard::ChessBoard(const std::string &fen_board_str)
    : data() {

    // start in top-left corner of board
    coord_t file = 0;
    coord_t rank = NUM_RANKS - 1;

    // every time we insert a new entry, make sure we haven't
    // inserted too many entries into a single row
    const auto push = [&](ChessPiece piece) {
        (*this)(file++, rank) = piece;
        if (file > NUM_FILES) {
            throw std::invalid_argument(
                "FEN board string contains a row with more than eight entries"
            );
        }
    };

    for (char c : fen_board_str) {
        if (c >= '1' && c <= '8') {
            for (; c > '0'; --c) { push(EMPTY_SQUARE); }
        } else if (c == '/') {
            // make sure we inserted exactly eight entries into this row
            if (file != NUM_FILES) {
                throw std::invalid_argument("FEN board string contains a row "
                                            "with less than eight entries");
            }
            // make sure we haven't processed more than eight rows
            if (rank <= 0) {
                throw std::invalid_argument(
                    "FEN board string contains more than eight rows"
                );
            }
            // move on to next row
            file = 0;
            --rank;
        } else {
            switch (c) {
                case 'K': push(WHITE_KING); break;
                case 'Q': push(WHITE_QUEEN); break;
                case 'R': push(WHITE_ROOK); break;
                case 'B': push(WHITE_BISHOP); break;
                case 'N': push(WHITE_KNIGHT); break;
                case 'P': push(WHITE_PAWN); break;
                case 'k': push(BLACK_KING); break;
                case 'q': push(BLACK_QUEEN); break;
                case 'r': push(BLACK_ROOK); break;
                case 'b': push(BLACK_BISHOP); break;
                case 'n': push(BLACK_KNIGHT); break;
                case 'p': push(BLACK_PAWN); break;
                default:
                    throw std::invalid_argument(
                        "FEN board string contains invalid character: " +
                        std::string{c}
                    );
            }
        }
    }

    // make sure we processed exactly eight rows
    if (rank != 0) {
        throw std::invalid_argument(
            "FEN board string contains less than eight rows"
        );
    }

    // make sure final row contains exactly eight entries
    if (file != NUM_FILES) {
        throw std::invalid_argument("FEN board string contains a row "
                                    "with less than eight entries");
    }
}


ChessSquare ChessBoard::find_unique_piece(ChessPiece piece) const {
    ChessSquare result;
    bool found = false;
    for (coord_t file = 0; file < NUM_FILES; ++file) {
        for (coord_t rank = 0; rank < NUM_RANKS; ++rank) {
            const ChessSquare square = {file, rank};
            if ((*this)[square] == piece) {
                if (found) {
                    throw std::invalid_argument(
                        "found more than one of desired piece"
                    );
                }
                result = square;
                found = true;
            }
        }
    }
    if (!found) { throw std::invalid_argument("failed to find desired piece"); }
    return result;
}


static constexpr char fen_char(ChessPiece piece) noexcept {
    switch (piece.get_color()) {
        case PieceColor::NONE: __builtin_unreachable();
        case PieceColor::WHITE:
            switch (piece.get_type()) {
                case PieceType::NONE: __builtin_unreachable();
                case PieceType::KING: return 'K';
                case PieceType::QUEEN: return 'Q';
                case PieceType::ROOK: return 'R';
                case PieceType::BISHOP: return 'B';
                case PieceType::KNIGHT: return 'N';
                case PieceType::PAWN: return 'P';
            }
            __builtin_unreachable();
        case PieceColor::BLACK:
            switch (piece.get_type()) {
                case PieceType::NONE: __builtin_unreachable();
                case PieceType::KING: return 'k';
                case PieceType::QUEEN: return 'q';
                case PieceType::ROOK: return 'r';
                case PieceType::BISHOP: return 'b';
                case PieceType::KNIGHT: return 'n';
                case PieceType::PAWN: return 'p';
            }
            __builtin_unreachable();
    }
    __builtin_unreachable();
}


std::ostream &operator<<(std::ostream &os, const ChessBoard &board) {
    int num_empty_squares = 0;
    auto print_empty_squares = [&]() {
        if (num_empty_squares > 0) {
            os << static_cast<char>('0' + num_empty_squares);
        }
        num_empty_squares = 0;
    };
    for (coord_t rank = NUM_RANKS - 1; rank >= 0; --rank) {
        for (coord_t file = 0; file < NUM_FILES; ++file) {
            const ChessPiece piece = board(file, rank);
            if (piece == EMPTY_SQUARE) {
                ++num_empty_squares;
            } else {
                print_empty_squares();
                os << fen_char(piece);
            }
        }
        print_empty_squares();
        if (rank != 0) { os << '/'; }
    }
    return os;
}
