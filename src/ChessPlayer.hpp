#ifndef SUCKER_CHESS_CHESS_PLAYER_HPP
#define SUCKER_CHESS_CHESS_PLAYER_HPP

#include <memory>  // for std::unique_ptr, std::make_unique
#include <sstream> // for std::ostringstream
#include <string>  // for std::string
#include <tuple>   // for std::tuple
#include <utility> // for std::move
#include <vector>  // for std::vector

#include "ChessEngine.hpp"
#include "ChessGame.hpp"

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
        std::unique_ptr<ChessEngine> &&player_engine, std::string player_name
    ) noexcept
        : name(std::move(player_name))
        , engine(std::move(player_engine))
        , elo(1000)
        , num_wins_as_white(0)
        , num_wins_as_black(0)
        , num_draws(0)
        , num_losses_as_white(0)
        , num_losses_as_black(0) {}

    explicit ChessPlayer(
        const std::vector<PreferenceToken> &tokens, std::string player_name = ""
    ) noexcept
        : name(std::move(player_name))
        , engine()
        , elo(1000)
        , num_wins_as_white(0)
        , num_wins_as_black(0)
        , num_draws(0)
        , num_losses_as_white(0)
        , num_losses_as_black(0) {

        using enum PreferenceToken;
        using namespace Preference;

        auto preference_engine = std::make_unique<Engine::Preference>();
        std::ostringstream name_builder;

#define CREATE_PREFERENCE_CASE(CLASS_NAME, TOKEN_NAME, STRING_NAME, COMMENT)   \
    case TOKEN_NAME:                                                           \
        name_builder << STRING_NAME;                                           \
        preference_engine->add_preference<CLASS_NAME>();                       \
        break;

        for (PreferenceToken token : tokens) {
            switch (token) { DECLARE_PREFERENCES(CREATE_PREFERENCE_CASE) }
        }

#undef CREATE_PREFERENCE_CASE

        if (name.empty()) { name = name_builder.str(); }
        engine = std::move(preference_engine);
    }

public: // ===========================================================

    // ACCESSORS

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

    /**
     * @brief Runs a game between two ChessPlayer objects; updates stats like
     * wins, losses, and ELO
     *
     * @param black Other ChessPlayer
     * @param elo_k_factor K-factor used in Elo rating calculation
     * @param verbose_level 0: nothing, 1: print result,
     *                      2: print board after every move
     * @return ChessGame
     */
    ChessGame
    versus(ChessPlayer &black, double elo_k_factor, int verbose_level = 1);

    [[nodiscard]] std::string get_name_with_elo(int precision) const;

}; // class ChessPlayer


#endif // SUCKER_CHESS_CHESS_PLAYER_HPP
