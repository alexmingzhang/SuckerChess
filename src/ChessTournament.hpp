#ifndef SUCKER_CHESS_CHESS_TOURNAMENT_HPP
#define SUCKER_CHESS_CHESS_TOURNAMENT_HPP

#include <memory>  // for std::unique_ptr
#include <string>  // for std::string
#include <utility> // for std::move
#include <vector>  // for std::vector

#include "ChessGame.hpp"
#include "ChessPlayer.hpp"


class ChessTournament {

    std::string name;
    std::vector<std::unique_ptr<ChessPlayer>> players;
    std::vector<ChessGame> game_history;
    long long current_round;

public: // ======================================================== CONSTRUCTORS

    explicit ChessTournament()
        : name("SuckerChess Tournament")
        , players()
        , game_history()
        , current_round(0) {}

    explicit ChessTournament(std::string n)
        : name(std::move(n))
        , players()
        , game_history()
        , current_round(0) {}

public: // =========================================================== ACCESSORS

    [[nodiscard]] constexpr const std::string &get_name() const noexcept {
        return name;
    }

    [[nodiscard]] constexpr const std::vector<std::unique_ptr<ChessPlayer>> &
    get_players() const noexcept {
        return players;
    }

    [[nodiscard]] constexpr const std::vector<ChessGame> &
    get_game_history() const noexcept {
        return game_history;
    }

public: // ============================================================ MUTATORS

    void add_player(std::unique_ptr<ChessPlayer> &&player) noexcept {
        players.push_back(std::move(player));
    }

    void sort_players_by_elo();

public: // =====================================================================

    void run(long long num_rounds, long long print_frequency);

    void print_info() const;

}; // class ChessTournament


#endif // SUCKER_CHESS_CHESS_TOURNAMENT_HPP
