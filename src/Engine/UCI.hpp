#ifndef SUCKER_CHESS_UCI_HPP
#define SUCKER_CHESS_UCI_HPP

#include <cstdio> // for std::FILE
#include <string> // for std::string
#include <vector> // for std::vector

#include "../ChessEngine.hpp"


namespace Engine {


class UCI final : public ChessEngine {

public:

    enum class Mode { DEPTH, NODES };

private:

    std::FILE *pipe;
    Mode mode;
    unsigned long long n;
    std::string name;

public:

    explicit UCI(
        const std::string &engine_command,
        Mode engine_mode,
        unsigned long long engine_n,
        std::string engine_name
    );

    ~UCI() override;

    // explicitly prevent copying and assignment of UCI engines
    UCI(const UCI &) = delete;
    UCI &operator=(const UCI &) = delete;

    ChessMove pick_move(
        ChessEngineInterface &interface,
        const std::vector<ChessPosition> &pos_history,
        const std::vector<ChessMove> &move_history
    ) override;

    const std::string &get_name() noexcept override;

}; // class UCI


} // namespace Engine


#endif // SUCKER_CHESS_UCI_HPP
