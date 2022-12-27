#ifndef SUCKER_CHESS_CHESS_TOURNAMENT_HPP
#define SUCKER_CHESS_CHESS_TOURNAMENT_HPP

#include <string> // for std::string
#include <vector> // for std::vector

#include "ChessEngine.hpp"
#include "ChessGame.hpp"

constexpr double KFACTOR = 32; // for ELO calculation

class ChessPlayer {

public:

    std::string name;
    ChessEngine *engine;

private:

    double elo;
    unsigned int num_wins;
    unsigned int num_draws;
    unsigned int num_losses;

public:


    explicit ChessPlayer(const std::string &n, ChessEngine *e) noexcept
        : name(n)
        , engine(e)
        , elo(1000)
        , num_wins(0)
        , num_draws(0)
        , num_losses(0) {}

    const std::string &get_name() const;
    double get_elo() const;
    unsigned int get_num_wins() const;
    unsigned int get_num_draws() const;
    unsigned int get_num_losses() const;

    ChessGame versus(ChessPlayer &);
};

class ChessTournament {

    std::string name;
    std::vector<ChessPlayer *> players;
    std::vector<ChessGame> game_history;


public:

    explicit ChessTournament(const std::string &n)
        : name(n)
        , players()
        , game_history() {}

    void add_player(ChessPlayer *);

    const std::vector<ChessPlayer *> &get_players() const;

    const std::vector<ChessGame> &get_game_history() const;

    void run(unsigned int);
};

#endif // SUCKER_CHESS_CHESS_TOURNAMENT_HPP