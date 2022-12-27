// TODO: add tournament name in PGN of individual games
// TODO: Be able to get the white and black players from the game itself (have
// game_history as tuple of game and player pointers, or derive
// ChessTournamentGame class which adds the necessary data)


#include "ChessTournament.hpp"

#include <algorithm>
#include <iostream>

void ChessTournament::add_player(ChessPlayer *player) {
    players.push_back(player);
}

void ChessTournament::sort_players_by_elo() {
    std::sort(
        players.begin(),
        players.end(),
        [](ChessPlayer *a, ChessPlayer *b) {
            return a->get_elo() < b->get_elo();
        }
    );
}

const std::vector<ChessPlayer *> &ChessTournament::get_players() const {
    return players;
}

const std::vector<ChessGame> &ChessTournament::get_game_history() const {
    return game_history;
}

// Run every possible distinct matchup
void ChessTournament::run(unsigned int num_rounds, bool verbose) {
    for (unsigned int round = 0; round < num_rounds; ++round) {
        if (verbose) { std::cout << "ROUND " << round << std::endl; }

        for (std::size_t i = 0; i < players.size(); ++i) {
            ChessPlayer *p1 = players[i];

            for (std::size_t j = i + 1; j < players.size(); ++j) {
                ChessPlayer *p2 = players[j];

                std::cout << current_game_index << ": ";
                game_history.push_back(p1->versus(p2, verbose));
                current_game_index++;

                std::cout << current_game_index << ": ";
                game_history.push_back(p2->versus(p1, verbose));
                current_game_index++;
            }
        }
    }

    if (verbose) {
        std::cout << "Total games played: " << current_game_index << std::endl;
    }
}
