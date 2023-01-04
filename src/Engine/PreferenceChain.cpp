#include "PreferenceChain.hpp"

#include <cassert> // for assert
#include <sstream> // for std::ostringstream
#include <utility> // for std::move

#include "../Utilities.hpp"


ChessPreference::~ChessPreference() noexcept = default;


namespace Preference {

#define DEFINE_PREFERENCE(NAME)                                                \
    std::vector<ChessMove> NAME::pick_preferred_moves(                         \
        [[maybe_unused]] ChessEngineInterface &interface,                      \
        const std::vector<ChessMove> &allowed_moves                            \
    )

DEFINE_PREFERENCE(MateInOne) {
    return maximal_elements(allowed_moves, [&](ChessMove move) {
        ChessPosition copy = interface.get_current_pos();
        copy.make_move(move);
        return interface.checkmated(copy);
    });
}

DEFINE_PREFERENCE(PreventMateInOne) {
    return maximal_elements(allowed_moves, [&](ChessMove move) {
        ChessPosition copy = interface.get_current_pos();
        copy.make_move(move);

        // for each possible opponent response...
        for (ChessMove move_2 : interface.get_legal_moves(copy)) {
            ChessPosition copy_2 = copy;
            copy_2.make_move(move_2);

            // ...ensure that response does not deliver checkmate
            if (interface.checkmated(copy_2)) { return false; }
        }
        return true;
    });
}

DEFINE_PREFERENCE(PreventDraw) {
    return minimal_elements(allowed_moves, [&](ChessMove move) {
        ChessPosition copy = interface.get_current_pos();
        copy.make_move(move);
        return interface.stalemated(copy) ||
               copy.get_board().has_insufficient_material();
    });
}

DEFINE_PREFERENCE(Check) {
    return maximal_elements(allowed_moves, [&](ChessMove move) {
        ChessPosition next = interface.get_current_pos();
        next.make_move(move);
        return next.in_check();
    });
}

DEFINE_PREFERENCE(Capture) {
    return maximal_elements(allowed_moves, [&](ChessMove move) {
        return interface.get_current_pos().is_capture(move);
    });
}

DEFINE_PREFERENCE(CaptureHanging) {
    const PieceColor self = interface.get_color_to_move();
    const PieceColor enemy = !self;
    const ChessPosition &current_pos = interface.get_current_pos();
    const ChessBoard &board = current_pos.get_board();
    return maximal_elements(allowed_moves, [&](ChessMove move) {
        return current_pos.is_capture(move) &&
               !board.is_attacked_by(enemy, move.get_dst());
    });
}

DEFINE_PREFERENCE(SmartCapture) {
    const PieceColor self = interface.get_color_to_move();
    const PieceColor enemy = !self;
    const ChessPosition &current_pos = interface.get_current_pos();
    const ChessBoard &board = current_pos.get_board();
    return maximal_elements(allowed_moves, [&](ChessMove move) {
        if (!current_pos.is_capture(move)) { return 0; }
        int defenders = board.count_attacks_by(self, move.get_dst());
        int attackers = board.count_attacks_by(enemy, move.get_dst());
        return defenders - attackers;
    });
}

DEFINE_PREFERENCE(Castle) {
    const PieceColor self = interface.get_color_to_move();
    const ChessPosition &current_pos = interface.get_current_pos();
    return maximal_elements(allowed_moves, [&](ChessMove move) {
        // always castle if possible
        if (current_pos.is_castle(move)) { return 2; }

        // determine whether this moves gives up castling rights
        ChessPosition copy = current_pos;
        copy.make_move(move);

        // avoid giving up short castling rights
        if (current_pos.can_short_castle(self) &&
            !copy.can_short_castle(self)) {
            return 0;
        }

        // avoid giving up long castling rights
        if (current_pos.can_long_castle(self) && !copy.can_long_castle(self)) {
            return 0;
        }

        return 1;
    });
}

DEFINE_PREFERENCE(First) { return {*allowed_moves.begin()}; }

DEFINE_PREFERENCE(Last) { return {*allowed_moves.rbegin()}; }

DEFINE_PREFERENCE(Reduce) {
    return minimal_elements(allowed_moves, [&](ChessMove move) {
        ChessPosition copy = interface.get_current_pos();
        copy.make_move(move);
        return interface.get_legal_moves(copy).size();
    });
}

DEFINE_PREFERENCE(Greedy) {
    const ChessBoard &board = interface.get_current_pos().get_board();
    return minimal_elements(allowed_moves, [&](ChessMove move) {
        const ChessPiece target = board.get_piece(move.get_dst());
        if (target == EMPTY_SQUARE) {
            return 7;
        } else {
            return static_cast<int>(target.get_type());
        }
    });
}

DEFINE_PREFERENCE(Swarm) {
    const ChessSquare enemy_king_location =
        interface.get_current_pos().get_enemy_king_location();
    return minimal_elements(allowed_moves, [&](ChessMove move) {
        return enemy_king_location.distance(move.get_dst()) -
               enemy_king_location.distance(move.get_src());
    });
}

DEFINE_PREFERENCE(Huddle) {
    const ChessSquare king_location =
        interface.get_current_pos().get_king_location();
    return minimal_elements(allowed_moves, [&](ChessMove move) {
        return king_location.distance(move.get_dst()) -
               king_location.distance(move.get_src());
    });
}

DEFINE_PREFERENCE(Sniper) {
    return maximal_elements(allowed_moves, [&](ChessMove move) {
        return move.distance();
    });
}

DEFINE_PREFERENCE(Sloth) {
    return minimal_elements(allowed_moves, [&](ChessMove move) {
        return move.distance();
    });
}

DEFINE_PREFERENCE(Conqueror) {
    const PieceColor self = interface.get_color_to_move();
    return maximal_elements(allowed_moves, [&](ChessMove move) {
        ChessPosition copy = interface.get_current_pos();
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

DEFINE_PREFERENCE(Constrictor) {
    const PieceColor self = interface.get_color_to_move();
    const PieceColor enemy = !self;
    return minimal_elements(allowed_moves, [&](ChessMove move) {
        ChessPosition copy = interface.get_current_pos();
        copy.make_move(move);
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

DEFINE_PREFERENCE(Reinforced) {
    const PieceColor self = interface.get_color_to_move();
    return maximal_elements(allowed_moves, [&](ChessMove move) {
        ChessPosition copy = interface.get_current_pos();
        copy.make_move(move);
        return copy.get_board().is_attacked_by(self, move.get_dst());
    });
}

DEFINE_PREFERENCE(Outpost) {
    const PieceColor self = interface.get_color_to_move();
    const PieceColor enemy = !self;
    const ChessBoard &board = interface.get_current_pos().get_board();
    return minimal_elements(allowed_moves, [&](ChessMove move) {
        return board.is_attacked_by(enemy, move.get_dst());
    });
}

DEFINE_PREFERENCE(Gambit) {
    const PieceColor self = interface.get_color_to_move();
    const PieceColor enemy = !self;
    const ChessBoard &board = interface.get_current_pos().get_board();
    return maximal_elements(allowed_moves, [&](ChessMove move) {
        return board.is_attacked_by(self, move.get_dst()) &&
               board.is_attacked_by(enemy, move.get_dst());
    });
}

DEFINE_PREFERENCE(Explore) {
    const PieceColor self = interface.get_color_to_move();
    const PieceColor enemy = !self;
    const ChessBoard &board = interface.get_current_pos().get_board();
    return minimal_elements(allowed_moves, [&](ChessMove move) {
        return board.is_attacked_by(self, move.get_dst()) ||
               board.is_attacked_by(enemy, move.get_dst());
    });
}

DEFINE_PREFERENCE(Coward) {
    const PieceColor self = interface.get_color_to_move();
    const PieceColor enemy = !self;
    const ChessBoard &board = interface.get_current_pos().get_board();
    return maximal_elements(allowed_moves, [&](ChessMove move) {
        return board.is_attacked_by(enemy, move.get_src());
    });
}

DEFINE_PREFERENCE(Hero) {
    const PieceColor self = interface.get_color_to_move();
    const PieceColor enemy = !self;
    const ChessBoard &board = interface.get_current_pos().get_board();
    return minimal_elements(allowed_moves, [&](ChessMove move) {
        return board.is_attacked_by(enemy, move.get_src());
    });
}

#undef DEFINE_PREFERENCE

} // namespace Preference


namespace Engine {


PreferenceChain::PreferenceChain(const std::vector<PreferenceToken> &tokens)
    : rng(properly_seeded_random_engine())
    , preferences() {
    std::ostringstream name_builder;
    for (PreferenceToken token : tokens) {
        switch (token) {

#define CREATE_PREFERENCE_CASE(CLASS_NAME, TOKEN_NAME, STRING_NAME, COMMENT)   \
    case PreferenceToken::TOKEN_NAME:                                          \
        preferences.push_back(std::make_unique<Preference::CLASS_NAME>());     \
        name_builder << STRING_NAME;                                           \
        break;

            DECLARE_PREFERENCES(CREATE_PREFERENCE_CASE)

#undef CREATE_PREFERENCE_CASE
        }
    }
    name = name_builder.str();
}


ChessMove PreferenceChain::pick_move(
    ChessEngineInterface &interface,
    [[maybe_unused]] const std::vector<ChessPosition> &pos_history,
    [[maybe_unused]] const std::vector<ChessMove> &move_history
) {
    std::vector<ChessMove> allowed_moves = interface.get_legal_moves();
    for (const std::unique_ptr<ChessPreference> &pref : preferences) {
        if (allowed_moves.size() <= 1) { break; }
        allowed_moves = pref->pick_preferred_moves(interface, allowed_moves);
    }
    assert(!allowed_moves.empty());
    if (allowed_moves.size() == 1) {
        return allowed_moves[0];
    } else {
        return random_choice(rng, allowed_moves);
    }
}


const std::string &PreferenceChain::get_name() noexcept { return name; }


} // namespace Engine
