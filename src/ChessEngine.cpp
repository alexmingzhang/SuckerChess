#include "ChessEngine.hpp"

#include <cassert> // for assert
#include <cmath>   // for std::abs
#include <utility> // for std::move

#include "Utilities.hpp"


namespace Preference {


std::vector<ChessMove> MateInOne::pick_preferred_moves(
    const ChessPosition &current_pos,
    const std::vector<ChessMove> &allowed_moves,
    [[maybe_unused]] const std::vector<ChessPosition> &pos_history,
    [[maybe_unused]] const std::vector<ChessMove> &move_history
) {
    return maximal_elements(allowed_moves, [&](ChessMove move) {
        ChessPosition copy = current_pos;
        copy.make_move(move);
        return copy.checkmated();
    });
}


std::vector<ChessMove> Check::pick_preferred_moves(
    const ChessPosition &current_pos,
    const std::vector<ChessMove> &allowed_moves,
    [[maybe_unused]] const std::vector<ChessPosition> &pos_history,
    [[maybe_unused]] const std::vector<ChessMove> &move_history
) {
    return maximal_elements(allowed_moves, [&](ChessMove move) {
        return current_pos.puts_opponent_in_check(move);
    });
}


std::vector<ChessMove> Capture::pick_preferred_moves(
    const ChessPosition &current_pos,
    const std::vector<ChessMove> &allowed_moves,
    [[maybe_unused]] const std::vector<ChessPosition> &pos_history,
    [[maybe_unused]] const std::vector<ChessMove> &move_history
) {
    return maximal_elements(allowed_moves, [&](ChessMove move) {
        return current_pos.is_capture(move);
    });
}


std::vector<ChessMove> Swarm::pick_preferred_moves(
    const ChessPosition &current_pos,
    const std::vector<ChessMove> &allowed_moves,
    [[maybe_unused]] const std::vector<ChessPosition> &pos_history,
    [[maybe_unused]] const std::vector<ChessMove> &move_history
) {
    const ChessSquare enemy_king_location =
        current_pos.get_enemy_king_location(current_pos.get_color_to_move());
    return minimal_elements(allowed_moves, [&](ChessMove move) {
        return enemy_king_location.distance(move.get_dst()) -
               enemy_king_location.distance(move.get_src());
    });
}


std::vector<ChessMove> Huddle::pick_preferred_moves(
    const ChessPosition &current_pos,
    const std::vector<ChessMove> &allowed_moves,
    [[maybe_unused]] const std::vector<ChessPosition> &pos_history,
    [[maybe_unused]] const std::vector<ChessMove> &move_history
) {
    const ChessSquare king_location =
        current_pos.get_king_location(current_pos.get_color_to_move());
    return minimal_elements(allowed_moves, [&](ChessMove move) {
        return king_location.distance(move.get_dst()) -
               king_location.distance(move.get_src());
    });
}


} // namespace Preference


namespace Engine {


Preference::Preference()
    : rng(properly_seeded_random_engine())
    , preferences() {}


void Preference::add_preference(std::unique_ptr<ChessPreference> &&pref) {
    preferences.push_back(std::move(pref));
}


ChessMove Preference::pick_move(
    const ChessPosition &current_pos,
    const std::vector<ChessMove> &legal_moves,
    const std::vector<ChessPosition> &pos_history,
    const std::vector<ChessMove> &move_history
) {
    std::vector<ChessMove> allowed_moves = legal_moves;
    for (const std::unique_ptr<ChessPreference> &pref : preferences) {
        if (allowed_moves.size() <= 1) { break; }
        allowed_moves = pref->pick_preferred_moves(
            current_pos, allowed_moves, pos_history, move_history
        );
    }
    assert(!allowed_moves.empty());
    if (allowed_moves.size() == 1) {
        return allowed_moves[0];
    } else {
        return random_choice(rng, allowed_moves);
    }
}


ChessMove FirstLegalMove::pick_move(
    [[maybe_unused]] const ChessPosition &current_pos,
    const std::vector<ChessMove> &legal_moves,
    [[maybe_unused]] const std::vector<ChessPosition> &pos_history,
    [[maybe_unused]] const std::vector<ChessMove> &move_history
) {
    return legal_moves[0];
}


Random::Random()
    : rng(properly_seeded_random_engine()) {}


ChessMove Random::pick_move(
    [[maybe_unused]] const ChessPosition &current_pos,
    const std::vector<ChessMove> &legal_moves,
    [[maybe_unused]] const std::vector<ChessPosition> &pos_history,
    [[maybe_unused]] const std::vector<ChessMove> &move_history
) {
    return random_choice(rng, legal_moves);
}


Lazy::Lazy()
    : rng(properly_seeded_random_engine()) {}


ChessMove Lazy::pick_move(
    [[maybe_unused]] const ChessPosition &current_pos,
    const std::vector<ChessMove> &legal_moves,
    [[maybe_unused]] const std::vector<ChessPosition> &pos_history,
    [[maybe_unused]] const std::vector<ChessMove> &move_history
) {
    return random_choice(
        rng,
        minimal_elements(
            legal_moves, [&](ChessMove move) { return move.distance(); }
        )
    );
}


Energetic::Energetic()
    : rng(properly_seeded_random_engine()) {}


ChessMove Energetic::pick_move(
    [[maybe_unused]] const ChessPosition &current_pos,
    const std::vector<ChessMove> &legal_moves,
    [[maybe_unused]] const std::vector<ChessPosition> &pos_history,
    [[maybe_unused]] const std::vector<ChessMove> &move_history
) {
    return random_choice(
        rng,
        maximal_elements(
            legal_moves, [&](ChessMove move) { return move.distance(); }
        )
    );
}


Reducer::Reducer()
    : rng(properly_seeded_random_engine()) {}


ChessMove Reducer::pick_move(
    const ChessPosition &current_pos,
    const std::vector<ChessMove> &legal_moves,
    [[maybe_unused]] const std::vector<ChessPosition> &pos_history,
    [[maybe_unused]] const std::vector<ChessMove> &move_history
) {
    return random_choice(
        rng,
        minimal_elements(
            legal_moves,
            [&](ChessMove move) {
                ChessPosition copy = current_pos;
                copy.make_move(move);
                return copy.get_legal_moves().size();
            }
        )
    );
}


CCCP::CCCP()
    : rng(properly_seeded_random_engine()) {}


ChessMove CCCP::pick_move(
    const ChessPosition &current_pos,
    const std::vector<ChessMove> &legal_moves,
    const std::vector<ChessPosition> &pos_history,
    [[maybe_unused]] const std::vector<ChessMove> &move_history
) {
    ChessMove check_move = NULL_MOVE;
    ChessMove best_capture_move = NULL_MOVE;
    int capture_move_best_material_advantage = 0;
    std::vector<ChessMove> push_moves;

    ChessSquare enemy_king_square =
        current_pos.get_enemy_king_location(current_pos.get_color_to_move());

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
        } else if (current_pos.puts_opponent_in_check(move)) {
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
        } else if ((best_capture_move == NULL_MOVE) &&
                   (move.get_dst().distance(enemy_king_square) <
                    move.get_src().distance(enemy_king_square))) {
            push_moves.push_back(move);
        }
    }

    if (check_move != NULL_MOVE) { return check_move; }
    if (best_capture_move != NULL_MOVE) { return best_capture_move; }
    if (!push_moves.empty()) { return random_choice(rng, push_moves); }
    return random_choice(rng, legal_moves);
}


} // namespace Engine
