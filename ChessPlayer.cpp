#include "ChessPlayer.hpp"

#include <algorithm>  // for std::generate
#include <cstddef>    // for std::size_t
#include <functional> // for std::ref
#include <iostream>   // for std::cin, std::cout
#include <iterator>   // for std::begin, std::end
#include <random>     // for std::random_device, std::seed_seq
#include <string>     // for std::string


ChessMove ConsolePlayer::pick_move(
    const ChessPosition &current_pos, const std::vector<ChessMove> &legal_moves,
    const std::vector<ChessPosition> &pos_history,
    const std::vector<ChessMove> &move_history
) {
    std::vector<std::string> names;
    for (const ChessMove &move : legal_moves) {
        names.push_back(current_pos.get_move_name(legal_moves, move));
    }
    while (true) {
        std::string command;
        std::cout << "> ";
        std::cin >> command;
        if (command == "ls") {
            std::cout << "Legal moves:";
            for (const std::string &name : names) { std::cout << ' ' << name; }
            std::cout << std::endl;
        } else {
            for (std::size_t i = 0; i < legal_moves.size(); ++i) {
                if (command == names[i]) { return legal_moves[i]; }
            }
            std::cout << "ERROR: " << command
                      << " is not a legal move in this position." << std::endl;
        }
    }
}


std::mt19937 properly_seeded_random_engine() {
    constexpr std::size_t SEED_SIZE =
        std::mt19937::state_size * sizeof(typename std::mt19937::result_type);
    std::random_device seed_source;
    std::random_device::result_type
        seed_data[(SEED_SIZE - 1) / sizeof(seed_source()) + 1];
    std::generate(
        std::begin(seed_data), std::end(seed_data), std::ref(seed_source)
    );
    std::seed_seq seed(std::begin(seed_data), std::end(seed_data));
    return std::mt19937(seed);
}


ChessMove RandomPlayer::pick_move(
    const ChessPosition &current_pos, const std::vector<ChessMove> &legal_moves,
    const std::vector<ChessPosition> &pos_history,
    const std::vector<ChessMove> &move_history
) {
    std::uniform_int_distribution<std::size_t> dist(0, legal_moves.size() - 1);
    return legal_moves[dist(rng)];
}