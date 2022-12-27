#ifndef SUCKER_CHESS_CHESS_TOURNAMENT_HPP
#define SUCKER_CHESS_CHESS_TOURNAMENT_HPP

#include "ChessEngine.hpp"

#include <cmath> // for std::exp10
#include <set>   // for std::set


class ChessTournament {

public:

    std::vector<ChessEngine *> engines;
};

#endif // SUCKER_CHESS_CHESS_TOURNAMENT_HPP