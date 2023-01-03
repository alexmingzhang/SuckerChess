// TODO: add tournament name in PGN of individual games
// TODO: Be able to get the white and black players from the game itself (have
// game_history as tuple of game and player pointers, or derive
// ChessTournamentGame class which adds the necessary data)

#include "ChessTournament.hpp"
#include "Utilities.hpp"

#include <algorithm> // for std::sort, std::shuffle
#include <cstddef>   // for std::size_t
#include <iomanip> // for std::setw, std::left, std::right, std::fixed, std::setprecision
#include <iostream> // for std::cout, std::endl, std::flush
#include <utility>  // for std::pair, std::make_pair

void ChessTournament::add_player(std::unique_ptr<ChessPlayer> &&player
) noexcept {
    players.push_back(std::move(player));
    const std::size_t num_players = players.size();
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
    matchups.resize(num_games_per_round);

    while (infinite_rounds ||
           (current_round < static_cast<std::size_t>(num_rounds))) {
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
            (enable_printing &&
             (current_round % static_cast<std::size_t>(print_frequency) == 0)
            )) {
            sort_players_by_elo();
            print_info();
        }

        elo_k_factor *= elo_k_factor_decay;
        ++current_round;
    }

    sort_players_by_elo();
    print_info();
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
