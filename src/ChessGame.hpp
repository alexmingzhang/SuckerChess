#ifndef SUCKER_CHESS_CHESS_GAME_HPP
#define SUCKER_CHESS_CHESS_GAME_HPP

#include <cstdint> // for std::uint8_t
#include <string>  // for std::string
#include <vector>  // for std::vector

#include "ChessEngine.hpp"
#include "ChessMove.hpp"
#include "ChessPiece.hpp"
#include "ChessPosition.hpp"


enum class GameStatus : std::uint8_t {
    IN_PROGRESS,
    WHITE_WON_BY_CHECKMATE,
    BLACK_WON_BY_CHECKMATE,
    DRAWN_BY_STALEMATE,
    DRAWN_BY_INSUFFICIENT_MATERIAL,
    DRAWN_BY_REPETITION,
    DRAWN_BY_50_MOVE_RULE,
}; // enum class GameStatus


class ChessGame final {

    ChessEngineInterface m_interface;
    GameStatus m_status;
    std::vector<ChessPosition> m_pos_history;
    std::vector<ChessMove> m_move_history;
    int m_half_move_clock;
    int m_full_move_count;

public: // ========================================================= CONSTRUCTOR

    explicit ChessGame() noexcept;

public: // =========================================================== ACCESSORS

    [[nodiscard]] constexpr GameStatus get_current_status() const noexcept {
        return m_status;
    }

    [[nodiscard]] constexpr const std::vector<ChessPosition> &
    get_pos_history() const noexcept {
        return m_pos_history;
    }

    [[nodiscard]] constexpr const std::vector<ChessMove> &
    get_move_history() const noexcept {
        return m_move_history;
    }

    [[nodiscard]] constexpr int get_half_move_clock() const noexcept {
        return m_half_move_clock;
    }

    [[nodiscard]] constexpr int get_full_move_count() const noexcept {
        return m_full_move_count;
    }

private: // ================================================== EXECUTION HELPERS

    GameStatus compute_current_status() noexcept;

    ChessMove get_console_move();

public: // =========================================================== EXECUTION

    void make_move(ChessMove move) noexcept;

    PieceColor run(ChessEngine *white, ChessEngine *black, bool verbose = true);

public: // ====================================================== FEN/PGN EXPORT

    std::string get_fen();

    std::string get_pgn(
        const std::string &event_name = "",
        long long num_round = -1,
        const std::string &white_name = "",
        const std::string &black_name = ""
    );

}; // class ChessGame


#endif // SUCKER_CHESS_CHESS_GAME_HPP
