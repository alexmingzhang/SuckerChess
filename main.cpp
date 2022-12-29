#include <iostream>
#include <memory>
#include <sstream>
#include <utility>
#include <vector>

#include "src/ChessEngine.hpp"
#include "src/ChessGame.hpp"
#include "src/ChessPlayer.hpp"
#include "src/ChessPosition.hpp"
#include "src/ChessTournament.hpp"
#include "src/Utilities.hpp"


[[maybe_unused]] static void self_test() {
    auto rng = properly_seeded_random_engine();
    std::size_t count = 0;
    while (true) {
        ChessPosition pos;
        while (true) {
            if (!pos.check_consistency()) {
                std::cerr << "FATAL ERROR: Inconsistent chess position found."
                          << std::endl;
                std::cerr << pos << std::endl;
                std::cerr << pos.get_fen() << std::endl;
                return;
            }
            if (pos.get_board().has_insufficient_material()) { break; }
            const auto moves = pos.get_legal_moves();
            if (moves.empty()) { break; }
            const auto chosen_move = random_choice(rng, moves);
            if (!pos.is_valid(chosen_move)) {
                std::cerr << "FATAL ERROR: Invalid chess move found."
                          << std::endl;
                std::cerr << pos << std::endl;
                std::cerr << pos.get_fen() << std::endl;
                std::cerr << chosen_move << std::endl;
                return;
            }
            if (!pos.is_legal(chosen_move)) {
                std::cerr << "FATAL ERROR: Illegal chess move found."
                          << std::endl;
                std::cerr << pos << std::endl;
                std::cerr << pos.get_fen() << std::endl;
                std::cerr << chosen_move << std::endl;
                return;
            }
            pos.make_move(random_choice(rng, moves));
        }
        ++count;
        std::cout << "Completed " << count
                  << ((count == 1) ? " random self-test game."
                                   : " random self-test games.")
                  << std::endl;
    }
}


//[[maybe_unused]] static void benchmark(unsigned long long num_games) {
//    ChessEngine *const white = new Engine::Random();
//    ChessEngine *const black = new Engine::Random();
//    for (unsigned long long i = 0; i < num_games; ++i) {
//        ChessGame game;
//        game.run(white, black, false);
//    }
//    delete white;
//    delete black;
//}


enum class PrefToken {

    MATE_IN_ONE,
    CHECK,
    CAPTURE,

    FIRST, // first available move
    LAST,  // last available move

    REDUCE, // minimize available moves for opponent

    SWARM,     // moves pieces toward enemy king
    HUDDLE,    // moves pieces toward own king
    ENERGETIC, // makes long moves
    LAZY,      // makes short moves

    COORDINATED, // maximizes number of squares attacked by self
    BLOCKADE,    // minimizes number of squares attacked by opponent
    DEFENDER,    // moves pieces to squares attacked by self
    OUTPOST,     // moves pieces to squares which are not attacked by opponent
    GAMBIT,      // moves pieces to squares attacked by both self and opponent

}; // enum class PrefToken


static std::unique_ptr<ChessPlayer> preference_player_factory(
    const std::vector<PrefToken> &tokens, const std::string &name = ""
) {
    using namespace Preference;
    using enum PrefToken;
    std::ostringstream oss;
    std::unique_ptr<Engine::Preference> pref =
        std::make_unique<Engine::Preference>();
    for (PrefToken token : tokens) {
        // clang-format off
        switch (token) {
            case MATE_IN_ONE: oss << 'M'; pref->add<MateInOne>();   break;
            case CHECK:       oss << 'C'; pref->add<Check>();       break;
            case CAPTURE:     oss << 'X'; pref->add<Capture>();     break;
            case FIRST:       oss << 'A'; pref->add<Capture>();     break;
            case LAST:        oss << 'Z'; pref->add<Capture>();     break;
            case REDUCE:      oss << 'R'; pref->add<Reduce>();      break;
            case SWARM:       oss << 'S'; pref->add<Swarm>();       break;
            case HUDDLE:      oss << 'H'; pref->add<Huddle>();      break;
            case ENERGETIC:   oss << 'E'; pref->add<Energetic>();   break;
            case LAZY:        oss << 'L'; pref->add<Lazy>();        break;
            case COORDINATED: oss << 'I'; pref->add<Coordinated>(); break;
            case BLOCKADE:    oss << 'B'; pref->add<Blockade>();    break;
            case DEFENDER:    oss << 'D'; pref->add<Defender>();    break;
            case OUTPOST:     oss << 'O'; pref->add<Outpost>();     break;
            case GAMBIT:      oss << 'G'; pref->add<Gambit>();      break;
        }
        // clang-format on
    }
    // A B C D E   G H I     L M   O     R S         X   Z
    //           F       J K     N   P Q     T U V W   Y
    return std::make_unique<ChessPlayer>(
        name.empty() ? oss.str() : name, std::move(pref)
    );
}


[[maybe_unused]] static void run_tournament() {

    using enum PrefToken;
    ChessTournament tournament;

    tournament.add_player(preference_player_factory({MATE_IN_ONE}));

    const std::vector<PrefToken> tokens = {CAPTURE, REDUCE, ENERGETIC, LAZY};

    for (PrefToken t1 : tokens) {
        for (PrefToken t2 : tokens) {
            if (t1 == t2) {
                tournament.add_player(
                    preference_player_factory({MATE_IN_ONE, t1})
                );
            } else {
                tournament.add_player(
                    preference_player_factory({MATE_IN_ONE, t1, t2})
                );
            }
        }
    }

    tournament.add_player(
        std::make_unique<ChessPlayer>("CCCP", std::make_unique<Engine::CCCP>())
    );
    tournament.add_player(
        preference_player_factory({MATE_IN_ONE, CHECK, CAPTURE, SWARM})
    );

    tournament.run(-1, 1);
}


int main() { run_tournament(); }
