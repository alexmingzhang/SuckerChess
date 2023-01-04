#include "Random.hpp"

#include "../Utilities.hpp"


Engine::Random::Random() noexcept
    : rng(properly_seeded_random_engine())
    , name("Random") {}


ChessMove Engine::Random::pick_move(
    ChessEngineInterface &interface,
    [[maybe_unused]] const std::vector<ChessPosition> &pos_history,
    [[maybe_unused]] const std::vector<ChessMove> &move_history
) {
    return random_choice(rng, interface.get_legal_moves());
}


const std::string &Engine::Random::get_name() noexcept { return name; }
