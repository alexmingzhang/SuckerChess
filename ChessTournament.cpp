// TODO: add tournament name in PGN of individual games
// TODO: Be able to get the white and black players from the game itself (have
// game_history as tuple of game and player pointers, or derive
// ChessTournamentGame class which adds the necessary data)

#include "ChessTournament.hpp"

#include <cstddef>  // for std::size_t
#include <iomanip>  // for std::setw
#include <iostream> // for std::cout, std::endl
#include <sstream>  // for std::ostringstream


const std::string &ChessTournament::get_name() const { return name; }


const std::vector<ChessPlayer *> &ChessTournament::get_players() const {
    return players;
}

const std::vector<ChessGame> &ChessTournament::get_game_history() const {
    return game_history;
}

void ChessTournament::add_player(ChessPlayer *player) {
    players.push_back(player);
}

void ChessTournament::sort_players_by_elo() {
    std::sort(
        players.begin(),
        players.end(),
        [](ChessPlayer *a, ChessPlayer *b) {
            return a->get_elo() > b->get_elo();
        }
    );
}

// Run every possible distinct matchup
void ChessTournament::run(int num_rounds, int print_frequency) {
    bool disable_printing = print_frequency == -1;
    bool verbose = print_frequency == 0;

    while (num_rounds == -1 ||
           this->current_round < static_cast<std::size_t>(num_rounds)) {

        if (!disable_printing && current_round % print_frequency == 0) {
            // sort_players_by_elo();
            print_info();
        }

        if (verbose) { std::cout << "ROUND " << current_round << std::endl; }

        for (std::size_t i = 0; i < players.size(); ++i) {
            ChessPlayer *p1 = players[i];

            for (std::size_t j = i + 1; j < players.size(); ++j) {
                ChessPlayer *p2 = players[j];

                if (verbose) { std::cout << current_game_index << ": "; }
                game_history.push_back(p1->versus(p2, verbose));
                current_game_index++;

                if (verbose) { std::cout << current_game_index << ": "; }
                game_history.push_back(p2->versus(p1, verbose));
                current_game_index++;
            }
        }

        current_round++;
    }
}

void ChessTournament::print_info() const {
    std::cout << name << " (round " << current_round << ", game "
              << current_game_index << ") \n";
    std::cout << "    Engine      ELO       Wins (W/B)  Draws   Losses (W/B)\n";

    auto concat_stats = [](unsigned int a, unsigned int b) {
        std::ostringstream result;
        result << a << "/" << b;
        return result.str();
    };

    for (std::size_t i = 0; i < players.size(); ++i) {
        const ChessPlayer *p = players[i];
        std::cout << std::right << std::setw(2) << i + 1 << ". " << std::left
                  << std::setw(12) << p->get_name() << std::setw(10)
                  << std::fixed << std::setprecision(2) << p->get_elo()
                  << std::setw(12)
                  << concat_stats(
                         p->get_num_wins_as_white(), p->get_num_wins_as_black()
                     )
                  << std::setw(8) << p->get_num_draws() << std::setw(12)
                  << concat_stats(
                         p->get_num_losses_as_white(),
                         p->get_num_losses_as_black()
                     )
                  << '\n';
    }
}
