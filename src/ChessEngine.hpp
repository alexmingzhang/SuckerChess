#ifndef SUCKER_CHESS_CHESS_ENGINE_HPP
#define SUCKER_CHESS_CHESS_ENGINE_HPP

#include <memory> // for std::unique_ptr
#include <random> // for std::mt19937
#include <vector> // for std::vector

#include "ChessMove.hpp"
#include "ChessPosition.hpp"


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

#define DECLARE_PREFERENCE(NAME)                                               \
    class NAME : public ChessPreference {                                      \
        std::vector<ChessMove> pick_preferred_moves(                           \
            const ChessPosition &current_pos,                                  \
            const std::vector<ChessMove> &allowed_moves,                       \
            const std::vector<ChessPosition> &pos_history,                     \
            const std::vector<ChessMove> &move_history                         \
        ) override;                                                            \
    }

DECLARE_PREFERENCE(MateInOne);
DECLARE_PREFERENCE(Check);
DECLARE_PREFERENCE(Capture);
DECLARE_PREFERENCE(First);
DECLARE_PREFERENCE(Last);
DECLARE_PREFERENCE(Reduce);
DECLARE_PREFERENCE(Swarm);
DECLARE_PREFERENCE(Huddle);
DECLARE_PREFERENCE(Energetic);
DECLARE_PREFERENCE(Lazy);
DECLARE_PREFERENCE(Coordinated);
DECLARE_PREFERENCE(Blockade);
DECLARE_PREFERENCE(Defender);
DECLARE_PREFERENCE(Outpost);
DECLARE_PREFERENCE(Gambit);

#undef DECLARE_PREFERENCE

} // namespace Preference


namespace Engine {


class Preference : public ChessEngine {

    std::mt19937 rng;
    std::vector<std::unique_ptr<ChessPreference>> preferences;

public:

    Preference();

    void add(std::unique_ptr<ChessPreference> &&pref);

    template <typename T>
    void add() {
        add(std::make_unique<T>());
    }

    ChessMove pick_move(
        const ChessPosition &current_pos,
        const std::vector<ChessMove> &legal_moves,
        const std::vector<ChessPosition> &pos_history,
        const std::vector<ChessMove> &move_history
    ) override;

}; // class Preference


class CCCP : public ChessEngine {

    std::mt19937 rng;

public:

    CCCP();

    ChessMove pick_move(
        const ChessPosition &current_pos,
        const std::vector<ChessMove> &legal_moves,
        const std::vector<ChessPosition> &pos_history,
        const std::vector<ChessMove> &move_history
    ) override;

}; // class CCCP


} // namespace Engine


#endif // SUCKER_CHESS_CHESS_ENGINE_HPP
