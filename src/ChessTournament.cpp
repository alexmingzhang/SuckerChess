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

void ChessTournament::evolve(bool verbose) {
    using enum PreferenceToken;

    constexpr std::array<PreferenceToken, 24> token_pool = {
        MATE_IN_ONE,   PREVENT_MATE_IN_ONE,
        PREVENT_DRAW,  CHECK,
        CAPTURE,       CAPTURE_HANGING,
        SMART_CAPTURE, CASTLE,
        FIRST,         LAST,
        REDUCE,        GREEDY,
        SWARM,         HUDDLE,
        SNIPER,        SLOTH,
        CONQUEROR,     CONSTRICTOR,
        REINFORCED,    OUTPOST,
        GAMBIT,        EXPLORE,
        COWARD,        HERO};

    // Kill off the lower half of players
    sort_players_by_win_ratio();

    if (verbose) {
        for (auto it = players.begin() + players.size() / 2;
             it != players.end();
             ++it) {
            std::cout << "Killing off " << (*it)->get_name() << '\n';
        }
    }

    players.erase(players.begin() + players.size() / 2, players.end());

    // Create children from the top half of players
    for (std::unique_ptr<ChessPlayer> &player : players) {
        if (verbose) { std::cout << player->get_name() << "'s child has "; }

        std::vector<PreferenceToken> mutated_tokens =
            player->get_preference_tokens();

        std::uniform_int_distribution<int> mutate_dist(0, 2);

        // Introduce "mutations"
        switch (mutate_dist(rng)) {
            case 0:
                { // Add random non-duplicate token
                    if (mutated_tokens.size() == token_pool.size()) {
                        goto case1;
                    }

                    if (verbose) { std::cout << "added a token to become "; }

                    PreferenceToken token;
                    std::uniform_int_distribution<
                        typename std::vector<PreferenceToken>::size_type>
                        token_pool_dist(0, token_pool.size());
                    do {
                        token = token_pool[token_pool_dist(rng)];
                    } while (
                        std::find(
                            mutated_tokens.begin(), mutated_tokens.end(), token
                        ) != mutated_tokens.end()
                    );

                    std::uniform_int_distribution<
                        typename std::vector<PreferenceToken>::size_type>
                        current_token_dist(0, mutated_tokens.size());
                    mutated_tokens.insert(
                        mutated_tokens.begin() + current_token_dist(rng), token
                    );
                    break;
                }
            case1:
            case 1:
                { // Remove random token
                    if (mutated_tokens.size() <= 1) { goto case2; }

                    if (verbose) { std::cout << "removed a token to become"; }

                    std::uniform_int_distribution<
                        typename std::vector<PreferenceToken>::size_type>
                        current_token_dist(0, mutated_tokens.size() - 1);

                    mutated_tokens.erase(
                        mutated_tokens.begin() + current_token_dist(rng)
                    );

                    break;
                }
            case2:
            case 2:
                { // Swap random tokens
                    std::cout << "swapped two tokens to become ";
                    std::uniform_int_distribution<
                        typename std::vector<PreferenceToken>::size_type>
                        current_token_dist(0, mutated_tokens.size() - 1);
                    std::swap(
                        mutated_tokens[current_token_dist(rng)],
                        mutated_tokens[current_token_dist(rng)]
                    );
                    break;
                }
            default: __builtin_unreachable();
        }

        std::unique_ptr<ChessPlayer> child =
            std::make_unique<ChessPlayer>(mutated_tokens);

        if (verbose) { std::cout << child->get_name() << '\n'; }

        add_player(std::move(child));
    }
}

void ChessTournament::print_info() const {
    std::cout << name << " Results (" << current_round + 1 << " rounds, "
              << current_round * num_games_per_round
              << " games, K-factor = " << elo_k_factor << ") \n";
    std::cout
        << "      Engine       :   ELO   :   W (w/b)   :   D   :   L (w/b)  \n";
    for (std::size_t i = 0; i < players.size(); ++i) {
        const ChessPlayer &p = *players[i];
        std::cout << std::right << std::setw(4) << i + 1 << ". ";
        std::cout << std::left << std::setw(12) << p.get_name() << " : ";
        std::cout << std::right << std::setw(7) << std::fixed
                  << std::setprecision(2) << p.get_elo() << " : ";
        std::cout << std::right << std::setw(5) << p.get_num_wins_as_white()
                  << '/' << std::left << std::setw(5)
                  << p.get_num_wins_as_black() << " : ";
        std::cout << std::right << std::setw(5) << p.get_num_draws() << " : ";
        std::cout << std::right << std::setw(5) << p.get_num_losses_as_white()
                  << '/' << std::left << std::setw(5)
                  << p.get_num_losses_as_black();
        std::cout << '\n';
    }
    std::cout << std::flush;
}
