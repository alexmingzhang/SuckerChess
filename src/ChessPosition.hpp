#ifndef SUCKER_CHESS_CHESS_POSITION_HPP
#define SUCKER_CHESS_CHESS_POSITION_HPP

#include <cassert> // for assert
#include <cstdint> // for std::uint8_t
#include <ostream> // for std::ostream
#include <string>  // for std::string
#include <vector>  // for std::vector

#include "CastlingRights.hpp"
#include "ChessBoard.hpp"
#include "ChessMove.hpp"
#include "ChessPiece.hpp"


class ChessPosition final {

    ChessBoard board;
    std::uint8_t move_data; // TO_MOVE, EN_PASSANT_AVAILABLE, EN_PASSANT_FILE
    CastlingRights castling_rights;

#ifdef SUCKER_CHESS_TRACK_KING_LOCATIONS
    std::uint8_t white_king_location_data;
    std::uint8_t black_king_location_data;
#endif

public: // ======================================================== CONSTRUCTORS

    explicit constexpr ChessPosition() noexcept
        : board()
        , move_data(0)
        , castling_rights(true, true, true, true)
#ifdef SUCKER_CHESS_TRACK_KING_LOCATIONS
        , white_king_location_data(0x40)
        , black_king_location_data(0x47)
#endif
    {
    }

    explicit ChessPosition(const std::string &fen)
        : board()
        , move_data(0)
        , castling_rights(false, false, false, false)
#ifdef SUCKER_CHESS_TRACK_KING_LOCATIONS
        , white_king_location_data(0)
        , black_king_location_data(0)
#endif
    {
        load_fen(fen);
    }

public: // =========================================================== ACCESSORS

    [[nodiscard]] constexpr const ChessBoard &get_board() const noexcept {
        return board;
    }

    [[nodiscard]] constexpr PieceColor get_color_to_move() const noexcept {
        return (move_data & 0x10) ? PieceColor::BLACK : PieceColor::WHITE;
    }

    [[nodiscard]] constexpr CastlingRights
    get_castling_rights() const noexcept {
        return castling_rights;
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

#ifdef SUCKER_CHESS_TRACK_KING_LOCATIONS

    [[nodiscard]] constexpr ChessSquare
    get_white_king_location() const noexcept {
        const coord_t file = white_king_location_data >> 4;
        const coord_t rank = white_king_location_data & 0x0F;
        const ChessSquare result = {file, rank};
        assert(result.in_bounds());
        return result;
    }

    [[nodiscard]] constexpr ChessSquare
    get_black_king_location() const noexcept {
        const coord_t file = black_king_location_data >> 4;
        const coord_t rank = black_king_location_data & 0x0F;
        const ChessSquare result = {file, rank};
        assert(result.in_bounds());
        return result;
    }

#else

    [[nodiscard]] constexpr ChessSquare
    get_white_king_location() const noexcept {
        return board.find_first_piece(WHITE_KING);
    }

    [[nodiscard]] constexpr ChessSquare
    get_black_king_location() const noexcept {
        return board.find_first_piece(BLACK_KING);
    }

#endif

    [[nodiscard]] constexpr ChessSquare get_king_location(PieceColor color
    ) const noexcept {
        switch (color) {
            case PieceColor::NONE: __builtin_unreachable();
            case PieceColor::WHITE: return get_white_king_location();
            case PieceColor::BLACK: return get_black_king_location();
        }
        __builtin_unreachable();
    }

    [[nodiscard]] constexpr ChessSquare get_enemy_king_location(PieceColor color
    ) const noexcept {
        switch (color) {
            case PieceColor::NONE: __builtin_unreachable();
            case PieceColor::WHITE: return get_black_king_location();
            case PieceColor::BLACK: return get_white_king_location();
        }
        __builtin_unreachable();
    }

    [[nodiscard]] constexpr ChessSquare get_king_location() const noexcept {
        return get_king_location(get_color_to_move());
    }

    [[nodiscard]] constexpr ChessSquare
    get_enemy_king_location() const noexcept {
        return get_enemy_king_location(get_color_to_move());
    }

public: // ========================================================== COMPARISON

    constexpr bool operator==(const ChessPosition &other
    ) const noexcept = default;

private: // ===================================================== PAWN UTILITIES

    [[nodiscard]] constexpr bool is_en_passant_available() const noexcept {
        return ((move_data & 0x08) != 0);
    }

    [[nodiscard]] constexpr ChessSquare en_passant_square() const noexcept {
        assert(is_en_passant_available());
        const coord_t en_passant_file = move_data & 0x07;
        switch (get_color_to_move()) {
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
        const ChessPiece piece = board.get_piece(move.get_src());
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
            ensure(board.get_piece(0, rank) == FRIENDLY_ROOK);
            ensure(board.get_piece(1, rank) == EMPTY_SQUARE);
            ensure(board.get_piece(2, rank) == EMPTY_SQUARE);
            ensure(board.get_piece(3, rank) == EMPTY_SQUARE);
            ensure(!board.is_attacked_by(opponent, {2, rank}));
            ensure(!board.is_attacked_by(opponent, {3, rank}));
            ensure(!board.is_attacked_by(opponent, {4, rank}));
            ensure(can_long_castle(color));
        } else {
            ensure(move.get_dst_file() == 6);
            ensure(board.get_piece(NUM_FILES - 1, rank) == FRIENDLY_ROOK);
            ensure(board.get_piece(5, rank) == EMPTY_SQUARE);
            ensure(board.get_piece(6, rank) == EMPTY_SQUARE);
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
        const ChessPiece piece = board.get_piece(move.get_src());
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
        const ChessPiece target = board.get_piece(move.get_dst());
        ensure(target == EMPTY_SQUARE);

        // En passant may only be performed by the player who holds the right
        // to move, when available.
        ensure(color == get_color_to_move());
        ensure(is_en_passant_available());

        // En passant may only occur on a specific square.
        ensure(move.get_dst() == en_passant_square());

        // En passant does not capture the piece on the target square.
        // Instead, it captures the piece located at the intersection of
        // the source rank and the destination file.
        const ChessPiece captured =
            board.get_piece(move.get_dst_file(), move.get_src_rank());

        // En passant may only be used to capture an enemy pawn.
        ensure(captured.get_color() != color);
        ensure(captured.get_type() == PieceType::PAWN);

        return true;
    }

    [[nodiscard]] constexpr bool path_is_clear(ChessMove move) const noexcept {
        const coord_t file_offset =
            (move.get_dst_file() < move.get_src_file())   ? -1
            : (move.get_dst_file() > move.get_src_file()) ? +1
                                                          : 0;
        const coord_t rank_offset =
            (move.get_dst_rank() < move.get_src_rank())   ? -1
            : (move.get_dst_rank() > move.get_src_rank()) ? +1
                                                          : 0;
        ChessSquare current = move.get_src().shift(file_offset, rank_offset);
        const ChessSquare dst = move.get_dst();
        while (current != dst) {
            if (!board.in_bounds_and_empty(current)) { return false; }
            current = current.shift(file_offset, rank_offset);
        }
        return true;
    }

public: // ===================================================== MOVE VALIDATION

    [[nodiscard]] constexpr bool is_valid(ChessMove move) const noexcept {

        // There must be a piece to move.
        ensure(move.in_bounds());
        const ChessPiece piece = board.get_piece(move.get_src());
        const PieceType type = piece.get_type();
        const PieceColor color = piece.get_color();
        ensure(color != PieceColor::NONE);

        // A piece can never move to the square it already occupies.
        ensure(move.get_src() != move.get_dst());

        // A piece cannot capture another piece of its own color.
        const ChessPiece target = board.get_piece(move.get_dst());
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
                    ensure(board.in_bounds_and_empty(
                        move.get_src().shift(0, direction)
                    ));
                    ensure(board.in_bounds_and_empty(move.get_dst()));
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
        return (board.get_piece(move.get_src()).get_type() == PieceType::KING
               ) &&
               (move.distance() != 1);
    }

    [[nodiscard]] constexpr bool is_en_passant(ChessMove move) const noexcept {
        return (board.get_piece(move.get_src()).get_type() == PieceType::PAWN
               ) &&
               move.is_diagonal() &&
               (board.get_piece(move.get_dst()) == EMPTY_SQUARE);
    }

    [[nodiscard]] constexpr bool is_capture(ChessMove move) const noexcept {
        return (board.get_piece(move.get_dst()).get_color() != PieceColor::NONE
               ) ||
               is_en_passant(move);
    }

    [[nodiscard]] constexpr bool is_capture_or_pawn_move(ChessMove move
    ) const noexcept {
        const ChessPiece piece = board.get_piece(move.get_src());
        const ChessPiece target = board.get_piece(move.get_dst());
        return (piece.get_type() == PieceType::PAWN) ||
               (target.get_color() != PieceColor::NONE);
    }

    constexpr void make_move(ChessMove move) noexcept {

        // validate move
        assert(is_valid(move));

        // no move should ever be made that captures a king,
        // since chess ends at checkmate
        assert(board.get_piece(move.get_dst()).get_type() != PieceType::KING);

        // get moving piece
        const ChessPiece piece = board.get_piece(move.get_src());
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
            board.set_piece(
                move.get_dst_file(), move.get_src_rank(), EMPTY_SQUARE
            );
        } else if (is_castle(move)) {
            const coord_t rank = move.get_src_rank();
            const ChessPiece rook = {color, PieceType::ROOK};
            if (move.get_dst_file() == 6) { // short castle
                board.set_piece(5, rank, rook);
                board.set_piece(7, rank, EMPTY_SQUARE);
            } else if (move.get_dst_file() == 2) { // long castle
                board.set_piece(3, rank, rook);
                board.set_piece(0, rank, EMPTY_SQUARE);
            } else {
                __builtin_unreachable();
            }
        }
        board.set_piece(
            move.get_dst(), piece.promote(move.get_promotion_type())
        );
        board.set_piece(move.get_src(), EMPTY_SQUARE);

#ifdef SUCKER_CHESS_TRACK_KING_LOCATIONS
        // update king location
        if (piece == WHITE_KING) {
            white_king_location_data = static_cast<std::uint8_t>(
                (move.get_dst_file() << 4) | move.get_dst_rank()
            );
        } else if (piece == BLACK_KING) {
            black_king_location_data = static_cast<std::uint8_t>(
                (move.get_dst_file() << 4) | move.get_dst_rank()
            );
        }
#endif

        // update player to move and clear previous en passant data
        switch (color) {
            case PieceColor::NONE: __builtin_unreachable();
            case PieceColor::WHITE: move_data = 0x10; break;
            case PieceColor::BLACK: move_data = 0x00; break;
        }

        // if applicable, update en passant data
        const coord_t delta_rank = move.get_dst_rank() - move.get_src_rank();
        if ((piece.get_type() == PieceType::PAWN) &&
            ((delta_rank == 2) || (delta_rank == -2))) {
            assert(move.get_src_file() == move.get_dst_file());
            move_data |= 0x08;
            move_data |= move.get_src_file();
        }
    }

public: // ======================================================= CHECK TESTING

    [[nodiscard]] constexpr bool in_check(PieceColor color) const noexcept {
        assert(color != PieceColor::NONE);
        switch (color) {
            case PieceColor::NONE: __builtin_unreachable();
            case PieceColor::WHITE:
                return board.is_attacked_by(
                    PieceColor::BLACK, get_white_king_location()
                );
            case PieceColor::BLACK:
                return board.is_attacked_by(
                    PieceColor::WHITE, get_black_king_location()
                );
        }
        __builtin_unreachable();
    }

    [[nodiscard]] constexpr bool in_check() const noexcept {
        return in_check(get_color_to_move());
    }

    [[nodiscard]] constexpr PieceColor get_moving_color(ChessMove move
    ) const noexcept {
        assert(move.in_bounds());
        const PieceColor result = board.get_piece(move.get_src()).get_color();
        assert(result != PieceColor::NONE);
        return result;
    }

private: // ============================================ MOVE GENERATION HELPERS

    template <typename F>
    constexpr void visit_leaper_move(
        PieceColor moving_color,
        ChessSquare src,
        coord_t file_offset,
        coord_t rank_offset,
        const F &f
    ) const {
        const ChessSquare dst = src.shift(file_offset, rank_offset);
        if (board.in_bounds_and_is_valid_dst(moving_color, dst)) {
            f(ChessMove{src, dst});
        }
    }

    template <typename F>
    constexpr void visit_slider_moves(
        PieceColor moving_color,
        ChessSquare src,
        coord_t file_offset,
        coord_t rank_offset,
        const F &f
    ) const {
        ChessSquare dst = src.shift(file_offset, rank_offset);
        while (board.in_bounds_and_empty(dst)) {
            f(ChessMove{src, dst});
            dst = dst.shift(file_offset, rank_offset);
        }
        if (board.in_bounds_and_is_valid_dst(moving_color, dst)) {
            f(ChessMove{src, dst});
        }
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
        const ChessSquare dst_move = src.shift(0, direction);
        if (board.in_bounds_and_empty(dst_move)) {
            visit_promotion_moves(moving_color, src, dst_move, f);
            if (src.rank == ChessBoard::pawn_origin_rank(moving_color)) {
                const ChessSquare dst_double = dst_move.shift(0, direction);
                if (board.in_bounds_and_empty(dst_double)) {
                    // no promotion possible on initial double-step move
                    f(ChessMove{src, dst_double});
                }
            }
        }
        const ChessSquare dst_capture_l = src.shift(-1, direction);
        if (board.in_bounds_and_is_valid_cap(moving_color, dst_capture_l)) {
            visit_promotion_moves(moving_color, src, dst_capture_l, f);
        }
        const ChessSquare dst_capture_r = src.shift(+1, direction);
        if (board.in_bounds_and_is_valid_cap(moving_color, dst_capture_r)) {
            visit_promotion_moves(moving_color, src, dst_capture_r, f);
        }
        if (is_en_passant_available() &&
            (moving_color == get_color_to_move())) {
            const ChessSquare dst_ep = en_passant_square();
            if ((dst_capture_l == dst_ep) || (dst_capture_r == dst_ep)) {
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
                assert(board.get_piece(4, 0) == WHITE_KING);
                assert(board.get_piece(7, 0) == WHITE_ROOK);
                if (board.get_piece(5, 0) == EMPTY_SQUARE &&
                    board.get_piece(6, 0) == EMPTY_SQUARE &&
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
                assert(board.get_piece(0, 0) == WHITE_ROOK);
                assert(board.get_piece(4, 0) == WHITE_KING);
                if (board.get_piece(1, 0) == EMPTY_SQUARE &&
                    board.get_piece(2, 0) == EMPTY_SQUARE &&
                    board.get_piece(3, 0) == EMPTY_SQUARE &&
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
                assert(board.get_piece(4, 7) == BLACK_KING);
                assert(board.get_piece(7, 7) == BLACK_ROOK);
                if (board.get_piece(5, 7) == EMPTY_SQUARE &&
                    board.get_piece(6, 7) == EMPTY_SQUARE &&
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
                assert(board.get_piece(0, 7) == BLACK_ROOK);
                assert(board.get_piece(4, 7) == BLACK_KING);
                if (board.get_piece(1, 7) == EMPTY_SQUARE &&
                    board.get_piece(2, 7) == EMPTY_SQUARE &&
                    board.get_piece(3, 7) == EMPTY_SQUARE &&
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
    constexpr void visit_valid_moves(
        PieceColor moving_color, ChessSquare src, const F &f
    ) const {
        assert(src.in_bounds());
        const ChessPiece piece = board.get_piece(src);
        assert(piece.get_color() == moving_color);
        assert(piece.get_type() != PieceType::NONE);
        switch (piece.get_type()) {
            case PieceType::NONE: __builtin_unreachable();
            case PieceType::KING:
                visit_leaper_move(moving_color, src, -1, -1, f);
                visit_leaper_move(moving_color, src, -1, 0, f);
                visit_leaper_move(moving_color, src, -1, +1, f);
                visit_leaper_move(moving_color, src, 0, -1, f);
                visit_leaper_move(moving_color, src, 0, +1, f);
                visit_leaper_move(moving_color, src, +1, -1, f);
                visit_leaper_move(moving_color, src, +1, 0, f);
                visit_leaper_move(moving_color, src, +1, +1, f);
                visit_castling_moves(moving_color, f);
                break;
            case PieceType::QUEEN:
                visit_slider_moves(moving_color, src, -1, -1, f);
                visit_slider_moves(moving_color, src, -1, 0, f);
                visit_slider_moves(moving_color, src, -1, +1, f);
                visit_slider_moves(moving_color, src, 0, -1, f);
                visit_slider_moves(moving_color, src, 0, +1, f);
                visit_slider_moves(moving_color, src, +1, -1, f);
                visit_slider_moves(moving_color, src, +1, 0, f);
                visit_slider_moves(moving_color, src, +1, +1, f);
                break;
            case PieceType::ROOK:
                visit_slider_moves(moving_color, src, -1, 0, f);
                visit_slider_moves(moving_color, src, 0, -1, f);
                visit_slider_moves(moving_color, src, 0, +1, f);
                visit_slider_moves(moving_color, src, +1, 0, f);
                break;
            case PieceType::BISHOP:
                visit_slider_moves(moving_color, src, -1, -1, f);
                visit_slider_moves(moving_color, src, -1, +1, f);
                visit_slider_moves(moving_color, src, +1, -1, f);
                visit_slider_moves(moving_color, src, +1, +1, f);
                break;
            case PieceType::KNIGHT:
                visit_leaper_move(moving_color, src, -2, -1, f);
                visit_leaper_move(moving_color, src, -2, +1, f);
                visit_leaper_move(moving_color, src, -1, -2, f);
                visit_leaper_move(moving_color, src, -1, +2, f);
                visit_leaper_move(moving_color, src, +1, -2, f);
                visit_leaper_move(moving_color, src, +1, +2, f);
                visit_leaper_move(moving_color, src, +2, -1, f);
                visit_leaper_move(moving_color, src, +2, +1, f);
                break;
            case PieceType::PAWN: visit_pawn_moves(moving_color, src, f); break;
        }
    }

    template <typename F>
    constexpr void
    visit_valid_moves(PieceColor moving_color, const F &f) const {
        for (coord_t src_file = 0; src_file < NUM_FILES; ++src_file) {
            for (coord_t src_rank = 0; src_rank < NUM_RANKS; ++src_rank) {
                const ChessSquare src = {src_file, src_rank};
                if (board.get_piece(src).get_color() == moving_color) {
                    visit_valid_moves(moving_color, src, f);
                }
            }
        }
    }

    template <typename F>
    constexpr void visit_valid_moves(const F &f) const {
        visit_valid_moves(get_color_to_move(), f);
    }

    template <typename F>
    constexpr void
    visit_legal_moves(PieceColor moving_color, const F &f) const {
        visit_valid_moves(moving_color, [&](ChessMove move) {
            assert(is_valid(move));
            assert(get_moving_color(move) == moving_color);
            ChessPosition next = *this;
            next.make_move(move);
            if (!next.in_check(moving_color)) { f(move, next); }
        });
    }

    template <typename F>
    constexpr void visit_legal_moves(const F &f) const {
        visit_legal_moves(get_color_to_move(), f);
    }

    [[nodiscard]] std::vector<ChessMove> get_legal_moves(PieceColor moving_color
    ) const noexcept {
        std::vector<ChessMove> result;
        visit_legal_moves(
            moving_color,
            [&](ChessMove move, const ChessPosition &) {
                result.push_back(move);
            }
        );
        return result;
    }

    [[nodiscard]] std::vector<ChessMove> get_legal_moves() const noexcept {
        return get_legal_moves(get_color_to_move());
    }

    [[nodiscard]] bool check_consistency() const noexcept;

public: // ======================================================== MATE TESTING

    [[nodiscard]] bool checkmated(PieceColor color) const {
        return in_check(color) && get_legal_moves(color).empty();
    }

    [[nodiscard]] bool checkmated() const {
        return checkmated(get_color_to_move());
    }

    [[nodiscard]] bool stalemated(PieceColor color) const {
        return !in_check(color) && get_legal_moves(color).empty();
    }

    [[nodiscard]] bool stalemated() const {
        return stalemated(get_color_to_move());
    }

public: // ============================================================ PRINTING

    [[nodiscard]] std::string get_move_name(
        const std::vector<ChessMove> &legal_moves,
        ChessMove move,
        bool suffix = true
    ) const;

    friend std::ostream &operator<<(std::ostream &os, const ChessPosition &b);

public: // ============================================================= FEN I/O

    void load_fen(const std::string &);

    [[nodiscard]] std::string get_fen() const noexcept;

}; // class ChessPosition


#endif // SUCKER_CHESS_CHESS_POSITION_HPP
