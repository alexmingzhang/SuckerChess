#ifndef SUCKER_CHESS_CHESS_TOURNAMENT_HPP
#define SUCKER_CHESS_CHESS_TOURNAMENT_HPP

#include <memory>  // for std::unique_ptr
#include <random>  // for std::mt19937
#include <string>  // for std::string
#include <utility> // for std::move, std::pair
#include <vector>  // for std::vector

#include "ChessEngine.hpp"
#include "ChessGame.hpp"
#include "Utilities.hpp"


struct PerformanceInfo {

    std::size_t num_wins_as_white;
    std::size_t num_wins_as_black;
    std::size_t num_draws;
    std::size_t num_losses_as_white;
    std::size_t num_losses_as_black;

    [[nodiscard]] constexpr std::size_t total_wins() const noexcept {
        return num_wins_as_white + num_wins_as_black;
    }

    [[nodiscard]] constexpr std::size_t total_losses() const noexcept {
        return num_losses_as_white + num_losses_as_black;
    }

}; // struct PerformanceInfo


class ChessTournament final {

    std::mt19937 rng;
    std::string name;
    std::vector<std::pair<std::unique_ptr<ChessEngine>, PerformanceInfo>>
        engines;
    int name_width;
    long long current_round;

public: // ======================================================== CONSTRUCTORS

    explicit ChessTournament(std::string n = "SuckerChess Tournament")
        : rng(properly_seeded_random_engine())
        , name(std::move(n))
        , engines()
        , name_width(6)
        , current_round(0) {}

public: // =========================================================== ACCESSORS

    [[nodiscard]] constexpr const std::string &get_name() const noexcept {
        return name;
    }

public: // ============================================================ MUTATORS

    void add_engine(std::unique_ptr<ChessEngine> &&) noexcept;

    void sort_players_by_win_ratio();

public: // =========================================================== EXECUTION

    /**
     * @brief Run a randomized tournament where each round every engine meets
     * every other engine once as white and once as black.
     *
     * @param num_rounds Number of rounds (-1 for infinite rounds)
     * @param print_frequency Print info about tournament every print_frequency
     * rounds (-1 disables printing, 0 prints each round and gives info about
     * each matchup)
     */
    void run(long long num_rounds, long long print_frequency = 1);

    /// @brief Print info about a tournament and its players
    void print_info() const;

}; // class ChessTournament


#endif // SUCKER_CHESS_CHESS_TOURNAMENT_HPP
