#include "ChessEngine.hpp"

#include <cmath>   // for std::abs
#include <cstddef> // for std::size_t
#include <limits>  // for std::numeric_limits
#include <random>  // for std::uniform_int_distribution

namespace Engine {


ChessMove FirstLegalMove::
    pick_move(const ChessPosition &, const std::vector<ChessMove> &legal_moves, const std::vector<ChessPosition> &, const std::vector<ChessMove> &) {
    return legal_moves[0];
}

ChessMove Random::
    pick_move(const ChessPosition &, const std::vector<ChessMove> &legal_moves, const std::vector<ChessPosition> &, const std::vector<ChessMove> &) {
    std::uniform_int_distribution<std::size_t> dist(0, legal_moves.size() - 1);
    return legal_moves[dist(rng)];
}

ChessMove Lazy::
    pick_move(const ChessPosition &, const std::vector<ChessMove> &legal_moves, const std::vector<ChessPosition> &, const std::vector<ChessMove> &) {
    std::vector<ChessMove> laziest_moves = {legal_moves[0]};
    coord_t laziest_move_distance =
        legal_moves[0].distance(); // Include header file that defines coord_t?

    for (const ChessMove move : legal_moves) {
        coord_t distance = move.distance();
        if (distance < laziest_move_distance) {
            laziest_moves.clear();
            laziest_moves.push_back(move);
            laziest_move_distance = distance;
        } else if (distance == laziest_move_distance) {
            laziest_moves.push_back(move);
        }
    }

    std::uniform_int_distribution<std::size_t> dist(
        0, laziest_moves.size() - 1
    );
    return laziest_moves[dist(rng)];
}

ChessMove Energetic::
    pick_move(const ChessPosition &, const std::vector<ChessMove> &legal_moves, const std::vector<ChessPosition> &, const std::vector<ChessMove> &) {
    std::vector<ChessMove> longest_moves = {legal_moves[0]};
    coord_t longest_move_distance =
        legal_moves[0].distance(); // Include header file that defines coord_t?

    for (const ChessMove move : legal_moves) {
        coord_t distance = move.distance();
        if (distance > longest_move_distance) {
            longest_moves.clear();
            longest_moves.push_back(move);
            longest_move_distance = distance;
        } else if (distance == longest_move_distance) {
            longest_moves.push_back(move);
        }
    }

    std::uniform_int_distribution<std::size_t> dist(
        0, longest_moves.size() - 1
    );
    return longest_moves[dist(rng)];
}

ChessMove Reducer::pick_move(
    const ChessPosition &current_pos,
    const std::vector<ChessMove> &legal_moves,
    const std::vector<ChessPosition> &,
    const std::vector<ChessMove> &

) {
    std::vector<ChessMove> best_moves;
    int best_move_score = std::numeric_limits<int>::max();

    for (const ChessMove move : legal_moves) {
        ChessPosition copy = current_pos;
        copy.make_move(move);
        int move_score = static_cast<int>(copy.get_legal_moves().size());

        if (move_score < best_move_score) {
            best_moves.clear();
            best_moves.push_back(move);
            best_move_score = move_score;
        } else if (move_score == best_move_score) {
            best_moves.push_back(move);
        }
    }

    std::uniform_int_distribution<std::size_t> dist(0, best_moves.size() - 1);
    return best_moves[dist(rng)];
}

ChessMove CCCP::pick_move(
    const ChessPosition &current_pos,
    const std::vector<ChessMove> &legal_moves,
    const std::vector<ChessPosition> &pos_history,
    const std::vector<ChessMove> &

) {
    ChessMove check_move = NULL_MOVE;
    ChessMove best_capture_move = NULL_MOVE;
    int capture_move_best_material_advantage = 0;
    std::vector<ChessMove> push_moves;

    ChessSquare enemy_king_square =
        current_pos.get_color_to_move() == PieceColor::WHITE
            ? current_pos.get_black_king_location()
            : current_pos.get_white_king_location();

    for (const ChessMove move : legal_moves) {
        ChessPosition copy = current_pos;
        copy.make_move(move);

        bool draw_by_repetition = false;
        int count = 0;
        for (const ChessPosition &pos : pos_history) {
            if (pos == copy) { ++count; }
            if (count >= 2) {
                draw_by_repetition = true;
                break;
            }
        }

        if (copy.stalemated() || draw_by_repetition) {
            continue;
        } else if (copy.checkmated()) {
            return move;
        } else if (copy.in_check()) {
            check_move = move;
        } else if (check_move == NULL_MOVE && current_pos[move.get_dst()] != EMPTY_SQUARE) {
            int capture_move_material_advantage = std::abs(
                copy.get_material_advantage() -
                current_pos.get_material_advantage()
            );

            if (capture_move_material_advantage >
                capture_move_best_material_advantage) {
                capture_move_best_material_advantage =
                    capture_move_material_advantage;
                best_capture_move = move;
            }
        } else if (best_capture_move == NULL_MOVE && move.get_dst().distance(enemy_king_square) < move.get_src().distance(enemy_king_square)) {
            push_moves.push_back(move);
        }
    }

    if (check_move != NULL_MOVE) { return check_move; }
    if (best_capture_move != NULL_MOVE) { return best_capture_move; }

    if (!push_moves.empty()) {
        std::uniform_int_distribution<std::size_t> dist(
            0, push_moves.size() - 1
        );
        return push_moves[dist(rng)];
    }

    // Fallback to random move
    std::uniform_int_distribution<std::size_t> dist(0, legal_moves.size() - 1);
    return legal_moves[0];
}


} // namespace Engine
