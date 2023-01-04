#ifndef SUCKER_CHESS_ENGINE_RANDOM_HPP
#define SUCKER_CHESS_ENGINE_RANDOM_HPP

#include <random> // for std::mt19937
#include <vector> // for std::vector

#include "../ChessEngine.hpp"


namespace Engine {


class Random final : public ChessEngine {

    std::mt19937 rng;

public:

    explicit Random() noexcept;

    ChessMove pick_move(
        ChessEngineInterface &interface,
        const std::vector<ChessPosition> &pos_history,
        const std::vector<ChessMove> &move_history
    ) override;

}; // class Random


} // namespace Engine


#endif // SUCKER_CHESS_ENGINE_RANDOM_HPP
