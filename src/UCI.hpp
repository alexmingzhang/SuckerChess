#ifndef SUCKER_CHESS_UCI_HPP
#define SUCKER_CHESS_UCI_HPP

#include <cstdio> // for std::FILE
#include <string> // for std::string
#include <vector> // for std::vector

#include "ChessEngine.hpp"


namespace Engine {


class UCI final : public ChessEngine {

public:

    enum class Mode { DEPTH, NODES };

private:

    std::FILE *pipe;
    Mode mode;
    unsigned long long n;

public:

    explicit UCI(
        const std::string &program,
        Mode engine_mode,
        unsigned long long engine_n
    );

    ~UCI() override;

    // explicitly prevent copying and assignment of UCI engines
    UCI(const UCI &) = delete;
    UCI &operator=(const UCI &) = delete;

    ChessMove pick_move(
        const ChessPosition &current_pos,
        const std::vector<ChessMove> &legal_moves,
        const std::vector<ChessPosition> &pos_history,
        const std::vector<ChessMove> &move_history
    ) override;

}; // class UCI


} // namespace Engine


#endif // SUCKER_CHESS_UCI_HPP
