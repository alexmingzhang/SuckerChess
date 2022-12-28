#include "ChessEngine.hpp"

#include <cmath>   // for std::abs
#include <cstddef> // for std::size_t
#include <limits>  // for std::numeric_limits
#include <random>  // for std::uniform_int_distribution


namespace Preference {


std::vector<ChessMove> Swarm::pick_preferred_moves(
    const ChessPosition &current_pos,
    const std::vector<ChessMove> &allowed_moves,
    [[maybe_unused]] const std::vector<ChessPosition> &pos_history,
    [[maybe_unused]] const std::vector<ChessMove> &move_history
) {
    const ChessSquare enemy_king_location =
        current_pos.get_enemy_king_location();
    std::vector<ChessMove> result;
    coord_t best = std::numeric_limits<coord_t>::max();
    for (ChessMove move : allowed_moves) {
        const coord_t score = enemy_king_location.distance(move.get_src()) -
                              enemy_king_location.distance(move.get_dst());
        if (score < best) {
            result.clear();
            result.push_back(move);
            best = score;
        } else if (score == best) {
            result.push_back(move);
        }
    }
    return result;
}


std::vector<ChessMove> Huddle::pick_preferred_moves(
    const ChessPosition &current_pos,
    const std::vector<ChessMove> &allowed_moves,
    [[maybe_unused]] const std::vector<ChessPosition> &pos_history,
    [[maybe_unused]] const std::vector<ChessMove> &move_history
) {
    const ChessSquare king_location = current_pos.get_king_location();
    std::vector<ChessMove> result;
    coord_t best = std::numeric_limits<coord_t>::max();
    for (ChessMove move : allowed_moves) {
        const coord_t score = king_location.distance(move.get_src()) -
                              king_location.distance(move.get_dst());
        if (score < best) {
            result.clear();
            result.push_back(move);
            best = score;
        } else if (score == best) {
            result.push_back(move);
        }
    }
    return result;
}


std::vector<ChessMove> MateInOne::pick_preferred_moves(
    const ChessPosition &current_pos,
    const std::vector<ChessMove> &allowed_moves,
    [[maybe_unused]] const std::vector<ChessPosition> &pos_history,
    [[maybe_unused]] const std::vector<ChessMove> &move_history
) {
    std::vector<ChessMove> result;
    bool found_mate = false;
    for (ChessMove move : allowed_moves) {
        ChessPosition copy = current_pos;
        copy.make_move(move);
        const bool score = copy.checkmated();
        if (score && !found_mate) {
            result.clear();
            result.push_back(move);
            found_mate = true;
        } else if (score == found_mate) {
            result.push_back(move);
        }
    }
    return result;
}


} // namespace Preference


namespace Engine {


ChessMove FirstLegalMove::
    pick_move(const ChessPosition &, const std::vector<ChessMove> &legal_moves, const std::vector<ChessPosition> &, const std::vector<ChessMove> &) {
    return legal_moves[0];
}

ChessMove Random::
    pick_move(const ChessPosition &, const std::vector<ChessMove> &legal_moves, const std::vector<ChessPosition> &, const std::vector<ChessMove> &) {
    return random_choice(rng, legal_moves);
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

ChessMove Reducer::
    pick_move(const ChessPosition &current_pos, const std::vector<ChessMove> &legal_moves, const std::vector<ChessPosition> &, const std::vector<ChessMove> &) {
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

        bool drawn = copy.stalemated();

        if (!drawn) {
            int count = 0;
            for (const ChessPosition &pos : pos_history) {
                if (pos == copy) { ++count; }
                if (count >= 2) {
                    drawn = true;
                    break;
                }
            }
        }

        if (drawn) {
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
