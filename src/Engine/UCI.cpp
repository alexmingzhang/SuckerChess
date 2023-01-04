#include "UCI.hpp"

#include <cassert>   // for assert
#include <cctype>    // for std::isspace
#include <cstring>   // for std::strncmp
#include <sstream>   // for std::ostringstream
#include <stdexcept> // for std::runtime_error
#include <utility>   // for std::move

// use C version of stdio to access non-standard popen() function
#include <stdio.h> // NOLINT(modernize-deprecated-headers)

#include "../Utilities.hpp"


Engine::UCI::UCI(
    const std::string &engine_command,
    Engine::UCI::Mode engine_mode,
    unsigned long long int engine_n,
    std::string engine_name
)
    : pipe(::popen(engine_command.c_str(), "r+"))
    , mode(engine_mode)
    , n(engine_n)
    , name(std::move(engine_name)) {

    if (pipe == nullptr) {
        throw std::runtime_error(
            "could not find chess engine program: " + engine_command
        );
    }

    ::fputs("uci\n", pipe);
    ::fflush(pipe);

    while (true) {
        char line[256];
        ::fgets(line, 256, pipe);
        if ((std::strncmp(line, "uciok", 5) == 0) && std::isspace(line[5])) {
            break;
        }
    }

    // TODO: set engine options

    ::fputs("isready\n", pipe);
    ::fflush(pipe);

    while (true) {
        char line[256];
        ::fgets(line, 256, pipe);
        if ((std::strncmp(line, "readyok", 7) == 0) && std::isspace(line[7])) {
            break;
        }
    }
}


Engine::UCI::~UCI() { ::pclose(pipe); }


static ChessMove read_best_move(std::FILE *pipe) {
    while (true) {
        char line[256];
        ::fgets(line, 256, pipe);
        if ((std::strncmp(line, "bestmove", 8) == 0) && std::isspace(line[8])) {

            const char src_file = line[9];
            if ((src_file < 'a') || (src_file > 'h')) {
                throw std::runtime_error(
                    "chess engine returned move with invalid source file"
                );
            }

            const char src_rank = line[10];
            if ((src_rank < '1') || (src_rank > '8')) {
                throw std::runtime_error(
                    "chess engine returned move with invalid source rank"
                );
            }

            const ChessSquare src = {
                static_cast<coord_t>(src_file - 'a'),
                static_cast<coord_t>(src_rank - '1')};

            const char dst_file = line[11];
            if ((dst_file < 'a') || (dst_file > 'h')) {
                throw std::runtime_error(
                    "chess engine returned move with invalid destination file"
                );
            }

            const char dst_rank = line[12];
            if ((dst_rank < '1') || (dst_rank > '8')) {
                throw std::runtime_error(
                    "chess engine returned move with invalid destination rank"
                );
            }

            const ChessSquare dst = {
                static_cast<coord_t>(dst_file - 'a'),
                static_cast<coord_t>(dst_rank - '1')};

            const char promotion_type = line[13];
            if (std::isspace(promotion_type)) {
                return {src, dst};
            } else if (promotion_type == 'q') {
                return {src, dst, PieceType::QUEEN};
            } else if (promotion_type == 'r') {
                return {src, dst, PieceType::ROOK};
            } else if (promotion_type == 'b') {
                return {src, dst, PieceType::BISHOP};
            } else if (promotion_type == 'n') {
                return {src, dst, PieceType::KNIGHT};
            } else {
                throw std::runtime_error(
                    "chess engine returned move with invalid promotion type"
                );
            }
        }
    }
}


ChessMove Engine::UCI::pick_move(
    ChessEngineInterface &interface,
    [[maybe_unused]] const std::vector<ChessPosition> &pos_history,
    [[maybe_unused]] const std::vector<ChessMove> &move_history
) {
    // send current position to engine
    std::ostringstream position_builder;
    position_builder << "position fen " << interface.get_current_pos().get_fen()
                     << '\n';
    const std::string position_command = position_builder.str();
    ::fputs(position_command.c_str(), pipe);

    // instruct engine to find best move
    std::ostringstream go_builder;
    switch (mode) {
        case Mode::DEPTH: go_builder << "go depth "; break;
        case Mode::NODES: go_builder << "go nodes "; break;
    }
    go_builder << n << '\n';
    const std::string go_command = go_builder.str();
    ::fputs(go_command.c_str(), pipe);

    // read best move from engine output
    const ChessMove result = read_best_move(pipe);
    assert(contains(interface.get_legal_moves(), result));
    return result;
}


const std::string &Engine::UCI::get_name() noexcept { return name; }
