#ifndef SUCKER_CHESS_CHESS_GAME_HPP
#define SUCKER_CHESS_CHESS_GAME_HPP

#include <cstdint> // for std::uint8_t
#include <string>  // for std::string
#include <vector>  // for std::vector

#include "ChessEngine.hpp"
#include "ChessPosition.hpp"
#include "src/ChessMove.hpp"
#include "src/ChessPiece.hpp"


enum class GameStatus : std::uint8_t {
    IN_PROGRESS,
    WHITE_WON_BY_CHECKMATE,
    BLACK_WON_BY_CHECKMATE,
    DRAWN_BY_STALEMATE,
    DRAWN_BY_REPETITION,
    DRAWN_BY_50_MOVE_RULE,
}; // enum class GameStatus


class ChessGame {

public:

    ChessPosition current_pos;
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

    void make_move(const ChessMove &move);

    [[nodiscard]] bool drawn() const;

    ChessMove get_console_move(const std::vector<ChessMove> &legal_moves);

    PieceColor run(ChessEngine *white, ChessEngine *black, bool verbose = true);

    std::string get_PGN() const;

    std::string get_full_PGN(
        const std::string &event_name,
        int num_round,
        const std::string &white_name,
        const std::string &black_name
    ) const;

private:

    std::string get_PGN_result() const;
    std::string get_PGN_move_text() const;

}; // class ChessGame


#endif // SUCKER_CHESS_CHESS_GAME_HPP
