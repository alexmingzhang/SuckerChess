#ifndef SUCKER_CHESS_CHESS_GAME_HPP
#define SUCKER_CHESS_CHESS_GAME_HPP

#include <cstdint>  // for std::uint8_t
#include <optional> // for std::optional, std::nullopt
#include <string>   // for std::string
#include <vector>   // for std::vector

#include "ChessEngine.hpp"
#include "ChessMove.hpp"
#include "ChessPiece.hpp"
#include "ChessPosition.hpp"


class ChessPositionWrapper {

    ChessPosition position;
    mutable std::optional<std::vector<ChessMove>> cached_legal_moves;

public: // ========================================================= CONSTRUCTOR

    explicit constexpr ChessPositionWrapper() noexcept
        : position()
        , cached_legal_moves(std::nullopt) {}

public: // =========================================================== ACCESSORS

    constexpr const ChessPosition &get() const noexcept { return position; }

    [[nodiscard]] constexpr PieceColor get_color_to_move() const noexcept {
        return position.get_color_to_move();
    }

    const std::vector<ChessMove> &get_legal_moves() const noexcept {
        if (!cached_legal_moves.has_value()) {
            cached_legal_moves.emplace(position.get_legal_moves());
        }
        return *cached_legal_moves;
    }

public: // ========================================================== COMPARISON

    constexpr bool operator==(const ChessPosition &other) const noexcept {
        return (position == other);
    }

public: // ======================================================== MOVE TESTING

    constexpr bool is_capture_or_pawn_move(ChessMove move) const noexcept {
        return position.is_capture_or_pawn_move(move);
    }

public: // ====================================================== MOVE EXECUTION

    void make_move(ChessMove move) noexcept {
        position.make_move(move);
        cached_legal_moves.reset();
    }

public: // ======================================================== MATE TESTING

    bool checkmated() const noexcept {
        return get_legal_moves().empty() && position.in_check();
    }

    bool stalemated() const noexcept {
        return get_legal_moves().empty() && !position.in_check();
    }

    bool has_insufficient_material() const noexcept {
        return position.get_board().has_insufficient_material();
    }

public: // ========================================================= MOVE NAMING

    std::string get_move_name(ChessMove move, bool suffix) {
        return position.get_move_name(get_legal_moves(), move, suffix);
    }

public: // ============================================================= FEN I/O

    std::string get_fen() { return position.get_fen(); }

    void load_fen(const std::string &fen) {
        position.load_fen(fen);
        cached_legal_moves.reset();
    }

}; // class ChessPositionWrapper


enum class GameStatus : std::uint8_t {
    IN_PROGRESS,
    WHITE_WON_BY_CHECKMATE,
    BLACK_WON_BY_CHECKMATE,
    DRAWN_BY_STALEMATE,
    DRAWN_BY_INSUFFICIENT_MATERIAL,
    DRAWN_BY_REPETITION,
    DRAWN_BY_50_MOVE_RULE,
}; // enum class GameStatus


class ChessGame {

public:

    ChessPositionWrapper current_pos;
    GameStatus current_status;
    std::vector<ChessPosition> pos_history;
    std::vector<ChessMove> move_history;
    int half_move_clock;
    int full_move_count;

    explicit ChessGame() noexcept
        : current_pos()
        , current_status(GameStatus::IN_PROGRESS)
        , pos_history()
        , move_history()
        , half_move_clock(0)
        , full_move_count(1) {}

    [[nodiscard]] GameStatus get_status() const;

    void make_move(const ChessMove &move);

    ChessMove get_console_move();

    PieceColor run(ChessEngine *white, ChessEngine *black, bool verbose = true);

    [[nodiscard]] std::string get_PGN() const;

    [[nodiscard]] std::string get_full_PGN(
        const std::string &event_name,
        int num_round,
        const std::string &white_name,
        const std::string &black_name
    ) const;

private:

    [[nodiscard]] std::string get_PGN_result() const;
    [[nodiscard]] std::string get_PGN_move_text() const;

}; // class ChessGame


#endif // SUCKER_CHESS_CHESS_GAME_HPP
