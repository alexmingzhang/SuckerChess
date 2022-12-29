#ifndef SUCKER_CHESS_CHESS_POSITION_HPP
#define SUCKER_CHESS_CHESS_POSITION_HPP

#include <cassert> // for assert
#include <ostream> // for std::ostream
#include <string>  // for std::string
#include <vector>  // for std::vector

#include "CastlingRights.hpp"
#include "ChessBoard.hpp"
#include "ChessMove.hpp"
#include "ChessPiece.hpp"


class ChessPosition {

    ChessBoard board;
    ChessSquare white_king_location;
    ChessSquare black_king_location;
    PieceColor to_move;
    coord_t en_passant_file;
    CastlingRights castling_rights;

public: // ======================================================== CONSTRUCTORS

    explicit constexpr ChessPosition() noexcept
        : board()
        , white_king_location({4, 0})
        , black_king_location({4, NUM_RANKS - 1})
        , to_move(PieceColor::WHITE)
        , en_passant_file(NUM_FILES)
        , castling_rights(true, true, true, true) {}

    explicit ChessPosition(const std::string &fen)
        : board()
        , white_king_location()
        , black_king_location()
        , to_move(PieceColor::NONE)
        , en_passant_file(NUM_FILES)
        , castling_rights(false, false, false, false) {
        load_fen(fen);
    }

public: // =========================================================== ACCESSORS

    [[nodiscard]] constexpr const ChessBoard &get_board() const noexcept {
        return board;
    }

    [[nodiscard]] constexpr ChessSquare
    get_white_king_location() const noexcept {
        return white_king_location;
    }

    [[nodiscard]] constexpr ChessSquare
    get_black_king_location() const noexcept {
        return black_king_location;
    }

    [[nodiscard]] constexpr ChessSquare get_king_location(PieceColor color
    ) const noexcept {
        switch (color) {
            case PieceColor::NONE: __builtin_unreachable();
            case PieceColor::WHITE: return white_king_location;
            case PieceColor::BLACK: return black_king_location;
        }
        __builtin_unreachable();
    }

    [[nodiscard]] constexpr ChessSquare get_enemy_king_location(PieceColor color
    ) const noexcept {
        switch (color) {
            case PieceColor::NONE: __builtin_unreachable();
            case PieceColor::WHITE: return black_king_location;
            case PieceColor::BLACK: return white_king_location;
        }
        __builtin_unreachable();
    }

    [[nodiscard]] constexpr PieceColor get_color_to_move() const noexcept {
        return to_move;
    }

    [[nodiscard]] constexpr bool can_short_castle(PieceColor color
    ) const noexcept {
        switch (color) {
            case PieceColor::NONE: __builtin_unreachable();
            case PieceColor::WHITE:
                return castling_rights.white_can_short_castle();
            case PieceColor::BLACK:
                return castling_rights.black_can_short_castle();
        }
        __builtin_unreachable();
    }

    [[nodiscard]] constexpr bool can_long_castle(PieceColor color
    ) const noexcept {
        switch (color) {
            case PieceColor::NONE: __builtin_unreachable();
            case PieceColor::WHITE:
                return castling_rights.white_can_long_castle();
            case PieceColor::BLACK:
                return castling_rights.black_can_long_castle();
        }
        __builtin_unreachable();
    }

public: // ====================================================== INDEX OPERATOR

    constexpr ChessPiece operator[](ChessSquare square) const noexcept {
        assert(square.in_bounds());
        return board[square];
    }

    constexpr ChessPiece &operator[](ChessSquare square) noexcept {
        assert(square.in_bounds());
        return board[square];
    }

    constexpr ChessPiece operator()(coord_t file, coord_t rank) const noexcept {
        return (*this)[{file, rank}];
    }

    constexpr ChessPiece &operator()(coord_t file, coord_t rank) noexcept {
        return (*this)[{file, rank}];
    }

public: // ========================================================== COMPARISON

    constexpr bool operator==(const ChessPosition &) const noexcept = default;

private: // ===================================================== PAWN UTILITIES

    [[nodiscard]] constexpr ChessSquare en_passant_square() const noexcept {
        switch (to_move) {
            case PieceColor::NONE: __builtin_unreachable();
            case PieceColor::WHITE: return {en_passant_file, NUM_RANKS - 3};
            case PieceColor::BLACK: return {en_passant_file, 2};
        }
        __builtin_unreachable();
    }

private: // ============================================ MOVE VALIDATION HELPERS

#define ensure(cond)                                                           \
    do {                                                                       \
        if (!(cond)) { return false; }                                         \
    } while (false)

    [[nodiscard]] constexpr bool is_valid_castle(ChessMove move
    ) const noexcept {

        // There must be a piece to move;
        ensure(move.in_bounds());
        const ChessPiece piece = (*this)[move.get_src()];
        const PieceType type = piece.get_type();
        const PieceColor color = piece.get_color();
        const PieceColor opponent = (color == PieceColor::WHITE)
                                        ? PieceColor::BLACK
                                        : PieceColor::WHITE;

        // Castling is a special move in which a king travels two squares.
        ensure(type == PieceType::KING);
        ensure(move.distance() == 2);

        // During castling, the king stays on its home rank.
        const coord_t rank = move.get_src_rank();
        ensure(rank == move.get_dst_rank());

        // Castling can only occur if the king is on its home square.
        ensure(move.get_src_file() == 4);
        ensure(
            ((color == PieceColor::WHITE) && (rank == 0)) ||
            ((color == PieceColor::BLACK) && (rank == NUM_RANKS - 1))
        );

        // There must be a friendly rook in the corner that the king moves
        // toward, and all squares between them must be empty. Moreover,
        // both pieces must have never been moved in the current game.
        const ChessPiece FRIENDLY_ROOK = {color, PieceType::ROOK};
        if (move.get_dst_file() == 2) {
            ensure((*this)(0, rank) == FRIENDLY_ROOK);
            ensure((*this)(1, rank) == EMPTY_SQUARE);
            ensure((*this)(2, rank) == EMPTY_SQUARE);
            ensure((*this)(3, rank) == EMPTY_SQUARE);
            ensure(!board.is_attacked_by(opponent, {2, rank}));
            ensure(!board.is_attacked_by(opponent, {3, rank}));
            ensure(!board.is_attacked_by(opponent, {4, rank}));
            ensure(can_long_castle(color));
        } else {
            ensure(move.get_dst_file() == 6);
            ensure((*this)(NUM_FILES - 1, rank) == FRIENDLY_ROOK);
            ensure((*this)(5, rank) == EMPTY_SQUARE);
            ensure((*this)(6, rank) == EMPTY_SQUARE);
            ensure(!board.is_attacked_by(opponent, {4, rank}));
            ensure(!board.is_attacked_by(opponent, {5, rank}));
            ensure(!board.is_attacked_by(opponent, {6, rank}));
            ensure(can_short_castle(color));
        }

        return true;
    }

    [[nodiscard]] constexpr bool is_valid_en_passant(ChessMove move
    ) const noexcept {

        // There must be a piece to move.
        ensure(move.in_bounds());
        const ChessPiece piece = (*this)[move.get_src()];
        const PieceType type = piece.get_type();
        const PieceColor color = piece.get_color();
        ensure(color != PieceColor::NONE);

        // En passant is a special move in which a pawn makes a one-square
        // forward diagonal move to an empty square.
        ensure(type == PieceType::PAWN);
        ensure(
            move.get_dst_rank() ==
            move.get_src_rank() + ChessBoard::pawn_direction(color)
        );
        ensure(move.distance() == 1);
        ensure(move.is_diagonal());
        const ChessPiece target = (*this)[move.get_dst()];
        ensure(target == EMPTY_SQUARE);

        // En passant may only be performed by the player who holds the right
        // to move.
        ensure(color == to_move);

        // En passant may only occur on a specific square.
        ensure(move.get_dst() == en_passant_square());

        // En passant does not capture the piece on the target square.
        // Instead, it captures the piece located at the intersection of
        // the source rank and the destination file.
        const ChessPiece captured =
            (*this)(move.get_dst_file(), move.get_src_rank());

        // En passant may only be used to capture an enemy pawn.
        ensure(captured.get_color() != color);
        ensure(captured.get_type() == PieceType::PAWN);

        return true;
    }

    [[nodiscard]] constexpr bool path_is_clear(ChessMove move) const noexcept {
        const ChessOffset offset = move.direction();
        ChessSquare current = move.get_src() + offset;
        const ChessSquare dst = move.get_dst();
        while (current != dst) {
            if (!board.is_empty(current)) { return false; }
            current += offset;
        }
        return true;
    }

public: // ===================================================== MOVE VALIDATION

    [[nodiscard]] constexpr bool is_valid(ChessMove move) const noexcept {

        // There must be a piece to move.
        ensure(move.in_bounds());
        const ChessPiece piece = (*this)[move.get_src()];
        const PieceType type = piece.get_type();
        const PieceColor color = piece.get_color();
        ensure(color != PieceColor::NONE);

        // A piece can never move to the square it already occupies.
        ensure(move.get_src() != move.get_dst());

        // A piece cannot capture another piece of its own color.
        const ChessPiece target = (*this)[move.get_dst()];
        const PieceColor target_color = target.get_color();
        ensure(color != target_color);

        // Since chess ends at checkmate, kings are never actually captured;
        ensure(target.get_type() != PieceType::KING);

        const bool is_cap =
            (target_color != PieceColor::NONE) || is_valid_en_passant(move);

        switch (type) {
            case PieceType::NONE: return false;
            case PieceType::KING:
                ensure(move.distance() == 1 || is_valid_castle(move));
                ensure(move.get_promotion_type() == PieceType::NONE);
                break;
            case PieceType::QUEEN:
                ensure(move.is_orthogonal() || move.is_diagonal());
                ensure(path_is_clear(move));
                ensure(move.get_promotion_type() == PieceType::NONE);
                break;
            case PieceType::ROOK:
                ensure(move.is_orthogonal());
                ensure(path_is_clear(move));
                ensure(move.get_promotion_type() == PieceType::NONE);
                break;
            case PieceType::BISHOP:
                ensure(move.is_diagonal());
                ensure(path_is_clear(move));
                ensure(move.get_promotion_type() == PieceType::NONE);
                break;
            case PieceType::KNIGHT:
                // Knight moves may be uniquely described as moves of
                // distance 2 which are neither orthogonal nor diagonal.
                ensure(move.distance() == 2);
                ensure(!move.is_orthogonal());
                ensure(!move.is_diagonal());
                ensure(move.get_promotion_type() == PieceType::NONE);
                break;
            case PieceType::PAWN:
                // Pawns move diagonally when and only when they capture.
                ensure(move.is_diagonal() == is_cap);
                const coord_t direction = ChessBoard::pawn_direction(color);
                // Pawns either move one or two squares at a time.
                if (move.get_dst_rank() ==
                    move.get_src_rank() + 2 * direction) {
                    // If a pawn moves two squares, it must move straight ahead.
                    ensure(move.get_src_file() == move.get_dst_file());
                    // Double-step moves are only possible from a pawn's
                    // home rank.
                    ensure(
                        move.get_src_rank() ==
                        ChessBoard::pawn_origin_rank(color)
                    );
                    // Both the square being moved through and the destination
                    // square must be empty.
                    ensure(board.is_empty(
                        move.get_src() + ChessOffset{0, direction}
                    ));
                    ensure(board.is_empty(move.get_dst()));
                } else {
                    ensure(
                        move.get_dst_rank() == move.get_src_rank() + direction
                    );
                    ensure(move.distance() == 1);
                }
                if (move.get_dst_rank() == ChessBoard::promotion_rank(color)) {
                    ensure(
                        move.get_promotion_type() == PieceType::QUEEN ||
                        move.get_promotion_type() == PieceType::ROOK ||
                        move.get_promotion_type() == PieceType::BISHOP ||
                        move.get_promotion_type() == PieceType::KNIGHT
                    );
                } else {
                    ensure(move.get_promotion_type() == PieceType::NONE);
                }
                break;
        }
        return true;
    }

#undef ensure

public: // ====================================================== MOVE EXECUTION

    [[nodiscard]] constexpr bool is_castle(ChessMove move) const noexcept {
        return ((*this)[move.get_src()].get_type() == PieceType::KING) &&
               (move.distance() != 1);
    }

    [[nodiscard]] constexpr bool is_en_passant(ChessMove move) const noexcept {
        return ((*this)[move.get_src()].get_type() == PieceType::PAWN) &&
               move.is_diagonal() && ((*this)[move.get_dst()] == EMPTY_SQUARE);
    }

    [[nodiscard]] constexpr bool is_capture(ChessMove move) const noexcept {
        return ((*this)[move.get_dst()].get_color() != PieceColor::NONE) ||
               is_en_passant(move);
    }

    constexpr void make_move(ChessMove move) noexcept {

        // validate move
        assert(is_valid(move));

        // no move should ever be made that captures a king,
        // since chess ends at checkmate
        assert((*this)[move.get_dst()].get_type() != PieceType::KING);

        // get moving piece
        const ChessPiece piece = (*this)[move.get_src()];
        const PieceColor color = piece.get_color();

        // update castling rights
        if (piece == WHITE_KING) {
            castling_rights.disallow_white_short_castle();
            castling_rights.disallow_white_long_castle();
        }
        if (move.affects({NUM_FILES - 1, 0})) {
            castling_rights.disallow_white_short_castle();
        }
        if (move.affects({0, 0})) {
            castling_rights.disallow_white_long_castle();
        }
        if (piece == BLACK_KING) {
            castling_rights.disallow_black_short_castle();
            castling_rights.disallow_black_long_castle();
        }
        if (move.affects({NUM_FILES - 1, NUM_RANKS - 1})) {
            castling_rights.disallow_black_short_castle();
        }
        if (move.affects({0, NUM_RANKS - 1})) {
            castling_rights.disallow_black_long_castle();
        }

        // perform move
        if (is_en_passant(move)) {
            (*this)(move.get_dst_file(), move.get_src_rank()) = EMPTY_SQUARE;
        } else if (is_castle(move)) {
            const coord_t rank = move.get_src_rank();
            const ChessPiece rook = {color, PieceType::ROOK};
            if (move.get_dst_file() == 6) { // short castle
                (*this)(5, rank) = rook;
                (*this)(7, rank) = EMPTY_SQUARE;
            } else if (move.get_dst_file() == 2) { // long castle
                (*this)(3, rank) = rook;
                (*this)(0, rank) = EMPTY_SQUARE;
            } else {
                __builtin_unreachable();
            }
        }
        (*this)[move.get_dst()] = piece.promote(move.get_promotion_type());
        (*this)[move.get_src()] = EMPTY_SQUARE;

        // update king location
        if (piece == WHITE_KING) {
            white_king_location = move.get_dst();
        } else if (piece == BLACK_KING) {
            black_king_location = move.get_dst();
        }

        // record en passant file
        const coord_t delta_rank = move.get_dst_rank() - move.get_src_rank();
        if ((piece.get_type() == PieceType::PAWN) &&
            ((delta_rank == 2) || (delta_rank == -2))) {
            en_passant_file = move.get_src_file();
        } else {
            en_passant_file = NUM_FILES;
        }

        // update player to move
        switch (color) {
            case PieceColor::NONE: __builtin_unreachable();
            case PieceColor::WHITE: to_move = PieceColor::BLACK; break;
            case PieceColor::BLACK: to_move = PieceColor::WHITE; break;
        }
    }

public: // ======================================================= CHECK TESTING

    [[nodiscard]] constexpr bool in_check(PieceColor color) const noexcept {
        assert(color != PieceColor::NONE);
        switch (color) {
            case PieceColor::NONE: __builtin_unreachable();
            case PieceColor::WHITE:
                return board.is_attacked_by(
                    PieceColor::BLACK, white_king_location
                );
            case PieceColor::BLACK:
                return board.is_attacked_by(
                    PieceColor::WHITE, black_king_location
                );
        }
        __builtin_unreachable();
    }

    [[nodiscard]] constexpr PieceColor get_moving_color(ChessMove move
    ) const noexcept {
        assert(move.in_bounds());
        const PieceColor result = (*this)[move.get_src()].get_color();
        assert(result != PieceColor::NONE);
        return result;
    }

    [[nodiscard]] constexpr bool puts_self_in_check(ChessMove move
    ) const noexcept {
        ChessPosition copy = *this;
        copy.make_move(move);
        return copy.in_check(get_moving_color(move));
    }

    [[nodiscard]] constexpr bool puts_opponent_in_check(ChessMove move
    ) const noexcept {
        ChessPosition copy = *this;
        copy.make_move(move);
        return copy.in_check(copy.get_color_to_move());
    }

    [[nodiscard]] constexpr bool is_legal(ChessMove move) const noexcept {
        return is_valid(move) && !puts_self_in_check(move);
    }

private: // ============================================ MOVE GENERATION HELPERS

    [[nodiscard]] constexpr bool
    is_valid_dst(PieceColor moving_color, ChessSquare square) const noexcept {
        if (!square.in_bounds()) { return false; }
        const ChessPiece target = (*this)[square];
        const PieceColor target_color = target.get_color();
        const PieceType target_type = target.get_type();
        return (target_color != moving_color) &&
               (target_type != PieceType::KING);
    }

    [[nodiscard]] constexpr bool
    is_valid_cap(PieceColor moving_color, ChessSquare square) const noexcept {
        if (!square.in_bounds()) { return false; }
        const ChessPiece target = (*this)[square];
        const PieceColor target_color = target.get_color();
        const PieceType target_type = target.get_type();
        return (target_color != moving_color) &&
               (target_color != PieceColor::NONE) &&
               (target_type != PieceType::KING);
    }

    template <typename F>
    constexpr void visit_leaper_move(
        PieceColor moving_color, ChessSquare src, ChessOffset offset, const F &f
    ) const {
        const ChessSquare dst = src + offset;
        if (is_valid_dst(moving_color, dst)) { f(ChessMove{src, dst}); }
    }

    template <typename F>
    constexpr void visit_slider_moves(
        PieceColor moving_color, ChessSquare src, ChessOffset offset, const F &f
    ) const {
        ChessSquare dst = src + offset;
        while (board.is_empty(dst)) {
            f(ChessMove{src, dst});
            dst += offset;
        }
        if (is_valid_dst(moving_color, dst)) { f(ChessMove{src, dst}); }
    }

    template <typename F>
    constexpr void visit_promotion_moves(
        PieceColor moving_color, ChessSquare src, ChessSquare dst, const F &f
    ) const {
        if (dst.rank == ChessBoard::promotion_rank(moving_color)) {
            f(ChessMove{src, dst, PieceType::QUEEN});
            f(ChessMove{src, dst, PieceType::ROOK});
            f(ChessMove{src, dst, PieceType::BISHOP});
            f(ChessMove{src, dst, PieceType::KNIGHT});
        } else {
            f(ChessMove{src, dst});
        }
    }

    template <typename F>
    constexpr void visit_pawn_moves(
        PieceColor moving_color, ChessSquare src, const F &f
    ) const {
        const coord_t direction = ChessBoard::pawn_direction(moving_color);
        const ChessSquare dst_move = src + ChessOffset{0, direction};
        if (board.is_empty(dst_move)) {
            visit_promotion_moves(moving_color, src, dst_move, f);
            if (src.rank == ChessBoard::pawn_origin_rank(moving_color)) {
                const ChessSquare dst_double_move =
                    dst_move + ChessOffset{0, direction};
                if (board.is_empty(dst_double_move)) {
                    // no promotion possible on initial double-step move
                    f(ChessMove{src, dst_double_move});
                }
            }
        }
        const ChessSquare dst_capture_l = src + ChessOffset{-1, direction};
        if (is_valid_cap(moving_color, dst_capture_l)) {
            visit_promotion_moves(moving_color, src, dst_capture_l, f);
        }
        const ChessSquare dst_capture_r = src + ChessOffset{+1, direction};
        if (is_valid_cap(moving_color, dst_capture_r)) {
            visit_promotion_moves(moving_color, src, dst_capture_r, f);
        }
        if (moving_color == to_move) {
            const ChessSquare dst_ep = en_passant_square();
            if (dst_ep.in_bounds() &&
                (dst_capture_l == dst_ep || dst_capture_r == dst_ep)) {
                // no promotion possible on en passant
                f(ChessMove{src, dst_ep});
            }
        }
    }

    template <typename F>
    constexpr void
    visit_castling_moves(PieceColor moving_color, const F &f) const {
        if (moving_color == PieceColor::WHITE) {
            if (castling_rights.white_can_short_castle()) {
                assert(board(4, 0) == WHITE_KING);
                assert(board(7, 0) == WHITE_ROOK);
                if (board(5, 0) == EMPTY_SQUARE &&
                    board(6, 0) == EMPTY_SQUARE &&
                    !board.is_attacked_by(PieceColor::BLACK, {4, 0}) &&
                    !board.is_attacked_by(PieceColor::BLACK, {5, 0}) &&
                    !board.is_attacked_by(PieceColor::BLACK, {6, 0})) {
                    f(ChessMove{
                        ChessSquare{4, 0},
                        ChessSquare{6, 0}
                    });
                }
            }
            if (castling_rights.white_can_long_castle()) {
                assert(board(0, 0) == WHITE_ROOK);
                assert(board(4, 0) == WHITE_KING);
                if (board(1, 0) == EMPTY_SQUARE &&
                    board(2, 0) == EMPTY_SQUARE &&
                    board(3, 0) == EMPTY_SQUARE &&
                    !board.is_attacked_by(PieceColor::BLACK, {2, 0}) &&
                    !board.is_attacked_by(PieceColor::BLACK, {3, 0}) &&
                    !board.is_attacked_by(PieceColor::BLACK, {4, 0})) {
                    f(ChessMove{
                        ChessSquare{4, 0},
                        ChessSquare{2, 0}
                    });
                }
            }
        } else if (moving_color == PieceColor::BLACK) {
            if (castling_rights.black_can_short_castle()) {
                assert(board(4, 7) == BLACK_KING);
                assert(board(7, 7) == BLACK_ROOK);
                if (board(5, 7) == EMPTY_SQUARE &&
                    board(6, 7) == EMPTY_SQUARE &&
                    !board.is_attacked_by(PieceColor::WHITE, {4, 7}) &&
                    !board.is_attacked_by(PieceColor::WHITE, {5, 7}) &&
                    !board.is_attacked_by(PieceColor::WHITE, {6, 7})) {
                    f(ChessMove{
                        ChessSquare{4, 7},
                        ChessSquare{6, 7}
                    });
                }
            }
            if (castling_rights.black_can_long_castle()) {
                assert(board(0, 7) == BLACK_ROOK);
                assert(board(4, 7) == BLACK_KING);
                if (board(1, 7) == EMPTY_SQUARE &&
                    board(2, 7) == EMPTY_SQUARE &&
                    board(3, 7) == EMPTY_SQUARE &&
                    !board.is_attacked_by(PieceColor::WHITE, {2, 7}) &&
                    !board.is_attacked_by(PieceColor::WHITE, {3, 7}) &&
                    !board.is_attacked_by(PieceColor::WHITE, {4, 7})) {
                    f(ChessMove{
                        ChessSquare{4, 7},
                        ChessSquare{2, 7}
                    });
                }
            }
        } else {
            __builtin_unreachable();
        }
    }

public: // ===================================================== MOVE GENERATION

    template <typename F>
    constexpr void
    visit_moves(PieceColor moving_color, ChessSquare src, const F &f) const {
        assert(src.in_bounds());
        const ChessPiece piece = (*this)[src];
        assert(piece.get_color() == moving_color);
        assert(piece.get_type() != PieceType::NONE);
        switch (piece.get_type()) {
            case PieceType::NONE: __builtin_unreachable();
            case PieceType::KING:
                visit_leaper_move(moving_color, src, {-1, -1}, f);
                visit_leaper_move(moving_color, src, {-1, 0}, f);
                visit_leaper_move(moving_color, src, {-1, +1}, f);
                visit_leaper_move(moving_color, src, {0, -1}, f);
                visit_leaper_move(moving_color, src, {0, +1}, f);
                visit_leaper_move(moving_color, src, {+1, -1}, f);
                visit_leaper_move(moving_color, src, {+1, 0}, f);
                visit_leaper_move(moving_color, src, {+1, +1}, f);
                visit_castling_moves(moving_color, f);
                break;
            case PieceType::QUEEN:
                visit_slider_moves(moving_color, src, {-1, -1}, f);
                visit_slider_moves(moving_color, src, {-1, 0}, f);
                visit_slider_moves(moving_color, src, {-1, +1}, f);
                visit_slider_moves(moving_color, src, {0, -1}, f);
                visit_slider_moves(moving_color, src, {0, +1}, f);
                visit_slider_moves(moving_color, src, {+1, -1}, f);
                visit_slider_moves(moving_color, src, {+1, 0}, f);
                visit_slider_moves(moving_color, src, {+1, +1}, f);
                break;
            case PieceType::ROOK:
                visit_slider_moves(moving_color, src, {-1, 0}, f);
                visit_slider_moves(moving_color, src, {0, -1}, f);
                visit_slider_moves(moving_color, src, {0, +1}, f);
                visit_slider_moves(moving_color, src, {+1, 0}, f);
                break;
            case PieceType::BISHOP:
                visit_slider_moves(moving_color, src, {-1, -1}, f);
                visit_slider_moves(moving_color, src, {-1, +1}, f);
                visit_slider_moves(moving_color, src, {+1, -1}, f);
                visit_slider_moves(moving_color, src, {+1, +1}, f);
                break;
            case PieceType::KNIGHT:
                visit_leaper_move(moving_color, src, {-2, -1}, f);
                visit_leaper_move(moving_color, src, {-2, +1}, f);
                visit_leaper_move(moving_color, src, {-1, -2}, f);
                visit_leaper_move(moving_color, src, {-1, +2}, f);
                visit_leaper_move(moving_color, src, {+1, -2}, f);
                visit_leaper_move(moving_color, src, {+1, +2}, f);
                visit_leaper_move(moving_color, src, {+2, -1}, f);
                visit_leaper_move(moving_color, src, {+2, +1}, f);
                break;
            case PieceType::PAWN: visit_pawn_moves(moving_color, src, f); break;
        }
    }

    [[nodiscard]] std::vector<ChessMove> get_valid_moves(PieceColor color
    ) const noexcept {
        std::vector<ChessMove> result;
        for (coord_t src_file = 0; src_file < NUM_FILES; ++src_file) {
            for (coord_t src_rank = 0; src_rank < NUM_RANKS; ++src_rank) {
                if ((*this)(src_file, src_rank).get_color() == color) {
                    visit_moves(
                        color,
                        {src_file, src_rank},
                        [&](ChessMove move) { result.push_back(move); }
                    );
                }
            }
        }
        return result;
    }

    [[nodiscard]] std::vector<ChessMove> get_legal_moves(PieceColor color
    ) const noexcept {
        std::vector<ChessMove> result;
        for (coord_t src_file = 0; src_file < NUM_FILES; ++src_file) {
            for (coord_t src_rank = 0; src_rank < NUM_RANKS; ++src_rank) {
                if ((*this)(src_file, src_rank).get_color() == color) {
                    visit_moves(
                        color,
                        {src_file, src_rank},
                        [&](ChessMove move) {
                            if (is_legal(move)) { result.push_back(move); }
                        }
                    );
                }
            }
        }
        return result;
    }

    [[nodiscard]] std::vector<ChessMove> get_valid_moves() const noexcept {
        return get_valid_moves(to_move);
    }

    [[nodiscard]] std::vector<ChessMove> get_legal_moves() const noexcept {
        return get_legal_moves(to_move);
    }

    [[nodiscard]] bool check_consistency() const noexcept;

public: // ======================================================== MATE TESTING

    [[nodiscard]] bool checkmated(PieceColor color) const {
        return in_check(color) && get_legal_moves(color).empty();
    }

    [[nodiscard]] bool checkmated() const { return checkmated(to_move); }

    [[nodiscard]] bool stalemated(PieceColor color) const {
        return !in_check(color) && get_legal_moves(color).empty();
    }

    [[nodiscard]] bool stalemated() const { return stalemated(to_move); }

public: // ============================================================ PRINTING

    [[nodiscard]] std::string get_move_name(
        const std::vector<ChessMove> &legal_moves,
        ChessMove move,
        bool suffix = true
    ) const;

    friend std::ostream &operator<<(std::ostream &os, const ChessPosition &b);

public: // ============================================================= FEN I/O

    void load_fen(const std::string &);

    [[nodiscard]] std::string get_fen() const;

public: // ========================================================== EVALUATION

    [[nodiscard]] int get_material_advantage() const;

}; // class ChessPosition


#endif // SUCKER_CHESS_CHESS_POSITION_HPP
