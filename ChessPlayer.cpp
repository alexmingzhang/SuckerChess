#include "ChessPlayer.hpp"


#include <algorithm>  // for std::generate
#include <cstddef>    // for std::size_t
#include <functional> // for std::ref
#include <iterator>   // for std::begin, std::end
#include <random>     // for std::random_device, std::seed_seq


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


ChessMove CCCP_Player::pick_move(
    const ChessPosition &current_pos, const std::vector<ChessMove> &legal_moves,
    const std::vector<ChessPosition> &pos_history,
    const std::vector<ChessMove> &move_history

) {
    std::vector<ChessMove> deepest_moves = {legal_moves[0]};
    coord_t deepest_move_distance = legal_moves[0].distance();

    coord_t enemy_king_file, enemy_king_rank;

    bool enemy_king_found = false;

    for (const ChessMove &move : legal_moves) {
        ChessPosition copy = current_pos;
        copy.make_move(move);

        if (copy.checkmated()) {
            return move;
        } else if (copy.in_check()) {
            return move;
        } else if (current_pos(move.dst_file, move.dst_rank) != EMPTY_SQUARE) {
            return move;
        } else if (copy.stalemated()) {
            continue;
        } else {
            if (!enemy_king_found) {
                for (coord_t file = 0; file < NUM_FILES; ++file) {
                    for (coord_t rank = 0; rank < NUM_RANKS; ++rank) {
                        if (current_pos(file, rank).type == PieceType::KING &&
                            current_pos(file, rank).color !=
                                current_pos.get_color_to_move()) {
                            enemy_king_file = file;
                            enemy_king_rank = rank;
                            enemy_king_found = true;
                            break;
                        }
                    }

                    if (enemy_king_found) { break; }
                }
            }

            if (std::max(
                    std::abs(enemy_king_file - move.dst_file),
                    std::abs(enemy_king_rank - move.dst_rank)
                ) <
                std::max(
                    std::abs(enemy_king_file - move.src_file),
                    std::abs(enemy_king_rank - move.src_rank)
                )) {}

            if (move.distance() > deepest_move_distance) {
                deepest_move_distance = move.distance();
                deepest_moves.clear();
                deepest_moves.push_back(move);
            } else if (move.distance() == deepest_move_distance) {
                deepest_moves.push_back(move);
            }
        }
    }


    std::uniform_int_distribution<std::size_t> dist(
        0, deepest_moves.size() - 1
    );
    return deepest_moves[dist(rng)];
}
