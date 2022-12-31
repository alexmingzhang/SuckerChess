#include "ChessEngine.hpp"

#include <cassert> // for assert
#include <cmath>   // for std::abs
#include <utility> // for std::move

#include "Utilities.hpp"


namespace Preference {

#define DEFINE_PREFERENCE(NAME)                                                \
    std::vector<ChessMove> NAME::pick_preferred_moves(                         \
        [[maybe_unused]] const ChessPosition &current_pos,                     \
        [[maybe_unused]] const std::vector<ChessMove> &allowed_moves,          \
        [[maybe_unused]] const std::vector<ChessPosition> &pos_history,        \
        [[maybe_unused]] const std::vector<ChessMove> &move_history            \
    )

DEFINE_PREFERENCE(MateInOne) {
    return maximal_elements(allowed_moves, [&](ChessMove move) {
        ChessPosition copy = current_pos;
        copy.make_move(move);
        return copy.checkmated();
    });
}

DEFINE_PREFERENCE(PreventMateInOne) {
    return maximal_elements(allowed_moves, [&](ChessMove move) {
        ChessPosition copy = current_pos;
        copy.make_move(move);

        for (ChessMove move_2 : copy.get_legal_moves()) {
            ChessPosition copy_2 = copy;
            copy_2.make_move(move_2);

            if (copy_2.checkmated()) { return false; }
        }

        return true;
    });
}

DEFINE_PREFERENCE(PreventStalemate) {
    return maximal_elements(allowed_moves, [&](ChessMove move) {
        ChessPosition copy = current_pos;
        copy.make_move(move);

        if (copy.stalemated()) { return false; }

        int count = 0;
        for (const ChessPosition &pos : pos_history) {
            if (copy == pos) { ++count; }
            if (count >= 2) { return false; }
        }

        return true;
    });
}

DEFINE_PREFERENCE(Check) {
    return maximal_elements(allowed_moves, [&](ChessMove move) {
        return current_pos.puts_opponent_in_check(move);
    });
}

DEFINE_PREFERENCE(Capture) {
    return maximal_elements(allowed_moves, [&](ChessMove move) {
        return current_pos.is_capture(move);
    });
}

DEFINE_PREFERENCE(CaptureHanging) {
    return maximal_elements(allowed_moves, [&](ChessMove move) {
        return current_pos.is_capture(move) &&
               current_pos.get_board().is_attacked_by(
                   !current_pos.get_color_to_move(), move.get_dst()
               ) == 0;
    });
}

DEFINE_PREFERENCE(SmartCapture) {
    return maximal_elements(allowed_moves, [&](ChessMove move) {
        if (!current_pos.is_capture(move)) { return 0; }

        const PieceColor self = current_pos.get_color_to_move();
        const PieceColor enemy = !self;
        int defenders =
            current_pos.get_board().count_attacks_by(self, move.get_dst());
        int attackers =
            current_pos.get_board().count_attacks_by(enemy, move.get_dst());

        return defenders - attackers;
    });
}

DEFINE_PREFERENCE(Castle) {
    return maximal_elements(allowed_moves, [&](ChessMove move) {
        if (current_pos.is_castle(move)) { return 2; }
        ChessPosition copy = current_pos;
        copy.make_move(move);
        if (copy.get_castling_rights() < current_pos.get_castling_rights()) {
            return 0;
        }
        return 1;
    });
}

DEFINE_PREFERENCE(First) { return {*allowed_moves.begin()}; }

DEFINE_PREFERENCE(Last) { return {*allowed_moves.rbegin()}; }

DEFINE_PREFERENCE(Reduce) {
    return minimal_elements(allowed_moves, [&](ChessMove move) {
        ChessPosition copy = current_pos;
        copy.make_move(move);
        return copy.get_legal_moves().size();
    });
}


DEFINE_PREFERENCE(Greedy) {
    return maximal_elements(allowed_moves, [&](ChessMove move) {
        ChessPosition copy = current_pos;
        copy.make_move(move);

        return (
            (current_pos.get_color_to_move() == PieceColor::WHITE)
                ? copy.get_material_advantage()
                : copy.get_material_advantage() * -1
        );
    });
}


DEFINE_PREFERENCE(Swarm) {
    const ChessSquare enemy_king_location =
        current_pos.get_enemy_king_location(current_pos.get_color_to_move());
    return minimal_elements(allowed_moves, [&](ChessMove move) {
        return enemy_king_location.distance(move.get_dst()) -
               enemy_king_location.distance(move.get_src());
    });
}

DEFINE_PREFERENCE(Huddle) {
    const ChessSquare king_location =
        current_pos.get_king_location(current_pos.get_color_to_move());
    return minimal_elements(allowed_moves, [&](ChessMove move) {
        return king_location.distance(move.get_dst()) -
               king_location.distance(move.get_src());
    });
}

DEFINE_PREFERENCE(Energetic) {
    return maximal_elements(allowed_moves, [&](ChessMove move) {
        return move.distance();
    });
}

DEFINE_PREFERENCE(Lazy) {
    return minimal_elements(allowed_moves, [&](ChessMove move) {
        return move.distance();
    });
}

DEFINE_PREFERENCE(Coordinated) {
    return maximal_elements(allowed_moves, [&](ChessMove move) {
        const PieceColor self = current_pos.get_color_to_move();
        ChessPosition copy = current_pos;
        copy.make_move(move);
        int result = 0;
        for (coord_t file = 0; file < NUM_FILES; ++file) {
            for (coord_t rank = 0; rank < NUM_RANKS; ++rank) {
                ChessSquare square = {file, rank};
                if (copy.get_board().is_attacked_by(self, square)) { ++result; }
            }
        }
        return result;
    });
}

DEFINE_PREFERENCE(Blockade) {
    return minimal_elements(allowed_moves, [&](ChessMove move) {
        ChessPosition copy = current_pos;
        copy.make_move(move);
        const PieceColor enemy = copy.get_color_to_move();
        int result = 0;
        for (coord_t file = 0; file < NUM_FILES; ++file) {
            for (coord_t rank = 0; rank < NUM_RANKS; ++rank) {
                ChessSquare square = {file, rank};
                if (copy.get_board().is_attacked_by(enemy, square)) {
                    ++result;
                }
            }
        }
        return result;
    });
}

DEFINE_PREFERENCE(Defender) {
    return maximal_elements(allowed_moves, [&](ChessMove move) {
        const PieceColor self = current_pos.get_color_to_move();
        return current_pos.get_board().is_attacked_by(self, move.get_dst());
    });
}

DEFINE_PREFERENCE(Outpost) {
    return minimal_elements(allowed_moves, [&](ChessMove move) {
        const PieceColor self = current_pos.get_color_to_move();
        const PieceColor enemy = !self;
        return current_pos.get_board().is_attacked_by(enemy, move.get_dst());
    });
}

DEFINE_PREFERENCE(Gambit) {
    return maximal_elements(allowed_moves, [&](ChessMove move) {
        const PieceColor self = current_pos.get_color_to_move();
        const PieceColor enemy = !self;
        return current_pos.get_board().is_attacked_by(self, move.get_dst()) &&
               current_pos.get_board().is_attacked_by(enemy, move.get_dst());
    });
}

DEFINE_PREFERENCE(Explore) {
    return minimal_elements(allowed_moves, [&](ChessMove move) {
        const PieceColor self = current_pos.get_color_to_move();
        const PieceColor enemy = !self;
        return current_pos.get_board().is_attacked_by(self, move.get_dst()) ||
               current_pos.get_board().is_attacked_by(enemy, move.get_dst());
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
    // int count = 0;
    for (const std::unique_ptr<ChessPreference> &pref : preferences) {
        if (allowed_moves.size() <= 1) { break; }
        allowed_moves = pref->pick_preferred_moves(
            current_pos, allowed_moves, pos_history, move_history
        );
        // std::cout << "Allowed moves " << count++ << ": " << allowed_moves
        //           << '\n';
    }
    assert(!allowed_moves.empty());
    if (allowed_moves.size() == 1) {
        return allowed_moves[0];
    } else {
        return random_choice(rng, allowed_moves);
    }
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
