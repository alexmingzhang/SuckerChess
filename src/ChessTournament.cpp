// TODO: add tournament name in PGN of individual games
// TODO: Be able to get the white and black players from the game itself (have
// game_history as tuple of game and player pointers, or derive
// ChessTournamentGame class which adds the necessary data)

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


void ChessTournament::add_player(std::unique_ptr<ChessPlayer> &&player
) noexcept {
    players.push_back(std::move(player));
    const long long num_players = static_cast<long long>(players.size());
    num_games_per_round = (num_players * (num_players - 1));
}


void ChessTournament::sort_players_by_elo() {
    std::sort(
        players.begin(),
        players.end(),
        [&](const std::unique_ptr<ChessPlayer> &a,
            const std::unique_ptr<ChessPlayer> &b) {
            return a->get_elo() > b->get_elo();
        }
    );
}

void ChessTournament::sort_players_by_wins() {
    std::sort(
        players.begin(),
        players.end(),
        [&](const std::unique_ptr<ChessPlayer> &a,
            const std::unique_ptr<ChessPlayer> &b) {
            return a->get_num_wins() > b->get_num_wins();
        }
    );
}

void ChessTournament::sort_players_by_win_ratio() {
    std::sort(
        players.begin(),
        players.end(),
        [&](const std::unique_ptr<ChessPlayer> &a,
            const std::unique_ptr<ChessPlayer> &b) {
            double a_wlr = static_cast<double>(a->get_num_wins()) /
                           static_cast<double>(a->get_num_losses());
            double b_wlr = static_cast<double>(b->get_num_wins()) /
                           static_cast<double>(b->get_num_losses());

            if (a_wlr == b_wlr) {
                if (a->get_num_wins() == b->get_num_wins()) {
                    return a->get_num_losses() < b->get_num_losses();
                }

                return a->get_num_wins() > b->get_num_wins();
            }

            return a_wlr > b_wlr;
        }
    );
}

void ChessTournament::run(
    long long num_rounds,
    double elo_k_factor_decay,
    long long print_frequency,
    bool store_games
) {
    const bool infinite_rounds = (num_rounds == -1);
    const bool enable_printing = (print_frequency != -1);
    const bool verbose = (print_frequency == 0);

    // Pairs of player indices; 1st player is white, 2nd player is black
    std::vector<std::pair<std::size_t, std::size_t>> matchups;
    matchups.resize(static_cast<unsigned long long>(num_games_per_round));

    long long num_final_round = current_round + num_rounds;

    while (infinite_rounds || (current_round < num_final_round)) {
        if (verbose) {
            std::cout << name << ", round " << current_round << std::endl;
        }

        // Generate each distinct matchup
        std::size_t count = 0;
        for (std::size_t i = 0; i < players.size(); ++i) {
            for (std::size_t j = i + 1; j < players.size(); ++j) {
                matchups[count++] = std::make_pair(i, j);
                matchups[count++] = std::make_pair(j, i);
            }
        }

        // Randomize all matchups
        std::shuffle(matchups.begin(), matchups.end(), rng);

        // Play each matchup

        for (auto &matchup : matchups) {
            ChessGame game = (players[matchup.first])
                                 ->versus(*players[matchup.second], verbose);

            if (store_games) { game_history.push_back(game); }
        }

        // Update ELO
        for (auto &player : players) { player->update_elo(elo_k_factor); }

        if (verbose ||
            (enable_printing && (current_round % print_frequency == 0))) {
            sort_players_by_win_ratio();
            print_info();
        }

        elo_k_factor *= elo_k_factor_decay;
        ++current_round;
    }
}

enum class MutationToken : std::uint8_t {
    INSERT, // insert distinct random preference
    DELETE, // delete random preference
    SWAP,   // swap two random preferences
    REPLACE // replace random preference with a new distinct random preference
};

void ChessTournament::evolve(int num_replace, bool verbose) {
    using enum PreferenceToken;

    constexpr std::array<PreferenceToken, 26> token_pool = {
        MATE_IN_ONE,   PREVENT_MATE_IN_ONE,
        PREVENT_DRAW,  CHECK,
        CAPTURE,       CAPTURE_HANGING,
        SMART_CAPTURE, CASTLE,
        FIRST,         LAST,
        EXTEND,        REDUCE,
        GREEDY,        GENEROUS,
        SWARM,         HUDDLE,
        SNIPER,        SLOTH,
        CONQUEROR,     CONSTRICTOR,
        REINFORCED,    OUTPOST,
        GAMBIT,        EXPLORE,
        COWARD,        HERO};

    std::size_t original_num_players = players.size();

    // Kill off players
    sort_players_by_win_ratio();

    if (verbose) {
        std::cout << "Killing off " << num_replace << " players: ";
        for (auto it = players.end() - num_replace; it != players.end(); ++it) {
            std::cout << (*it)->get_name() << ' ';
        }
        std::cout << '\n';
    }

    players.erase(players.end() - num_replace, players.end());

    // Create children from the top players
    for (std::size_t i = 0; i < num_replace; ++i) {
        std::unique_ptr<ChessPlayer> &player =
            players[i % (original_num_players - num_replace)];
        if (verbose) { std::cout << player->get_name() << "'s child has "; }

        std::vector<PreferenceToken> mutated_tokens =
            player->get_preference_tokens();

        // Create distribution of possible mutations
        std::discrete_distribution<MutationToken> mutate_dist{
            static_cast<double>(mutated_tokens.size() < token_pool.size()),
            static_cast<double>(mutated_tokens.size() > 0),
            static_cast<double>(mutated_tokens.size() > 1),
            static_cast<double>(
                mutated_tokens.size() > 0 &&
                mutated_tokens.size() < token_pool.size()
            )};

        MutationToken mutate_choice = mutate_dist(rng);
        switch (mutate_choice) {
            case MutationToken::INSERT:
                {
                    if (verbose) {
                        std::cout << "added a token to become " << std::flush;
                    }

                    PreferenceToken new_token;
                    std::uniform_int_distribution<
                        typename std::vector<PreferenceToken>::size_type>
                        token_pool_dist(0, token_pool.size());
                    do {
                        new_token = token_pool[token_pool_dist(rng)];
                    } while (std::find(
                                 mutated_tokens.begin(),
                                 mutated_tokens.end(),
                                 new_token
                             ) != mutated_tokens.end());

                    std::uniform_int_distribution<
                        typename std::vector<PreferenceToken>::size_type>
                        current_token_dist(0, mutated_tokens.size());
                    mutated_tokens.insert(
                        mutated_tokens.begin() + current_token_dist(rng),
                        new_token
                    );
                    break;
                }
            case MutationToken::DELETE:
                {
                    if (verbose) {
                        std::cout << "deleted a token to become " << std::flush;
                    }

                    std::uniform_int_distribution<
                        typename std::vector<PreferenceToken>::size_type>
                        current_token_dist(0, mutated_tokens.size() - 1);

                    mutated_tokens.erase(
                        mutated_tokens.begin() + current_token_dist(rng)
                    );
                    break;
                }
            case MutationToken::SWAP:
                {
                    if (verbose) {
                        std::cout << "swapped two tokens to become "
                                  << std::flush;
                    }

                    std::uniform_int_distribution<
                        typename std::vector<PreferenceToken>::size_type>
                        current_token_dist(0, mutated_tokens.size() - 1);
                    std::swap(
                        mutated_tokens[current_token_dist(rng)],
                        mutated_tokens[current_token_dist(rng)]
                    );
                    break;
                }
            case MutationToken::REPLACE:
                {
                    if (verbose) {
                        std::cout << "replaced a token to become "
                                  << std::flush;
                    }

                    PreferenceToken new_token;
                    std::uniform_int_distribution<
                        typename std::vector<PreferenceToken>::size_type>
                        token_pool_dist(0, token_pool.size());
                    do {
                        new_token = token_pool[token_pool_dist(rng)];
                    } while (std::find(
                                 mutated_tokens.begin(),
                                 mutated_tokens.end(),
                                 new_token
                             ) != mutated_tokens.end());

                    std::uniform_int_distribution<
                        typename std::vector<PreferenceToken>::size_type>
                        current_token_dist(0, mutated_tokens.size() - 1);
                    mutated_tokens[current_token_dist(rng)] = new_token;
                    break;
                }
            default: __builtin_unreachable();
        }

        std::unique_ptr<ChessPlayer> child =
            std::make_unique<ChessPlayer>(mutated_tokens);

        if (verbose) { std::cout << child->get_name() << std::endl; }

        add_player(std::move(child));
    }
}

void ChessTournament::print_info() const {
    std::size_t max_name_width = 6;
    for (const std::unique_ptr<ChessPlayer> &player : players) {
        std::size_t name_width = player->get_name().size();
        if (name_width > max_name_width) { max_name_width = name_width; }
    }

    std::cout << name << " Results (" << current_round + 1 << " rounds, "
              << (current_round + 1) * num_games_per_round
              << " games, K-factor = " << elo_k_factor << ") \n";
    std::cout << "      " << std::setw(max_name_width) << "Engine"
              << " :   ELO   :   W (w/b)   :   D   :   L (w/b)   : "
                 "  WLR   \n";

    for (std::size_t i = 0; i < players.size(); ++i) {
        const ChessPlayer &p = *players[i];
        std::cout << std::right << std::setw(4) << i + 1 << ". ";
        std::cout << std::left << std::setw(max_name_width) << p.get_name()
                  << " : ";
        std::cout << std::right << std::setw(7) << std::fixed
                  << std::setprecision(2) << p.get_elo() << " : ";
        std::cout << std::right << std::setw(5) << p.get_num_wins_as_white()
                  << '/' << std::left << std::setw(5)
                  << p.get_num_wins_as_black() << " : ";
        std::cout << std::right << std::setw(5) << p.get_num_draws() << " : ";
        std::cout << std::right << std::setw(5) << p.get_num_losses_as_white()
                  << '/' << std::left << std::setw(5)
                  << p.get_num_losses_as_black() << " : ";
        std::cout << std::setw(5)
                  << static_cast<double>(p.get_num_wins()) /
                         static_cast<double>(p.get_num_losses());
        std::cout << '\n';
    }
    std::cout << std::flush;
}
