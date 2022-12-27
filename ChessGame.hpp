#ifndef SUCKER_CHESS_CHESS_GAME_HPP
#define SUCKER_CHESS_CHESS_GAME_HPP

#include <vector> // for std::vector

#include "ChessEngine.hpp"
#include "ChessMove.hpp"
#include "ChessPiece.hpp"
#include "ChessPosition.hpp"


class ChessGame {

public:

    ChessPosition current_pos;
    PieceColor winner;
    std::vector<ChessPosition> pos_history;
    std::vector<ChessMove> move_history;
    int half_move_clock;
    int full_move_count;

    explicit ChessGame() noexcept
        : current_pos()
        , winner(PieceColor::NONE)
        , pos_history()
        , move_history()
        , half_move_clock(0)
        , full_move_count(1) {}

    void make_move(const ChessMove &move);

    [[nodiscard]] bool drawn() const;

    ChessMove get_console_move(const std::vector<ChessMove> &legal_moves);

    PieceColor run(ChessEngine *white, ChessEngine *black, bool verbose = true);

    std::string get_PGN() const;

}; // class ChessGame


#endif // SUCKER_CHESS_CHESS_GAME_HPP
