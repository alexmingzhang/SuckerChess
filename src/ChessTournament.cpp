#include "ChessTournament.hpp"
#include "ChessEngine.hpp"
#include "Utilities.hpp"

#include <algorithm> // for std::find, std::sort, std::shuffle
#include <array>     // for std::array
#include <cstddef>   // for std::size_t
#include <cstdint>   // for std::uint8_t
#include <iomanip> // for std::setw, std::left, std::right, std::fixed, std::setprecision
#include <iostream> // for std::cout, std::endl, std::flush
#include <utility>  // for std::pair, std::make_pair


void ChessTournament::add_engine(std::unique_ptr<ChessEngine> &&engine
) noexcept {
    engines.emplace_back(std::move(engine), PerformanceInfo{0, 0, 0, 0, 0});
}


void ChessTournament::sort_players_by_wins() {
    std::sort(
        engines.begin(),
        engines.end(),
        [&](const auto &a, const auto &b) {
            return a.second.total_wins() > b.second.total_wins();
        }
    );
}


void ChessTournament::sort_players_by_win_ratio() {
    std::sort(
        engines.begin(),
        engines.end(),
        [&](const auto &a, const auto &b) {
            const unsigned long long a_wins = a.second.total_wins();
            const unsigned long long a_losses = a.second.total_losses();
            const unsigned long long b_wins = b.second.total_wins();
            const unsigned long long b_losses = b.second.total_losses();

            const double a_ratio =
                static_cast<double>(a_wins) / static_cast<double>(a_losses);
            const double b_ratio =
                static_cast<double>(b_wins) / static_cast<double>(b_losses);

            if (a_ratio == b_ratio) {
                if (a_wins == b_wins) { return a_losses < b_losses; }
                return a_wins > b_wins;
            }
            return a_ratio > b_ratio;
        }
    );
}


void ChessTournament::run(long long num_rounds, long long print_frequency) {

    const bool infinite_rounds = (num_rounds == -1);
    const bool enable_printing = (print_frequency != -1);
    const bool verbose = (print_frequency == 0);

    // Pairs of player indices; 1st player is white, 2nd player is black
    std::vector<std::pair<std::size_t, std::size_t>> matchups;
    for (std::size_t i = 0; i < engines.size(); ++i) {
        for (std::size_t j = 0; j < engines.size(); ++j) {
            if (i != j) { matchups.emplace_back(i, j); }
        }
    }

    const long long final_round = current_round + num_rounds;
    while (infinite_rounds || (current_round < final_round)) {

        if (verbose) {
            std::cout << name << ", round " << current_round << std::endl;
        }

        // Randomize all matchups
        std::shuffle(matchups.begin(), matchups.end(), rng);

        // Play each matchup
        for (const auto [i, j] : matchups) {
            auto &[white_engine, white_info] = engines[i];
            auto &[black_engine, black_info] = engines[j];
            ChessGame game;
            PieceColor winner =
                game.run(white_engine.get(), black_engine.get(), false);
            switch (winner) {
                case PieceColor::NONE:
                    ++white_info.num_draws;
                    ++black_info.num_draws;
                    break;
                case PieceColor::WHITE:
                    ++white_info.num_wins_as_white;
                    ++black_info.num_losses_as_black;
                    break;
                case PieceColor::BLACK:
                    ++white_info.num_losses_as_white;
                    ++black_info.num_wins_as_black;
                    break;
            }
        }

        const bool should_print =
            verbose ||
            (enable_printing && (current_round % print_frequency == 0));

        if (should_print) {
            sort_players_by_win_ratio();
            print_info();
        }

        ++current_round;
    }
}


void ChessTournament::print_info() const {

    int max_name_width = 6;
    for (const auto &[engine, info] : engines) {
        const int name_width = static_cast<int>(engine->get_name().size());
        if (name_width > max_name_width) { max_name_width = name_width; }
    }

    const std::size_t num_games = static_cast<std::size_t>(current_round) *
                                  engines.size() * (engines.size() - 1);

    std::cout << name << " Results (" << current_round + 1 << " rounds, "
              << num_games << " games) \n";
    std::cout << "      " << std::setw(max_name_width) << "Engine"
              << " :   W (w/b)   :   D   :   L (w/b)   : "
                 "  WLR   \n";

    for (std::size_t i = 0; i < engines.size(); ++i) {
        const auto &[engine, info] = engines[i];
        std::cout << std::right << std::setw(4) << (i + 1) << ". ";
        std::cout << std::left << std::setw(max_name_width)
                  << engine->get_name() << " : ";
        std::cout << std::right << std::setw(5) << info.num_wins_as_white << '/'
                  << std::left << std::setw(5) << info.num_wins_as_black
                  << " : ";
        std::cout << std::right << std::setw(5) << info.num_draws << " : ";
        std::cout << std::right << std::setw(5) << info.num_losses_as_white
                  << '/' << std::left << std::setw(5)
                  << info.num_losses_as_black << " : ";
        std::cout << std::setw(5)
                  << (static_cast<double>(info.total_wins()) /
                      static_cast<double>(info.total_losses()))
                  << '\n';
    }
    std::cout << std::flush;
}


// enum class MutationToken : std::uint8_t {
//     INSERT, // insert distinct random preference
//     DELETE, // delete random preference
//     SWAP,   // swap two random preferences
//     REPLACE // replace random preference with a new distinct random
//     preference
// };


// void ChessTournament::evolve(int num_replace, bool verbose) {
//     using enum PreferenceToken;
//
//     constexpr std::array<PreferenceToken, 26> token_pool = {
//         MATE_IN_ONE,   PREVENT_MATE_IN_ONE,
//         PREVENT_DRAW,  CHECK,
//         CAPTURE,       CAPTURE_HANGING,
//         SMART_CAPTURE, CASTLE,
//         FIRST,         LAST,
//         EXTEND,        REDUCE,
//         GREEDY,        GENEROUS,
//         SWARM,         HUDDLE,
//         SNIPER,        SLOTH,
//         CONQUEROR,     CONSTRICTOR,
//         REINFORCED,    OUTPOST,
//         GAMBIT,        EXPLORE,
//         COWARD,        HERO};
//
//     std::size_t original_num_players = players.size();
//
//     // Kill off players
//     sort_players_by_win_ratio();
//
//     if (verbose) {
//         std::cout << "Killing off " << num_replace << " players: ";
//         for (auto it = players.end() - num_replace; it != players.end();
//         ++it) {
//             std::cout << (*it)->get_name() << ' ';
//         }
//         std::cout << '\n';
//     }
//
//     players.erase(players.end() - num_replace, players.end());
//
//     // Create children from the top players
//     for (std::size_t i = 0; i < num_replace; ++i) {
//         std::unique_ptr<ChessPlayer> &player =
//             players[i % (original_num_players - num_replace)];
//         if (verbose) { std::cout << player->get_name() << "'s child has "; }
//
//         std::vector<PreferenceToken> mutated_tokens =
//             player->get_preference_tokens();
//
//         // Create distribution of possible mutations
//         std::discrete_distribution<MutationToken> mutate_dist{
//             static_cast<double>(mutated_tokens.size() < token_pool.size()),
//             static_cast<double>(mutated_tokens.size() > 0),
//             static_cast<double>(mutated_tokens.size() > 1),
//             static_cast<double>(
//                 mutated_tokens.size() > 0 &&
//                 mutated_tokens.size() < token_pool.size()
//             )};
//
//         MutationToken mutate_choice = mutate_dist(rng);
//         switch (mutate_choice) {
//             case MutationToken::INSERT:
//                 {
//                     if (verbose) {
//                         std::cout << "added a token to become " <<
//                         std::flush;
//                     }
//
//                     PreferenceToken new_token;
//                     std::uniform_int_distribution<
//                         typename std::vector<PreferenceToken>::size_type>
//                         token_pool_dist(0, token_pool.size());
//                     do {
//                         new_token = token_pool[token_pool_dist(rng)];
//                     } while (std::find(
//                                  mutated_tokens.begin(),
//                                  mutated_tokens.end(),
//                                  new_token
//                              ) != mutated_tokens.end());
//
//                     std::uniform_int_distribution<
//                         typename std::vector<PreferenceToken>::size_type>
//                         current_token_dist(0, mutated_tokens.size());
//                     mutated_tokens.insert(
//                         mutated_tokens.begin() + current_token_dist(rng),
//                         new_token
//                     );
//                     break;
//                 }
//             case MutationToken::DELETE:
//                 {
//                     if (verbose) {
//                         std::cout << "deleted a token to become " <<
//                         std::flush;
//                     }
//
//                     std::uniform_int_distribution<
//                         typename std::vector<PreferenceToken>::size_type>
//                         current_token_dist(0, mutated_tokens.size() - 1);
//
//                     mutated_tokens.erase(
//                         mutated_tokens.begin() + current_token_dist(rng)
//                     );
//                     break;
//                 }
//             case MutationToken::SWAP:
//                 {
//                     if (verbose) {
//                         std::cout << "swapped two tokens to become "
//                                   << std::flush;
//                     }
//
//                     std::uniform_int_distribution<
//                         typename std::vector<PreferenceToken>::size_type>
//                         current_token_dist(0, mutated_tokens.size() - 1);
//                     std::swap(
//                         mutated_tokens[current_token_dist(rng)],
//                         mutated_tokens[current_token_dist(rng)]
//                     );
//                     break;
//                 }
//             case MutationToken::REPLACE:
//                 {
//                     if (verbose) {
//                         std::cout << "replaced a token to become "
//                                   << std::flush;
//                     }
//
//                     PreferenceToken new_token;
//                     std::uniform_int_distribution<
//                         typename std::vector<PreferenceToken>::size_type>
//                         token_pool_dist(0, token_pool.size());
//                     do {
//                         new_token = token_pool[token_pool_dist(rng)];
//                     } while (std::find(
//                                  mutated_tokens.begin(),
//                                  mutated_tokens.end(),
//                                  new_token
//                              ) != mutated_tokens.end());
//
//                     std::uniform_int_distribution<
//                         typename std::vector<PreferenceToken>::size_type>
//                         current_token_dist(0, mutated_tokens.size() - 1);
//                     mutated_tokens[current_token_dist(rng)] = new_token;
//                     break;
//                 }
//             default: __builtin_unreachable();
//         }
//
//         std::unique_ptr<ChessPlayer> child =
//             std::make_unique<ChessPlayer>(mutated_tokens);
//
//         if (verbose) { std::cout << child->get_name() << std::endl; }
//
//         add_player(std::move(child));
//     }
// }
