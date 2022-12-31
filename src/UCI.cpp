#include "UCI.hpp"

#include <algorithm> // for std::find
#include <cassert>   // for assert
#include <cctype>    // for std::isspace
#include <sstream>   // for std::ostringstream
#include <stdexcept> // for std::runtime_error

// use C version of stdio to access non-standard popen() function
#include <stdio.h> // NOLINT(modernize-deprecated-headers)


namespace Engine {


UCI::UCI(
    const std::string &program, Mode engine_mode, unsigned long long engine_n
)
    : pipe(nullptr)
    , mode(engine_mode)
    , n(engine_n) {

    pipe = ::popen(program.c_str(), "r+");
    if (pipe == nullptr) {
        throw std::runtime_error(
            "could not find chess engine program: " + program
        );
    }

    ::fputs("uci\n", pipe);
    ::fflush(pipe);

    while (true) {
        char line[256];
        ::fgets(line, 256, pipe);
        if ((line[0] == 'u') && (line[1] == 'c') && (line[2] == 'i') &&
            (line[3] == 'o') && (line[4] == 'k') && std::isspace(line[5])) {
            break;
        }
    }
}


UCI::~UCI() { ::pclose(pipe); }


static ChessMove read_best_move(std::FILE *pipe) {
    while (true) {
        char line[256];
        ::fgets(line, 256, pipe);
        if ((line[0] == 'b') && (line[1] == 'e') && (line[2] == 's') &&
            (line[3] == 't') && (line[4] == 'm') && (line[5] == 'o') &&
            (line[6] == 'v') && (line[7] == 'e') && std::isspace(line[8])) {

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


ChessMove UCI::pick_move(
    const ChessPosition &current_pos,
    [[maybe_unused]] const std::vector<ChessMove> &legal_moves,
    [[maybe_unused]] const std::vector<ChessPosition> &pos_history,
    [[maybe_unused]] const std::vector<ChessMove> &move_history
) {
    // send current position to engine
    std::ostringstream position_builder;
    position_builder << "position fen " << current_pos.get_fen() << '\n';
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
    assert(
        std::find(legal_moves.begin(), legal_moves.end(), result) !=
        legal_moves.end()
    );
    return result;
}


} // namespace Engine
