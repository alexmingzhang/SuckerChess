#ifndef SUCKER_CHESS_CHESS_ENGINE_HPP
#define SUCKER_CHESS_CHESS_ENGINE_HPP

#include <random> // for std::mt19937
#include <vector> // for std::vector

#include "ChessMove.hpp"
#include "ChessPosition.hpp"
#include "Utilities.hpp"


class ChessEngine {

public:

    virtual ~ChessEngine() noexcept = default;

    virtual ChessMove pick_move(
        const ChessPosition &current_pos,
        const std::vector<ChessMove> &legal_moves,
        const std::vector<ChessPosition> &pos_history,
        const std::vector<ChessMove> &move_history
    ) = 0;

}; // class ChessEngine

namespace Engine {

// ============================================================ FIRST LEGAL MOVE

class FirstLegalMove : public ChessEngine {

public:

    ChessMove pick_move(
        const ChessPosition &current_pos,
        const std::vector<ChessMove> &legal_moves,
        const std::vector<ChessPosition> &pos_history,
        const std::vector<ChessMove> &move_history
    ) override;

}; // class FirstLegalMove


// ===============================================================  RANDOM MOVES


class Random : public ChessEngine {

    std::mt19937 rng;

public:

    Random()
        : rng(properly_seeded_random_engine()) {}

    ChessMove pick_move(
        const ChessPosition &current_pos,
        const std::vector<ChessMove> &legal_moves,
        const std::vector<ChessPosition> &pos_history,
        const std::vector<ChessMove> &move_history
    ) override;

}; // class Random


// =======================================  LAZY (Picks moves w/ least distance)


class Lazy : public ChessEngine {

    std::mt19937 rng;

public:

    Lazy()
        : rng(properly_seeded_random_engine()) {}

    ChessMove pick_move(
        const ChessPosition &current_pos,
        const std::vector<ChessMove> &legal_moves,
        const std::vector<ChessPosition> &pos_history,
        const std::vector<ChessMove> &move_history
    ) override;

}; // class Lazy


// ===================================  ENERGETIC (Picks moves w/ most distance)


class Energetic : public ChessEngine {

    std::mt19937 rng;

public:

    Energetic()
        : rng(properly_seeded_random_engine()) {}

    ChessMove pick_move(
        const ChessPosition &current_pos,
        const std::vector<ChessMove> &legal_moves,
        const std::vector<ChessPosition> &pos_history,
        const std::vector<ChessMove> &move_history
    ) override;

}; // class Energetic


// ================================================ REDUCER (Reduce enemy moves)


class Reducer : public ChessEngine {

    std::mt19937 rng;

public:

    Reducer()
        : rng(properly_seeded_random_engine()) {}

    ChessMove pick_move(
        const ChessPosition &current_pos,
        const std::vector<ChessMove> &legal_moves,
        const std::vector<ChessPosition> &pos_history,
        const std::vector<ChessMove> &move_history
    ) override;
}; // class Reducer


// ====================================== CHECKMATE, CHECK, CAPTURE, PUSH (CCCP)


class CCCP : public ChessEngine {

    std::mt19937 rng;

public:

    CCCP()
        : rng(properly_seeded_random_engine()) {}

    ChessMove pick_move(
        const ChessPosition &current_pos,
        const std::vector<ChessMove> &legal_moves,
        const std::vector<ChessPosition> &pos_history,
        const std::vector<ChessMove> &move_history
    ) override;
}; // class CCCP

} // namespace Engine


#endif // SUCKER_CHESS_CHESS_ENGINE_HPP
