#ifndef SUCKER_CHESS_ENGINE_RANDOM_HPP
#define SUCKER_CHESS_ENGINE_RANDOM_HPP

#include <random> // for std::mt19937
#include <string> // for std::string
#include <vector> // for std::vector

#include "../ChessEngine.hpp"


namespace Engine {


class Random final : public ChessEngine {

    std::mt19937 rng;
    std::string name;

public:

    explicit Random() noexcept;

    ChessMove pick_move(
        ChessEngineInterface &interface,
        const std::vector<ChessPosition> &pos_history,
        const std::vector<ChessMove> &move_history
    ) override;

    const std::string &get_name() noexcept override;

}; // class Random


} // namespace Engine


#endif // SUCKER_CHESS_ENGINE_RANDOM_HPP
