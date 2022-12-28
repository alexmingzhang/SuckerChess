#ifndef SUCKER_CHESS_CHESS_POSITION_HPP
#define SUCKER_CHESS_CHESS_POSITION_HPP

#include <array>   // for std::array
#include <cassert> // for assert
#include <cstddef> // for std::size_t
#include <ostream> // for std::ostream
#include <string>  // for std::string
#include <vector>  // for std::vector

#include "ChessMove.hpp"
#include "ChessPiece.hpp"


class ChessPosition {

    std::array<std::array<ChessPiece, NUM_RANKS>, NUM_FILES> board;
    ChessSquare white_king_location;
    ChessSquare black_king_location;
    PieceColor to_move;
    coord_t en_passant_file;
    bool white_can_short_castle;
    bool white_can_long_castle;
    bool black_can_short_castle;
    bool black_can_long_castle;

public: // ======================================================== CONSTRUCTORS

    explicit constexpr ChessPosition() noexcept
        : board{{{WHITE_ROOK, WHITE_PAWN, EMPTY_SQUARE, EMPTY_SQUARE,
                EMPTY_SQUARE, EMPTY_SQUARE, BLACK_PAWN, BLACK_ROOK},
               {WHITE_KNIGHT, WHITE_PAWN, EMPTY_SQUARE, EMPTY_SQUARE,
                EMPTY_SQUARE, EMPTY_SQUARE, BLACK_PAWN, BLACK_KNIGHT},
               {WHITE_BISHOP, WHITE_PAWN, EMPTY_SQUARE, EMPTY_SQUARE,
                EMPTY_SQUARE, EMPTY_SQUARE, BLACK_PAWN, BLACK_BISHOP},
               {WHITE_QUEEN, WHITE_PAWN, EMPTY_SQUARE, EMPTY_SQUARE,
                EMPTY_SQUARE, EMPTY_SQUARE, BLACK_PAWN, BLACK_QUEEN},
               {WHITE_KING, WHITE_PAWN, EMPTY_SQUARE, EMPTY_SQUARE,
                EMPTY_SQUARE, EMPTY_SQUARE, BLACK_PAWN, BLACK_KING},
               {WHITE_BISHOP, WHITE_PAWN, EMPTY_SQUARE, EMPTY_SQUARE,
                EMPTY_SQUARE, EMPTY_SQUARE, BLACK_PAWN, BLACK_BISHOP},
               {WHITE_KNIGHT, WHITE_PAWN, EMPTY_SQUARE, EMPTY_SQUARE,
                EMPTY_SQUARE, EMPTY_SQUARE, BLACK_PAWN, BLACK_KNIGHT},
               {WHITE_ROOK, WHITE_PAWN, EMPTY_SQUARE, EMPTY_SQUARE,
                EMPTY_SQUARE, EMPTY_SQUARE, BLACK_PAWN, BLACK_ROOK}}}
        , white_king_location({4, 0}), black_king_location({4, NUM_RANKS - 1})
        , to_move(PieceColor::WHITE), en_passant_file(NUM_FILES)
        , white_can_short_castle(true), white_can_long_castle(true)
        , black_can_short_castle(true), black_can_long_castle(true) {}

    explicit ChessPosition(const std::string &fen)
        : board()
        , white_king_location()
        , black_king_location()
        , to_move(PieceColor::NONE)
        , en_passant_file(NUM_FILES)
        , white_can_short_castle(false)
        , white_can_long_castle(false)
        , black_can_short_castle(false)
        , black_can_long_castle(false) {
        load_fen(fen);
    }

public: // =========================================================== ACCESSORS

    [[nodiscard]] constexpr PieceColor get_color_to_move() const noexcept {
        return to_move;
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

    [[nodiscard]] constexpr bool can_short_castle(PieceColor color
    ) const noexcept {
        switch (color) {
            case PieceColor::NONE: __builtin_unreachable();
            case PieceColor::WHITE: return white_can_short_castle;
            case PieceColor::BLACK: return black_can_short_castle;
        }
        __builtin_unreachable();
    }

    [[nodiscard]] constexpr bool can_long_castle(PieceColor color
    ) const noexcept {
        switch (color) {
            case PieceColor::NONE: __builtin_unreachable();
            case PieceColor::WHITE: return white_can_long_castle;
            case PieceColor::BLACK: return black_can_long_castle;
        }
        __builtin_unreachable();
    }

public: // ====================================================== INDEX OPERATOR

    constexpr const ChessPiece &operator[](ChessSquare square) const noexcept {
        assert(square.in_bounds());
        return board[static_cast<std::size_t>(square.file)]
                    [static_cast<std::size_t>(square.rank)];
    }

    constexpr ChessPiece &operator[](ChessSquare square) noexcept {
        assert(square.in_bounds());
        return board[static_cast<std::size_t>(square.file)]
                    [static_cast<std::size_t>(square.rank)];
    }

    constexpr const ChessPiece &
    operator()(coord_t file, coord_t rank) const noexcept {
        return (*this)[{file, rank}];
    }

    constexpr ChessPiece &operator()(coord_t file, coord_t rank) noexcept {
        return (*this)[{file, rank}];
    }

public: // ========================================================== COMPARISON

    constexpr bool operator==(const ChessPosition &) const noexcept = default;

private: // ===================================================== SQUARE TESTING

    [[nodiscard]] constexpr bool is_empty(ChessSquare square) const noexcept {
        return square.in_bounds() && ((*this)[square] == EMPTY_SQUARE);
    }

    [[nodiscard]] constexpr bool
    is_valid_dst(PieceColor moving_color, ChessSquare square) const noexcept {
        return square.in_bounds() &&
               ((*this)[square].get_color() != moving_color);
    }

    [[nodiscard]] constexpr bool
    is_valid_cap(PieceColor moving_color, ChessSquare square) const noexcept {
        if (!square.in_bounds()) { return false; }
        const ChessPiece target = (*this)[square];
        const PieceColor target_color = target.get_color();
        return (target_color != moving_color) &&
               (target_color != PieceColor::NONE);
    }

    [[nodiscard]] constexpr bool has_enemy_piece(
        PieceColor moving_color, ChessSquare square, PieceType type
    ) const noexcept {
        if (!square.in_bounds()) { return false; }
        const ChessPiece target = (*this)[square];
        const PieceColor target_color = target.get_color();
        return (target_color != moving_color) &&
               (target_color != PieceColor::NONE) &&
               (target.get_type() == type);
    }

private: // ===================================================== PAWN UTILITIES

    [[nodiscard]] static constexpr coord_t pawn_direction(PieceColor color
    ) noexcept {
        switch (color) {
            case PieceColor::NONE: __builtin_unreachable();
            case PieceColor::WHITE: return +1;
            case PieceColor::BLACK: return -1;
        }
        __builtin_unreachable();
    }

    [[nodiscard]] static constexpr coord_t pawn_origin_rank(PieceColor color
    ) noexcept {
        switch (color) {
            case PieceColor::NONE: __builtin_unreachable();
            case PieceColor::WHITE: return 1;
            case PieceColor::BLACK: return NUM_RANKS - 2;
        }
        __builtin_unreachable();
    }

    [[nodiscard]] static constexpr coord_t promotion_rank(PieceColor color
    ) noexcept {
        switch (color) {
            case PieceColor::NONE: __builtin_unreachable();
            case PieceColor::WHITE: return NUM_RANKS - 1;
            case PieceColor::BLACK: return 0;
        }
        __builtin_unreachable();
    }

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
    } while (false);

    [[nodiscard]] constexpr bool is_valid_castle(ChessMove move
    ) const noexcept {
        ensure(move.in_bounds());
        const ChessPiece piece = (*this)[move.get_src()];
        const PieceColor color = piece.get_color();
        ensure(color != PieceColor::NONE);

        // The only time a king may travel more than one space in a single move
        // is through castling.
        if ((piece.get_type() == PieceType::KING) && (move.distance() != 1)) {

            // During a castle, the king moves exactly two spaces left or right
            // and stays on its home rank.
            ensure(move.distance() == 2);
            const coord_t rank = move.get_src_rank();
            ensure(rank == move.get_dst_rank());

            // Castling can only take place if the king is on its home square.
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
                ensure(is_empty({1, rank}));
                ensure(is_empty({2, rank}));
                ensure(is_empty({3, rank}));
                ensure(can_long_castle(color));
            } else {
                ensure(move.get_dst_file() == 6);
                ensure((*this)(NUM_FILES - 1, rank) == FRIENDLY_ROOK);
                ensure(is_empty({5, rank}));
                ensure(is_empty({6, rank}));
                ensure(can_short_castle(color));
            }
            return true;
        }
        return false;
    }

    [[nodiscard]] constexpr bool is_valid_en_passant(ChessMove move
    ) const noexcept {
        ensure(move.in_bounds());
        const ChessPiece piece = (*this)[move.get_src()];
        const PieceColor color = piece.get_color();
        ensure(color != PieceColor::NONE);

        // En passant is a special move in which a pawn makes a diagonal move
        // to an empty square.
        const ChessPiece target = (*this)[move.get_dst()];
        if ((piece.get_type() == PieceType::PAWN) && move.is_diagonal() &&
            (target == EMPTY_SQUARE)) {

            // En passant is a diagonal move of distance 1.
            ensure(move.distance() == 1);
            ensure(move.get_dst_file() != move.get_src_file());
            ensure(
                move.get_dst_rank() ==
                move.get_src_rank() + pawn_direction(color)
            );

            // En passant may only be performed by the player who holds
            // the right to move.
            ensure(color == to_move);

            // En passant may only occur on a specific square.
            ensure(move.get_dst() == en_passant_square());

            // En passant does not capture the piece on the target square.
            // Instead, it captures the piece located on the source rank
            // and destination file.
            const ChessPiece captured =
                (*this)(move.get_dst_file(), move.get_src_rank());

            // En passant may only be used to capture an enemy pawn.
            ensure(captured.get_color() != color);
            ensure(captured.get_type() == PieceType::PAWN);

            return true;
        }
        return false;
    }

public: // ===================================================== MOVE VALIDATION

    [[nodiscard]] constexpr bool is_valid(ChessMove move) const noexcept {
        ensure(move.in_bounds());
        const ChessPiece piece = (*this)[move.get_src()];
        const PieceColor color = piece.get_color();
        const ChessPiece target = (*this)[move.get_dst()];
        const PieceColor target_color = target.get_color();

        // Pieces cannot capture other pieces of their own color.
        ensure(color != PieceColor::NONE);
        ensure(color != target_color);
        const bool is_capture =
            (target_color != PieceColor::NONE) || is_valid_en_passant(move);

        switch (piece.get_type()) {
            case PieceType::NONE: return false;
            case PieceType::KING:
                ensure(move.distance() == 1 || is_valid_castle(move));
                break;
            case PieceType::QUEEN:
                ensure(move.is_orthogonal() || move.is_diagonal());
                break;
            case PieceType::ROOK: ensure(move.is_orthogonal()); break;
            case PieceType::BISHOP: ensure(move.is_diagonal()); break;
            case PieceType::KNIGHT:
                // Knight moves may be uniquely described as moves of
                // distance 2 which are neither orthogonal nor diagonal.
                ensure(move.distance() == 2);
                ensure(!move.is_orthogonal());
                ensure(!move.is_diagonal());
                break;
            case PieceType::PAWN:
                // Pawns move diagonally when and only when they capture.
                ensure(move.is_diagonal() == is_capture);
                const coord_t direction = pawn_direction(color);
                // Pawns either move one or two squares at a time.
                if (move.get_dst_rank() ==
                    move.get_src_rank() + 2 * direction) {
                    // If a pawn moves two squares, it must move straight ahead.
                    ensure(move.get_src_file() == move.get_dst_file());
                    // Double-step moves are only possible from a pawn's
                    // home rank.
                    ensure(move.get_src_rank() == pawn_origin_rank(color));
                    // Both the square being moved through and the destination
                    // square must be empty.
                    ensure(is_empty(move.get_src() + ChessOffset{0, direction})
                    );
                    ensure(is_empty(move.get_dst()));
                } else {
                    ensure(
                        move.get_dst_rank() == move.get_src_rank() + direction
                    );
                    ensure(move.distance() == 1);
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
        assert(is_valid(move));

        // get moving piece
        const ChessPiece piece = (*this)[move.get_src()];
        const PieceColor color = piece.get_color();

        // update castling rights
        if (piece == WHITE_KING) {
            white_can_short_castle = false;
            white_can_long_castle = false;
        }
        if (move.affects({NUM_FILES - 1, 0})) {
            white_can_short_castle = false;
        }
        if (move.affects({0, 0})) { white_can_long_castle = false; }
        if (piece == BLACK_KING) {
            black_can_short_castle = false;
            black_can_long_castle = false;
        }
        if (move.affects({NUM_FILES - 1, NUM_RANKS - 1})) {
            black_can_short_castle = false;
        }
        if (move.affects({0, NUM_RANKS - 1})) { black_can_long_castle = false; }

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

private: // ============================================= ATTACK TESTING HELPERS

    [[nodiscard]] constexpr ChessPiece
    get_slider_attacker(ChessSquare square, ChessOffset offset) const noexcept {
        ChessSquare current = square + offset;
        while (is_empty(current)) { current += offset; }
        if (current.in_bounds()) {
            const ChessPiece piece = (*this)[current];
            if (piece.get_color() != to_move) { return piece; }
        }
        return EMPTY_SQUARE;
    }

    [[nodiscard]] constexpr bool is_attacked_by_king(ChessSquare square
    ) const noexcept {
        using enum PieceType;
        return has_enemy_piece(to_move, square + ChessOffset{-1, -1}, KING) ||
               has_enemy_piece(to_move, square + ChessOffset{-1, 0}, KING) ||
               has_enemy_piece(to_move, square + ChessOffset{-1, +1}, KING) ||
               has_enemy_piece(to_move, square + ChessOffset{0, -1}, KING) ||
               has_enemy_piece(to_move, square + ChessOffset{0, +1}, KING) ||
               has_enemy_piece(to_move, square + ChessOffset{+1, -1}, KING) ||
               has_enemy_piece(to_move, square + ChessOffset{+1, 0}, KING) ||
               has_enemy_piece(to_move, square + ChessOffset{+1, +1}, KING);
    }

    [[nodiscard]] constexpr bool is_attacked_orthogonally(ChessSquare square
    ) const noexcept {
        using enum PieceType;
        const ChessPiece a = get_slider_attacker(square, ChessOffset{-1, 0});
        if (a.get_type() == QUEEN || a.get_type() == ROOK) { return true; }
        const ChessPiece b = get_slider_attacker(square, ChessOffset{0, -1});
        if (b.get_type() == QUEEN || b.get_type() == ROOK) { return true; }
        const ChessPiece c = get_slider_attacker(square, ChessOffset{0, +1});
        if (c.get_type() == QUEEN || c.get_type() == ROOK) { return true; }
        const ChessPiece d = get_slider_attacker(square, ChessOffset{+1, 0});
        if (d.get_type() == QUEEN || d.get_type() == ROOK) { return true; }
        return false;
    }

    [[nodiscard]] constexpr bool is_attacked_diagonally(ChessSquare square
    ) const noexcept {
        using enum PieceType;
        const ChessPiece a = get_slider_attacker(square, ChessOffset{-1, -1});
        if (a.get_type() == QUEEN || a.get_type() == BISHOP) { return true; }
        const ChessPiece b = get_slider_attacker(square, ChessOffset{-1, +1});
        if (b.get_type() == QUEEN || b.get_type() == BISHOP) { return true; }
        const ChessPiece c = get_slider_attacker(square, ChessOffset{+1, -1});
        if (c.get_type() == QUEEN || c.get_type() == BISHOP) { return true; }
        const ChessPiece d = get_slider_attacker(square, ChessOffset{+1, +1});
        if (d.get_type() == QUEEN || d.get_type() == BISHOP) { return true; }
        return false;
    }

    [[nodiscard]] constexpr bool is_attacked_by_knight(ChessSquare square
    ) const noexcept {
        using enum PieceType;
        return has_enemy_piece(to_move, square + ChessOffset{-2, -1}, KNIGHT) ||
               has_enemy_piece(to_move, square + ChessOffset{-2, +1}, KNIGHT) ||
               has_enemy_piece(to_move, square + ChessOffset{-1, -2}, KNIGHT) ||
               has_enemy_piece(to_move, square + ChessOffset{-1, +2}, KNIGHT) ||
               has_enemy_piece(to_move, square + ChessOffset{+1, -2}, KNIGHT) ||
               has_enemy_piece(to_move, square + ChessOffset{+1, +2}, KNIGHT) ||
               has_enemy_piece(to_move, square + ChessOffset{+2, -1}, KNIGHT) ||
               has_enemy_piece(to_move, square + ChessOffset{+2, +1}, KNIGHT);
    }

    [[nodiscard]] constexpr bool is_attacked_by_pawn(ChessSquare square
    ) const noexcept {
        using enum PieceType;
        const coord_t direction = pawn_direction(to_move);
        return has_enemy_piece(
                   to_move, square + ChessOffset{-1, direction}, PAWN
               ) ||
               has_enemy_piece(
                   to_move, square + ChessOffset{+1, direction}, PAWN
               );
    }

public: // ====================================================== ATTACK TESTING

    [[nodiscard]] constexpr bool is_attacked(ChessSquare square
    ) const noexcept {
        assert(square.in_bounds());
        return is_attacked_by_king(square) ||
               is_attacked_orthogonally(square) ||
               is_attacked_diagonally(square) ||
               is_attacked_by_knight(square) || is_attacked_by_pawn(square);
    }

    [[nodiscard]] constexpr bool in_check() const noexcept {
        if (to_move == PieceColor::WHITE) {
            return is_attacked(white_king_location);
        } else if (to_move == PieceColor::BLACK) {
            return is_attacked(black_king_location);
        } else {
            __builtin_unreachable();
        }
    }

    [[nodiscard]] constexpr bool puts_self_in_check(ChessMove move
    ) const noexcept {
        ChessPosition copy = *this;
        copy.make_move(move);
        copy.to_move = to_move;
        return copy.in_check();
    }

    [[nodiscard]] constexpr bool is_legal(ChessMove move) const noexcept {
        return is_valid(move) && !puts_self_in_check(move);
    }

public: // ===================================================== MOVE GENERATION

    constexpr void push_if_legal(std::vector<ChessMove> &moves, ChessMove move)
        const noexcept {
        assert(is_valid(move));
        if (is_legal(move)) { moves.push_back(move); }
    }

    constexpr void push_leaper_move(
        std::vector<ChessMove> &moves, ChessSquare src, ChessOffset offset
    ) const noexcept {
        const ChessSquare dst = src + offset;
        if (is_valid_dst(to_move, dst)) { push_if_legal(moves, {src, dst}); }
    }

    constexpr void push_slider_moves(
        std::vector<ChessMove> &moves, ChessSquare src, ChessOffset offset
    ) const noexcept {
        ChessSquare dst = src + offset;
        while (is_empty(dst)) {
            push_if_legal(moves, {src, dst});
            dst += offset;
        }
        if (is_valid_dst(to_move, dst)) { push_if_legal(moves, {src, dst}); }
    }

    constexpr void push_promotion_moves(
        std::vector<ChessMove> &moves, ChessSquare src, ChessSquare dst
    ) const noexcept {
        using enum PieceType;
        if (dst.rank == promotion_rank(to_move)) {
            push_if_legal(moves, {src, dst, QUEEN});
            push_if_legal(moves, {src, dst, ROOK});
            push_if_legal(moves, {src, dst, BISHOP});
            push_if_legal(moves, {src, dst, KNIGHT});
        } else {
            push_if_legal(moves, {src, dst});
        }
    }

    constexpr void push_pawn_moves(
        std::vector<ChessMove> &moves, ChessSquare src
    ) const noexcept {
        const coord_t direction = pawn_direction(to_move);
        const ChessSquare dst_move = src + ChessOffset{0, direction};
        if (is_empty(dst_move)) {
            push_promotion_moves(moves, src, dst_move);
            if (src.rank == pawn_origin_rank(to_move)) {
                const ChessSquare dst_double_move =
                    dst_move + ChessOffset{0, direction};
                if (is_empty(dst_double_move)) {
                    // no promotion possible on initial double-step move
                    push_if_legal(moves, {src, dst_double_move});
                }
            }
        }
        const ChessSquare dst_capture_l = src + ChessOffset{-1, direction};
        if (is_valid_cap(to_move, dst_capture_l)) {
            push_promotion_moves(moves, src, dst_capture_l);
        }
        const ChessSquare dst_capture_r = src + ChessOffset{+1, direction};
        if (is_valid_cap(to_move, dst_capture_r)) {
            push_promotion_moves(moves, src, dst_capture_r);
        }
        const ChessSquare dst_ep = en_passant_square();
        if (dst_ep.in_bounds() &&
            (dst_capture_l == dst_ep || dst_capture_r == dst_ep)) {
            push_if_legal(moves, {src, dst_ep});
        }
    }

    constexpr void push_castling_moves(std::vector<ChessMove> &moves
    ) const noexcept {
        if (to_move == PieceColor::WHITE) {
            if (white_can_short_castle) {
                assert(board[4][0] == WHITE_KING);
                assert(board[7][0] == WHITE_ROOK);
                if (board[5][0] == EMPTY_SQUARE &&
                    board[6][0] == EMPTY_SQUARE && !is_attacked({4, 0}) &&
                    !is_attacked({5, 0}) && !is_attacked({6, 0})) {
                    moves.emplace_back(ChessSquare{4, 0}, ChessSquare{6, 0});
                }
            }
            if (white_can_long_castle) {
                assert(board[0][0] == WHITE_ROOK);
                assert(board[4][0] == WHITE_KING);
                if (board[1][0] == EMPTY_SQUARE &&
                    board[2][0] == EMPTY_SQUARE &&
                    board[3][0] == EMPTY_SQUARE && !is_attacked({2, 0}) &&
                    !is_attacked({3, 0}) && !is_attacked({4, 0})) {
                    moves.emplace_back(ChessSquare{4, 0}, ChessSquare{2, 0});
                }
            }
        } else if (to_move == PieceColor::BLACK) {
            if (black_can_short_castle) {
                assert(board[4][7] == BLACK_KING);
                assert(board[7][7] == BLACK_ROOK);
                if (board[5][7] == EMPTY_SQUARE &&
                    board[6][7] == EMPTY_SQUARE && !is_attacked({4, 7}) &&
                    !is_attacked({5, 7}) && !is_attacked({6, 7})) {
                    moves.emplace_back(ChessSquare{4, 7}, ChessSquare{6, 7});
                }
            }
            if (black_can_long_castle) {
                assert(board[0][7] == BLACK_ROOK);
                assert(board[4][7] == BLACK_KING);
                if (board[1][7] == EMPTY_SQUARE &&
                    board[2][7] == EMPTY_SQUARE &&
                    board[3][7] == EMPTY_SQUARE && !is_attacked({2, 7}) &&
                    !is_attacked({3, 7}) && !is_attacked({4, 7})) {
                    moves.emplace_back(ChessSquare{4, 7}, ChessSquare{2, 7});
                }
            }
        } else {
            __builtin_unreachable();
        }
    }

    constexpr void push_legal_moves(
        std::vector<ChessMove> &moves, ChessSquare src
    ) const noexcept {
        assert(src.in_bounds());
        const ChessPiece piece = (*this)[src];
        assert(piece.get_color() == to_move);
        assert(piece.get_type() != PieceType::NONE);
        switch (piece.get_type()) {
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

    [[nodiscard]] std::vector<ChessMove> get_legal_moves() const noexcept {
        std::vector<ChessMove> result;
        for (coord_t src_file = 0; src_file < NUM_FILES; ++src_file) {
            for (coord_t src_rank = 0; src_rank < NUM_RANKS; ++src_rank) {
                const ChessPiece &piece = (*this)(src_file, src_rank);
                if (piece.get_color() == to_move) {
                    push_legal_moves(result, {src_file, src_rank});
                }
            }
        }
        return result;
    }

public: // ======================================================= CHECK TESTING

    [[nodiscard]] bool checkmated() const {
        return in_check() && get_legal_moves().empty();
    }

    [[nodiscard]] bool stalemated() const {
        return !in_check() && get_legal_moves().empty();
    }

public: // ============================================================ PRINTING

    [[nodiscard]] std::string get_move_name(
        const std::vector<ChessMove> &legal_moves,
        ChessMove move,
        bool suffix = true
    ) const;

    friend std::ostream &operator<<(std::ostream &os, const ChessPosition &b);

public: // ================================================================ MISC

    void load_fen(const std::string &);

    [[nodiscard]] std::string get_fen() const;

    [[nodiscard]] int get_material_advantage() const;

}; // class ChessPosition


#endif // SUCKER_CHESS_CHESS_POSITION_HPP
