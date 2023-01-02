#ifndef SUCKER_CHESS_CHESS_TOURNAMENT_HPP
#define SUCKER_CHESS_CHESS_TOURNAMENT_HPP

#include <memory>  // for std::unique_ptr
#include <string>  // for std::string
#include <utility> // for std::move
#include <vector>  // for std::vector

#include "ChessGame.hpp"
#include "ChessPlayer.hpp"
#include "Utilities.hpp"

class ChessTournament final {

    std::mt19937 rng;
    std::string name;
    std::vector<std::unique_ptr<ChessPlayer>> players;
    std::vector<ChessGame> game_history;
    std::size_t current_round;
    std::size_t num_matchups_per_round;
    double elo_k_factor;

public: // ======================================================== CONSTRUCTORS

    explicit ChessTournament()
        : rng(properly_seeded_random_engine())
        , name("SuckerChess Tournament")
        , players()
        , game_history()
        , current_round(0)
        , num_matchups_per_round(0)
        , elo_k_factor(40.0) {}

    explicit ChessTournament(std::string n)
        : rng(properly_seeded_random_engine())
        , name(std::move(n))
        , players()
        , game_history()
        , current_round(0)
        , num_matchups_per_round(0)
        , elo_k_factor(40.0) {}

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

    /**
     * @brief Add player to tournament
     *
     * @param player unique_ptr of ChessPlayer
     */
    void add_player(std::unique_ptr<ChessPlayer> &&player) noexcept;

    void sort_players_by_elo();

public: // =====================================================================

    /**
     * @brief Run a randomized tournament where each round every player meets
     * every other player once as white and once as black.
     *
     * @param num_rounds Number of rounds
     * @param print_frequency Print info about tournament every print_frequency
     * rounds (-1 disables printing, 0 prints each round and gives info about
     * each matchup)
     * @param store_games Store games in game_history
     */
    void
    run(long long num_rounds,
        long long print_frequency = 1,
        bool store_games = false);

    /// @brief Print info about a tournament and its players
    void print_info() const;

}; // class ChessTournament


#endif // SUCKER_CHESS_CHESS_TOURNAMENT_HPP
