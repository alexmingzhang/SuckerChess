#ifndef SUCKER_CHESS_CHESS_GAME_HPP
#define SUCKER_CHESS_CHESS_GAME_HPP

#include <cstdint> // for std::uint8_t
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

    [[nodiscard]] constexpr int get_half_move_clock() const noexcept {
        return m_half_move_clock;
    }

    [[nodiscard]] constexpr int get_full_move_count() const noexcept {
        return m_full_move_count;
    }

private: // ====================================================================

    GameStatus compute_current_status() noexcept;

public: // =====================================================================

    void make_move(ChessMove move) noexcept;

public: // =====================================================================

    PieceColor run(ChessEngine *white, ChessEngine *black, bool verbose = true);

}; // class ChessGame


#endif // SUCKER_CHESS_CHESS_GAME_HPP
