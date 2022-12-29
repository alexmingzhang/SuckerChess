#ifndef SUCKER_CHESS_CHESS_PLAYER_HPP
#define SUCKER_CHESS_CHESS_PLAYER_HPP

#include <memory>  // for std::unique_ptr
#include <string>  // for std::string
#include <tuple>   // for std::tuple
#include <utility> // for std::move

#include "ChessEngine.hpp"
#include "ChessGame.hpp"


constexpr double ELO_K_FACTOR = 32.0;


class ChessPlayer final {

    std::string name;
    std::unique_ptr<ChessEngine> engine;
    double elo;
    unsigned long long num_wins_as_white;
    unsigned long long num_wins_as_black;
    unsigned long long num_draws;
    unsigned long long num_losses_as_white;
    unsigned long long num_losses_as_black;

public: // ========================================================= CONSTRUCTOR

    explicit ChessPlayer(
        std::string n, std::unique_ptr<ChessEngine> &&e
    ) noexcept
        : name(std::move(n))
        , engine(std::move(e))
        , elo(1000)
        , num_wins_as_white(0)
        , num_wins_as_black(0)
        , num_draws(0)
        , num_losses_as_white(0)
        , num_losses_as_black(0) {}

public: // =========================================================== ACCESSORS

    [[nodiscard]] constexpr const std::string &get_name() const noexcept {
        return name;
    }

    [[nodiscard]] constexpr double get_elo() const noexcept { return elo; }

    [[nodiscard]] constexpr unsigned long long
    get_num_wins_as_white() const noexcept {
        return num_wins_as_white;
    }

    [[nodiscard]] constexpr unsigned long long
    get_num_wins_as_black() const noexcept {
        return num_wins_as_black;
    }

    [[nodiscard]] constexpr unsigned long long get_num_draws() const noexcept {
        return num_draws;
    }

    [[nodiscard]] constexpr unsigned long long
    get_num_losses_as_white() const noexcept {
        return num_losses_as_white;
    }

    [[nodiscard]] constexpr unsigned long long
    get_num_losses_as_black() const noexcept {
        return num_losses_as_black;
    }

public: // =====================================================================

    ChessGame versus(ChessPlayer &black, bool verbose);

    [[nodiscard]] std::string get_name_with_elo(int precision) const;

}; // class ChessPlayer


#endif // SUCKER_CHESS_CHESS_PLAYER_HPP
