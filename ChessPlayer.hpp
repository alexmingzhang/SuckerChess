#ifndef SUCKER_CHESS_CHESS_PLAYER_HPP
#define SUCKER_CHESS_CHESS_PLAYER_HPP

#include <random> // for std::mt19937
#include <vector> // for std::vector

#include "ChessMove.hpp"
#include "ChessPosition.hpp"


class ChessPlayer {

public:

    virtual ChessMove pick_move(
        const ChessPosition &current_pos,
        const std::vector<ChessMove> &legal_moves,
        const std::vector<ChessPosition> &pos_history,
        const std::vector<ChessMove> &move_history
    ) = 0;

}; // class ChessPlayer

// =============================================================== RANDOM PLAYER

std::mt19937 properly_seeded_random_engine();


class RandomPlayer : public ChessPlayer {

    std::mt19937 rng;

public:

    RandomPlayer()
        : rng(properly_seeded_random_engine()) {}

    ChessMove pick_move(
        const ChessPosition &current_pos,
        const std::vector<ChessMove> &legal_moves,
        const std::vector<ChessPosition> &pos_history,
        const std::vector<ChessMove> &move_history
    ) override;

}; // class RandomPlayer

// ====================================== CHECKMATE, CHECK, CAPTURE, PUSH (CCCP)

class CCCP_Player : public ChessPlayer {

    std::mt19937 rng;

public:

    CCCP_Player()
        : rng(properly_seeded_random_engine()) {}

    ChessMove pick_move(
        const ChessPosition &current_pos,
        const std::vector<ChessMove> &legal_moves,
        const std::vector<ChessPosition> &pos_history,
        const std::vector<ChessMove> &move_history
    ) override;
};

#endif // SUCKER_CHESS_CHESS_PLAYER_HPP
