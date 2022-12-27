#include "ChessEngine.hpp"

#include <cstddef> // for std::size_t
#include <limits>  // for std::numeric_limits


ChessMove RandomEngine::
    pick_move(const ChessPosition &, const std::vector<ChessMove> &legal_moves, const std::vector<ChessPosition> &, const std::vector<ChessMove> &) {
    std::uniform_int_distribution<std::size_t> dist(0, legal_moves.size() - 1);
    return legal_moves[dist(rng)];
}


ChessMove CCCP_Engine::pick_move(
    const ChessPosition &current_pos,
    const std::vector<ChessMove> &legal_moves,
    const std::vector<ChessPosition> &pos_history,
    const std::vector<ChessMove> &

) {
    ChessMove check_move = NULL_MOVE;
    ChessMove best_capture_move = NULL_MOVE;
    int capture_move_best_material_advantage = 0;
    std::vector<ChessMove> regular_moves;

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
        } else if (current_pos[move.get_dst()] != EMPTY_SQUARE) {
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
        } else {
            regular_moves.push_back(move);
        }
    }

    if (check_move != NULL_MOVE) { return check_move; }
    if (best_capture_move != NULL_MOVE) { return best_capture_move; }

    if (!regular_moves.empty()) {
        std::uniform_int_distribution<std::size_t> dist(
            0, regular_moves.size() - 1
        );
        return regular_moves[dist(rng)];
    }

    return legal_moves[0];
}

ChessMove ReducerEngine::pick_move(
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
