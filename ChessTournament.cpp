// TODO: add tournament name in PGN of individual games
// TODO: Be able to get the white and black players from the game itself (have
// game_history as tuple of game and player pointers, or derive
// ChessTournamentGame class which adds the necessary data)

#include "ChessTournament.hpp"

#include <cstddef>  // for std::size_t
#include <iostream> // for std::cout, std::endl


void ChessTournament::add_player(ChessPlayer *player) {
    players.push_back(player);
}


const std::vector<ChessPlayer *> &ChessTournament::get_players() const {
    return players;
}


const std::vector<ChessGame> &ChessTournament::get_game_history() const {
    return game_history;
}


// Run every possible distinct matchup
void ChessTournament::run(unsigned int times) {
    for (unsigned int n = 0; n < times; ++n) {
        std::cout << "ROUND " << n << std::endl;
        for (std::size_t i = 0; i < players.size(); ++i) {
            ChessPlayer *p1 = players[i];

            for (std::size_t j = i + 1; j < players.size(); ++j) {
                ChessPlayer *p2 = players[j];

                game_history.push_back(p1->versus(*p2));
                game_history.push_back(p2->versus(*p1));
            }
        }
    }
}
