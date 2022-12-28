#ifndef SUCKER_CHESS_CHESS_TOURNAMENT_HPP
#define SUCKER_CHESS_CHESS_TOURNAMENT_HPP

#include <string> // for std::string
#include <vector> // for std::vector

#include "ChessEngine.hpp"
#include "ChessGame.hpp"
#include "ChessPlayer.hpp"


class ChessTournament {

    std::string name;
    std::vector<ChessPlayer *> players;
    std::vector<ChessGame> game_history;
    std::size_t current_round;
    std::size_t current_game_index;

public:

    explicit ChessTournament()
        : name("SuckerChess Tournament")
        , players()
        , game_history()
        , current_round(0)
        , current_game_index(0) {}

    explicit ChessTournament(std::string n)
        : name(std::move(n))
        , players()
        , game_history()
        , current_round(0)
        , current_game_index(0) {}

    [[nodiscard]] const std::string &get_name() const;

    [[nodiscard]] const std::vector<ChessPlayer *> &get_players() const;

    [[nodiscard]] const std::vector<ChessGame> &get_game_history() const;

    void add_player(ChessPlayer *);

    void sort_players_by_elo();

    void run(int num_rounds, bool verbose);

    void print_info() const;

}; // class ChessTournament


#endif // SUCKER_CHESS_CHESS_TOURNAMENT_HPP
