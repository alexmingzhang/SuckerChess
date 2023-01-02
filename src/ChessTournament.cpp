// TODO: add tournament name in PGN of individual games
// TODO: Be able to get the white and black players from the game itself (have
// game_history as tuple of game and player pointers, or derive
// ChessTournamentGame class which adds the necessary data)

#include "ChessTournament.hpp"
#include "Utilities.hpp"

#include <algorithm> // for std::sort
#include <array>     // for std::array
#include <cmath>     // for std::tgamma
#include <cstddef>   // for std::size_t
#include <iomanip> // for std::setw, std::left, std::right, std::fixed, std::setprecision
#include <iostream> // for std::cout, std::endl, std::flush
#include <utility>  // for std::make_pair


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
    long long num_rounds, long long print_frequency, bool store_games
) {

    const bool infinite_rounds = (num_rounds == -1);
    const bool enable_printing = (print_frequency != -1);
    const bool verbose = (print_frequency == 0);

    while (infinite_rounds || (this->current_round < num_rounds)) {

        if (enable_printing && (current_round % print_frequency == 0)) {
            sort_players_by_elo();
            print_info();
        }

        if (verbose) { std::cout << "ROUND " << current_round << std::endl; }

        // First player is white, second player is black
        std::vector<std::pair<ChessPlayer *, ChessPlayer *>> matchups;

        std::size_t num_players = players.size();
        matchups.reserve(num_players * (num_players - 1));

        for (std::size_t i = 0; i < players.size(); ++i) {
            ChessPlayer *p1 = players[i].get();
            for (std::size_t j = i + 1; j < players.size(); ++j) {
                ChessPlayer *p2 = players[j].get();
                matchups.emplace_back(p1, p2);
                matchups.emplace_back(p2, p1);
            }
        }

        std::shuffle(matchups.begin(), matchups.end(), rng);

        for (auto &matchup : matchups) {
            ChessGame game =
                (matchup.first)->versus(*matchup.second, elo_k_factor, verbose);

            if (store_games) { game_history.push_back(game); }
        }

        // ELO K-Factor decay
        if (infinite_rounds) {
            elo_k_factor *= 0.99;
        } else {
            elo_k_factor -= 40.0 / static_cast<double>(num_rounds);
        }

        ++current_round;
    }

    sort_players_by_elo();
    print_info();
}


void ChessTournament::print_info() const {
    std::cout << name << " (round " << current_round << ", game "
              << game_history.size() << ", K-factor " << elo_k_factor << ") \n";
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
