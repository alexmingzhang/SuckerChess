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

    [[nodiscard]] constexpr ChessSquare get_king_location() const noexcept {
        switch (to_move) {
            case PieceColor::NONE: __builtin_unreachable();
            case PieceColor::WHITE: return white_king_location;
            case PieceColor::BLACK: return black_king_location;
        }
    }

    [[nodiscard]] constexpr ChessSquare
    get_enemy_king_location() const noexcept {
        switch (to_move) {
            case PieceColor::NONE: __builtin_unreachable();
            case PieceColor::WHITE: return black_king_location;
            case PieceColor::BLACK: return white_king_location;
        }
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

public: // ====================================================== SQUARE TESTING

    [[nodiscard]] constexpr bool is_empty(ChessSquare square) const noexcept {
        return square.in_bounds() && (*this)[square] == EMPTY_SQUARE;
    }

    [[nodiscard]] constexpr bool is_valid_dst(ChessSquare square
    ) const noexcept {
        return square.in_bounds() && (*this)[square].get_color() != to_move;
    }

    [[nodiscard]] constexpr bool is_valid_cap(ChessSquare square
    ) const noexcept {
        if (!square.in_bounds()) { return false; }
        const ChessPiece piece = (*this)[square];
        return piece.get_color() != to_move &&
               piece.get_color() != PieceColor::NONE;
    }

    [[nodiscard]] constexpr bool
    has_enemy_piece(ChessSquare square, PieceType type) const noexcept {
        if (!square.in_bounds()) { return false; }
        const ChessPiece piece = (*this)[square];
        return piece.get_color() != to_move &&
               piece.get_color() != PieceColor::NONE &&
               piece.get_type() == type;
    }

private: // ========================================================== UTILITIES

    [[nodiscard]] constexpr coord_t pawn_direction() const noexcept {
        switch (to_move) {
            case PieceColor::NONE: __builtin_unreachable();
            case PieceColor::WHITE: return +1;
            case PieceColor::BLACK: return -1;
        }
        __builtin_unreachable();
    }

    [[nodiscard]] constexpr coord_t pawn_origin_rank() const noexcept {
        switch (to_move) {
            case PieceColor::NONE: __builtin_unreachable();
            case PieceColor::WHITE: return 1;
            case PieceColor::BLACK: return NUM_RANKS - 2;
        }
        __builtin_unreachable();
    }

    [[nodiscard]] constexpr coord_t promotion_rank() const noexcept {
        switch (to_move) {
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

public: // ===================================================== MOVE VALIDATION

    [[nodiscard]] constexpr bool is_castle(ChessMove move) const noexcept {
        assert(move.get_src().in_bounds());
        assert(move.get_dst().in_bounds());
        const ChessPiece piece = (*this)[move.get_src()];
        const PieceColor color = piece.get_color();
        assert(color != PieceColor::NONE);

        // The only time a king may travel more than one space in a single move
        // is through castling.
        if (piece.get_type() == PieceType::KING && move.distance() != 1) {

            // During a castle, the king moves exactly two spaces left or right
            // and stays on its home rank.
            assert(move.distance() == 2);
            const coord_t rank = move.get_src_rank();
            assert(rank == move.get_dst_rank());

            // Castling can only take place if the king is on its home square.
            assert(move.get_src_file() == 4);
            assert(
                color == PieceColor::WHITE && rank == 0 ||
                color == PieceColor::BLACK && rank == NUM_RANKS - 1
            );

            // There must be a friendly rook in the corner that the king moves
            // toward, and all squares between them must be empty. Moreover,
            // both pieces must have never been moved in the current game.
            if (move.get_dst_file() == 2) {
                [[maybe_unused]] const ChessPiece rook = (*this)(0, rank);
                assert(rook.get_color() == color);
                assert(rook.get_type() == PieceType::ROOK);
                assert(is_empty({1, rank}));
                assert(is_empty({2, rank}));
                assert(is_empty({3, rank}));
                if (color == PieceColor::WHITE) {
                    assert(white_can_long_castle);
                } else {
                    assert(color == PieceColor::BLACK);
                    assert(black_can_long_castle);
                }
            } else {
                assert(move.get_dst_file() == 6);
                [[maybe_unused]] const ChessPiece rook =
                    (*this)(NUM_FILES - 1, rank);
                assert(rook.get_color() == color);
                assert(rook.get_type() == PieceType::ROOK);
                assert(is_empty({5, rank}));
                assert(is_empty({6, rank}));
                if (color == PieceColor::WHITE) {
                    assert(white_can_short_castle);
                } else {
                    assert(color == PieceColor::BLACK);
                    assert(black_can_short_castle);
                }
            }
            return true;
        }
        return false;
    }

    [[nodiscard]] constexpr bool is_en_passant(ChessMove move) const noexcept {
        assert(move.get_src().in_bounds());
        assert(move.get_dst().in_bounds());
        const ChessPiece piece = (*this)[move.get_src()];
        assert(piece.get_color() != PieceColor::NONE);

        // En passant is a special move in which a pawn makes a diagonal move
        // to an empty square.
        if (piece.get_type() == PieceType::PAWN) {

            // Pawns may only move one or two spaces at a time.
            assert(move.distance() == 1 || move.distance() == 2);

            // If moving diagonally, a pawn can only move one space at a time.
            if (move.is_diagonal()) {
                assert(move.distance() == 1);

                // If the destination square is empty, then this move is an
                // en passant capture.
                const ChessPiece captured = (*this)[move.get_dst()];
                if (captured == EMPTY_SQUARE) {

                    // En passant may only occur on a specific square.
                    assert(move.get_dst() == en_passant_square());

                    // En passant may only be used to capture an enemy pawn.
                    [[maybe_unused]] const ChessPiece ep_captured =
                        (*this)(move.get_dst_file(), move.get_src_rank());
                    assert(ep_captured.get_color() != piece.get_color());
                    assert(ep_captured.get_type() == PieceType::PAWN);

                    return true;
                }
            }
        }
        return false;
    }

    [[nodiscard]] constexpr bool is_capture(ChessMove move) const noexcept {
        assert(move.get_src().in_bounds());
        assert(move.get_dst().in_bounds());
        [[maybe_unused]] const ChessPiece piece = (*this)[move.get_src()];
        const ChessPiece target = (*this)[move.get_dst()];

        // Pieces cannot capture other pieces of their own color.
        assert(piece.get_color() != PieceColor::NONE);
        assert(piece.get_color() != target.get_color());

        // A capture is a move that either lands on an enemy piece or is
        // en passant.
        return target.get_color() != PieceColor::NONE || is_en_passant(move);
    }

    [[nodiscard]] constexpr bool is_valid(ChessMove move) const noexcept {
        assert(move.get_src().in_bounds());
        assert(move.get_dst().in_bounds());
        const ChessPiece piece = (*this)[move.get_src()];
        [[maybe_unused]] const ChessPiece target = (*this)[move.get_dst()];

        // Pieces cannot capture other pieces of their own color.
        assert(piece.get_color() != PieceColor::NONE);
        assert(piece.get_color() != target.get_color());

        switch (piece.get_type()) {
            case PieceType::NONE: return false;
            case PieceType::KING:
                assert(move.distance() == 1 || is_castle(move));
                break;
            case PieceType::QUEEN:
                assert(move.is_orthogonal() || move.is_diagonal());
                break;
            case PieceType::ROOK: assert(move.is_orthogonal()); break;
            case PieceType::BISHOP: assert(move.is_diagonal()); break;
            case PieceType::KNIGHT:
                // Knight moves may be uniquely described as moves of
                // distance 2 which are neither orthogonal nor diagonal.
                assert(move.distance() == 2);
                assert(!move.is_orthogonal());
                assert(!move.is_diagonal());
                break;
            case PieceType::PAWN:
                // Pawns never move horizontally; they always change rank.
                assert(move.get_src_rank() != move.get_dst_rank());
                // Pawns move either one or two spaces at a time.
                // If they move two spaces, they must move straight ahead.
                // TODO: Validate empty squares.
                assert(
                    move.distance() == 1 ||
                    (move.distance() == 2 &&
                     move.get_src_file() == move.get_dst_file())
                );
                // Pawns move diagonally when and only when they capture.
                assert(move.is_diagonal() == is_capture(move));
                // TODO: Validate pawn direction.
                break;
        }
        return true;
    }

public: // ====================================================== MOVE EXECUTION

    constexpr void make_move(ChessMove move) noexcept {
        assert(is_valid(move));

        // get moving piece
        const ChessPiece piece = (*this)[move.get_src()];

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
        } else if (piece.get_type() == PieceType::KING && move.distance() != 1) {
            const coord_t rank = move.get_src_rank();
            if (move.get_dst_file() == 6) { // short castle
                (*this)(5, rank) = (*this)(7, rank);
                (*this)(7, rank) = EMPTY_SQUARE;
            } else if (move.get_dst_file() == 2) { // long castle
                (*this)(3, rank) = (*this)(0, rank);
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
        if (piece.get_type() == PieceType::PAWN &&
            (move.get_src_rank() - move.get_dst_rank() == 2 ||
             move.get_src_rank() - move.get_dst_rank() == -2)) {
            en_passant_file = move.get_src_file();
        } else {
            en_passant_file = NUM_FILES;
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
        return has_enemy_piece(square + ChessOffset{-1, -1}, KING) ||
               has_enemy_piece(square + ChessOffset{-1, 0}, KING) ||
               has_enemy_piece(square + ChessOffset{-1, +1}, KING) ||
               has_enemy_piece(square + ChessOffset{0, -1}, KING) ||
               has_enemy_piece(square + ChessOffset{0, +1}, KING) ||
               has_enemy_piece(square + ChessOffset{+1, -1}, KING) ||
               has_enemy_piece(square + ChessOffset{+1, 0}, KING) ||
               has_enemy_piece(square + ChessOffset{+1, +1}, KING);
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
        return has_enemy_piece(square + ChessOffset{-2, -1}, KNIGHT) ||
               has_enemy_piece(square + ChessOffset{-2, +1}, KNIGHT) ||
               has_enemy_piece(square + ChessOffset{-1, -2}, KNIGHT) ||
               has_enemy_piece(square + ChessOffset{-1, +2}, KNIGHT) ||
               has_enemy_piece(square + ChessOffset{+1, -2}, KNIGHT) ||
               has_enemy_piece(square + ChessOffset{+1, +2}, KNIGHT) ||
               has_enemy_piece(square + ChessOffset{+2, -1}, KNIGHT) ||
               has_enemy_piece(square + ChessOffset{+2, +1}, KNIGHT);
    }

    [[nodiscard]] constexpr bool is_attacked_by_pawn(ChessSquare square
    ) const noexcept {
        using enum PieceType;
        const coord_t direction = pawn_direction();
        return has_enemy_piece(square + ChessOffset{-1, direction}, PAWN) ||
               has_enemy_piece(square + ChessOffset{+1, direction}, PAWN);
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
        if (is_valid_dst(dst)) { push_if_legal(moves, {src, dst}); }
    }

    constexpr void push_slider_moves(
        std::vector<ChessMove> &moves, ChessSquare src, ChessOffset offset
    ) const noexcept {
        ChessSquare dst = src + offset;
        while (is_empty(dst)) {
            push_if_legal(moves, {src, dst});
            dst += offset;
        }
        if (is_valid_dst(dst)) { push_if_legal(moves, {src, dst}); }
    }

    constexpr void push_promotion_moves(
        std::vector<ChessMove> &moves, ChessSquare src, ChessSquare dst
    ) const noexcept {
        using enum PieceType;
        if (dst.rank == promotion_rank()) {
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
        const coord_t direction = pawn_direction();
        const ChessSquare dst_move = src + ChessOffset{0, direction};
        if (is_empty(dst_move)) {
            push_promotion_moves(moves, src, dst_move);
            if (src.rank == pawn_origin_rank()) {
                const ChessSquare dst_double_move =
                    dst_move + ChessOffset{0, direction};
                if (is_empty(dst_double_move)) {
                    // no promotion possible on initial double-step move
                    push_if_legal(moves, {src, dst_double_move});
                }
            }
        }
        const ChessSquare dst_capture_l = src + ChessOffset{-1, direction};
        if (is_valid_cap(dst_capture_l)) {
            push_promotion_moves(moves, src, dst_capture_l);
        }
        const ChessSquare dst_capture_r = src + ChessOffset{+1, direction};
        if (is_valid_cap(dst_capture_r)) {
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
        const std::vector<ChessMove> &legal_moves, ChessMove move
    ) const;

    friend std::ostream &operator<<(std::ostream &os, const ChessPosition &b);

public: // ================================================================ MISC

    void load_fen(const std::string &);

    std::string get_fen() const;

    int get_material_advantage() const;

}; // class ChessPosition


#endif // SUCKER_CHESS_CHESS_POSITION_HPP
