#ifndef SUCKER_CHESS_CHESS_ENGINE_HPP
#define SUCKER_CHESS_CHESS_ENGINE_HPP

#include <memory> // for std::unique_ptr
#include <random> // for std::mt19937
#include <vector> // for std::vector

#include "ChessMove.hpp"
#include "ChessPosition.hpp"
#include "Utilities.hpp"


class ChessPreference {

public:

    virtual ~ChessPreference() noexcept = default;

    virtual std::vector<ChessMove> pick_preferred_moves(
        const ChessPosition &current_pos,
        const std::vector<ChessMove> &allowed_moves,
        const std::vector<ChessPosition> &pos_history,
        const std::vector<ChessMove> &move_history
    ) = 0;

}; // class ChessPreference


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


namespace Preference {


class Swarm : public ChessPreference {

    std::vector<ChessMove> pick_preferred_moves(
        const ChessPosition &current_pos,
        const std::vector<ChessMove> &allowed_moves,
        const std::vector<ChessPosition> &pos_history,
        const std::vector<ChessMove> &move_history
    ) override;

}; // class Swarm


class Huddle : public ChessPreference {

    std::vector<ChessMove> pick_preferred_moves(
        const ChessPosition &current_pos,
        const std::vector<ChessMove> &allowed_moves,
        const std::vector<ChessPosition> &pos_history,
        const std::vector<ChessMove> &move_history
    ) override;

}; // class Huddle


class MateInOne : public ChessPreference {

    std::vector<ChessMove> pick_preferred_moves(
        const ChessPosition &current_pos,
        const std::vector<ChessMove> &allowed_moves,
        const std::vector<ChessPosition> &pos_history,
        const std::vector<ChessMove> &move_history
    ) override;

}; // class MateInOne


} // namespace Preference


namespace Engine {


class Preference : public ChessEngine {

    std::mt19937 rng;
    std::vector<std::unique_ptr<ChessPreference>> preferences;

public:

    Preference()
        : rng(properly_seeded_random_engine())
        , preferences() {}

    void add_preference(std::unique_ptr<ChessPreference> &&pref) {
        preferences.push_back(std::move(pref));
    }

    ChessMove pick_move(
        const ChessPosition &current_pos,
        const std::vector<ChessMove> &legal_moves,
        const std::vector<ChessPosition> &pos_history,
        const std::vector<ChessMove> &move_history
    ) override {
        std::vector<ChessMove> allowed_moves = legal_moves;
        for (const std::unique_ptr<ChessPreference> &pref : preferences) {
            if (allowed_moves.size() <= 1) { break; }
            allowed_moves = pref->pick_preferred_moves(
                current_pos, allowed_moves, pos_history, move_history
            );
        }
        assert(allowed_moves.size() > 0);
        return random_choice(rng, allowed_moves);
    }

}; // class Preference


class FirstLegalMove : public ChessEngine { // ================ FIRST LEGAL MOVE

public:

    ChessMove pick_move(
        const ChessPosition &current_pos,
        const std::vector<ChessMove> &legal_moves,
        const std::vector<ChessPosition> &pos_history,
        const std::vector<ChessMove> &move_history
    ) override;

}; // class FirstLegalMove


class Random : public ChessEngine { // ============================ RANDOM MOVES

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


class Lazy : public ChessEngine { // ====== LAZY (Picks moves w/ least distance)

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


class Reducer : public ChessEngine { // =========== REDUCER (Reduce enemy moves)

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
