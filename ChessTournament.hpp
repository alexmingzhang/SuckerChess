#ifndef SUCKER_CHESS_CHESS_TOURNAMENT_HPP
#define SUCKER_CHESS_CHESS_TOURNAMENT_HPP

#include <string> // for std::string
#include <vector> // for std::vector

#include "ChessEngine.hpp"
#include "ChessGame.hpp"
#include "ChessPlayer.hpp"


class ChessTournament {

    std::string event_name;
    std::vector<ChessPlayer *> players;
    std::vector<ChessGame> game_history;
    std::size_t current_game_index;


public:

    explicit ChessTournament(const std::string &n)
        : event_name(n)
        , players()
        , game_history()
        , current_game_index(0) {}

    void add_player(ChessPlayer *);

    void sort_players_by_elo();

    const std::vector<ChessPlayer *> &get_players() const;

    const std::vector<ChessGame> &get_game_history() const;

    void run(unsigned int num_rounds, bool verbose);

}; // class ChessTournament


#endif // SUCKER_CHESS_CHESS_TOURNAMENT_HPP
