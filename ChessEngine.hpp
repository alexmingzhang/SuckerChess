#ifndef SUCKER_CHESS_CHESS_ENGINE_HPP
#define SUCKER_CHESS_CHESS_ENGINE_HPP

#include <random> // for std::mt19937
#include <vector> // for std::vector

#include "ChessMove.hpp"
#include "ChessPosition.hpp"


class ChessEngine {

public:

    virtual ChessMove pick_move(
        const ChessPosition &current_pos,
        const std::vector<ChessMove> &legal_moves,
        const std::vector<ChessPosition> &pos_history,
        const std::vector<ChessMove> &move_history
    ) = 0;

}; // class ChessEngine

// ===============================================================  RANDOM MOVES

std::mt19937 properly_seeded_random_engine();


class RandomEngine : public ChessEngine {

    std::mt19937 rng;

public:

    RandomEngine()
        : rng(properly_seeded_random_engine()) {}

    ChessMove pick_move(
        const ChessPosition &current_pos,
        const std::vector<ChessMove> &legal_moves,
        const std::vector<ChessPosition> &pos_history,
        const std::vector<ChessMove> &move_history
    ) override;

}; // class RandomEngine

// ====================================== CHECKMATE, CHECK, CAPTURE, PUSH (CCCP)

class CCCP_Engine : public ChessEngine {

    std::mt19937 rng;

public:

    CCCP_Engine()
        : rng(properly_seeded_random_engine()) {}

    ChessMove pick_move(
        const ChessPosition &current_pos,
        const std::vector<ChessMove> &legal_moves,
        const std::vector<ChessPosition> &pos_history,
        const std::vector<ChessMove> &move_history
    ) override;
};

#endif // SUCKER_CHESS_CHESS_ENGINE_HPP
