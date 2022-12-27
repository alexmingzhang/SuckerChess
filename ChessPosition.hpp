#ifndef SUCKER_CHESS_CHESS_POSITION_HPP
#define SUCKER_CHESS_CHESS_POSITION_HPP

#include <array>   // for std::array
#include <cstddef> // for std::size_t
#include <ostream> // for std::ostream
#include <string>  // for std::string
#include <vector>  // for std::vector

#include "ChessMove.hpp"
#include "ChessPiece.hpp"


class ChessPosition {

    std::array<std::array<ChessPiece, NUM_RANKS>, NUM_FILES> board;
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
                EMPTY_SQUARE, EMPTY_SQUARE, BLACK_PAWN, BLACK_ROOK}}},
          to_move(PieceColor::WHITE), en_passant_file(NUM_FILES),
          white_can_short_castle(true), white_can_long_castle(true),
          black_can_short_castle(true), black_can_long_castle(true) {}

    explicit ChessPosition(const std::string &fen)
        : board()
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

    [[nodiscard]] constexpr bool is_legal_dst(ChessSquare square
    ) const noexcept {
        return square.in_bounds() && (*this)[square].get_color() != to_move;
    }

    [[nodiscard]] constexpr bool is_legal_cap(ChessSquare square
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

    [[nodiscard]] constexpr coord_t pawn_direction() const {
        switch (to_move) {
            case PieceColor::NONE: __builtin_unreachable();
            case PieceColor::WHITE: return +1;
            case PieceColor::BLACK: return -1;
        }
        __builtin_unreachable();
    }

    [[nodiscard]] constexpr coord_t pawn_origin_rank() const {
        switch (to_move) {
            case PieceColor::NONE: __builtin_unreachable();
            case PieceColor::WHITE: return 1;
            case PieceColor::BLACK: return NUM_RANKS - 2;
        }
        __builtin_unreachable();
    }

    [[nodiscard]] constexpr coord_t promotion_rank() const {
        switch (to_move) {
            case PieceColor::NONE: __builtin_unreachable();
            case PieceColor::WHITE: return NUM_RANKS - 1;
            case PieceColor::BLACK: return 0;
        }
        __builtin_unreachable();
    }

    [[nodiscard]] constexpr ChessSquare en_passant_square() const {
        switch (to_move) {
            case PieceColor::NONE: __builtin_unreachable();
            case PieceColor::WHITE: return {en_passant_file, NUM_RANKS - 3};
            case PieceColor::BLACK: return {en_passant_file, 2};
        }
        __builtin_unreachable();
    }

public: // ===================================================== MOVE VALIDATION

    [[nodiscard]] constexpr bool is_castle(const ChessMove &move
    ) const noexcept {
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
                const ChessPiece rook = (*this)(0, rank);
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
                const ChessPiece rook = (*this)(NUM_FILES - 1, rank);
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

    [[nodiscard]] constexpr bool is_en_passant(const ChessMove &move
    ) const noexcept {
        assert(move.get_src().in_bounds());
        assert(move.get_dst().in_bounds());
        const ChessPiece piece = (*this)[move.get_src()];
        const PieceColor color = piece.get_color();
        assert(color != PieceColor::NONE);

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
                    const ChessPiece ep_captured =
                        (*this)(move.get_dst_file(), move.get_src_rank());
                    assert(ep_captured.get_color() != piece.get_color());
                    assert(ep_captured.get_type() == PieceType::PAWN);

                    return true;
                }
            }
        }
        return false;
    }

    [[nodiscard]] constexpr bool is_capture(const ChessMove &move
    ) const noexcept {
        assert(move.get_src().in_bounds());
        assert(move.get_dst().in_bounds());
        const ChessPiece piece = (*this)[move.get_src()];
        const ChessPiece target = (*this)[move.get_dst()];

        // Pieces cannot capture other pieces of their own color.
        assert(piece.get_color() != target.get_color());

        // A capture is a move that either lands on an enemy piece or is
        // en passant.
        return target.get_color() != PieceColor::NONE || is_en_passant(move);
    }

    [[nodiscard]] constexpr bool is_valid(const ChessMove &move
    ) const noexcept {
        assert(move.get_src().in_bounds());
        assert(move.get_dst().in_bounds());
        const ChessPiece piece = (*this)[move.get_src()];
        const ChessPiece target = (*this)[move.get_dst()];

        // Pieces cannot capture other pieces of their own color.
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

private: // ============================================= ATTACK TESTING HELPERS

    [[nodiscard]] constexpr ChessPiece
    get_slider_attacker(ChessSquare square, ChessOffset offset) const {
        ChessSquare current = square + offset;
        while (is_empty(current)) { current += offset; }
        if (current.in_bounds()) {
            const ChessPiece piece = (*this)[current];
            if (piece.get_color() != to_move) { return piece; }
        }
        return EMPTY_SQUARE;
    }

    [[nodiscard]] constexpr bool is_attacked_by_king(ChessSquare square) const {
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
    ) const {
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
    ) const {
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
    ) const {
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

    [[nodiscard]] constexpr bool is_attacked_by_pawn(ChessSquare square) const {
        using enum PieceType;
        const coord_t direction = pawn_direction();
        return has_enemy_piece(square + ChessOffset{-1, direction}, PAWN) ||
               has_enemy_piece(square + ChessOffset{+1, direction}, PAWN);
    }

public: // ====================================================== ATTACK TESTING

    [[nodiscard]] constexpr bool is_attacked(ChessSquare square) const {
        assert(square.in_bounds());
        return is_attacked_by_king(square) ||
               is_attacked_orthogonally(square) ||
               is_attacked_diagonally(square) ||
               is_attacked_by_knight(square) || is_attacked_by_pawn(square);
    }

    [[nodiscard]] constexpr bool in_check() const {
        for (coord_t file = 0; file < NUM_FILES; ++file) {
            for (coord_t rank = 0; rank < NUM_RANKS; ++rank) {
                const ChessPiece piece = (*this)(file, rank);
                if (piece.get_type() == PieceType::KING &&
                    piece.get_color() == to_move) {
                    return is_attacked({file, rank});
                }
            }
        }
        return false;
    }

public: // ===================================================== MOVE GENERATION

    void push_leaper_move(
        std::vector<ChessMove> &moves, ChessSquare src, ChessOffset offset
    ) const;

    void push_slider_moves(
        std::vector<ChessMove> &moves, ChessSquare src, ChessOffset offset
    ) const;

    void push_promotion_moves(
        std::vector<ChessMove> &moves, ChessSquare src, ChessSquare dst
    ) const;

    void push_pawn_moves(std::vector<ChessMove> &moves, ChessSquare src) const;

    void push_castling_moves(std::vector<ChessMove> &moves) const;

    void push_all_moves(std::vector<ChessMove> &moves, ChessSquare src) const;

    [[nodiscard]] std::vector<ChessMove> get_all_moves() const;

public: // ====================================================== MOVE EXECUTION

    void make_move(const ChessMove &move);

public: // ======================================================= CHECK TESTING

    [[nodiscard]] bool puts_self_in_check(const ChessMove &move) const {
        ChessPosition copy = *this;
        copy.make_move(move);
        copy.to_move = to_move;
        return copy.in_check();
    }

    [[nodiscard]] std::vector<ChessMove> get_legal_moves() const {
        const std::vector<ChessMove> all_moves = get_all_moves();
        std::vector<ChessMove> result;
        for (const ChessMove &move : all_moves) {
            if (!puts_self_in_check(move)) { result.push_back(move); }
        }
        return result;
    }

    [[nodiscard]] bool checkmated() const {
        return in_check() && get_legal_moves().empty();
    }

    [[nodiscard]] bool stalemated() const {
        return !in_check() && get_legal_moves().empty();
    }

public: // ============================================================ PRINTING

    [[nodiscard]] std::string get_move_name(
        const std::vector<ChessMove> &legal_moves, const ChessMove &move
    ) const;

    friend std::ostream &operator<<(std::ostream &os, const ChessPosition &b);

public: // ================================================================ MISC

    void load_fen(const std::string &);

    std::string get_fen() const;

    int get_material_advantage() const;

}; // class ChessPosition


#endif // SUCKER_CHESS_CHESS_POSITION_HPP
