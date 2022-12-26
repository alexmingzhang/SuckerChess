#ifndef SUCKER_CHESS_CHESS_GAME_HPP
#define SUCKER_CHESS_CHESS_GAME_HPP

#include <vector> // for std::vector

#include "ChessMove.hpp"
#include "ChessPiece.hpp"
#include "ChessPlayer.hpp"
#include "ChessPosition.hpp"


class ChessGame {

    ChessPosition current_pos;
    std::vector<ChessPosition> pos_history;
    std::vector<ChessMove> move_history;
    int half_move_clock;
    int full_move_count;

public:

    explicit constexpr ChessGame() noexcept
        : current_pos()
        , pos_history()
        , move_history()
        , half_move_clock(0)
        , full_move_count(1) {}

    void make_move(const ChessMove &move);

    [[nodiscard]] bool drawn() const;

    PieceColor run(ChessPlayer *white, ChessPlayer *black, bool verbose = true);

}; // class ChessGame


#endif // SUCKER_CHESS_CHESS_GAME_HPP
