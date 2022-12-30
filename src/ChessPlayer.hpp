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

        // A B C D E   G H I     L M   O     R S         X   Z
        //           F       J K     N   P Q     T U V W   Y

        for (PreferenceToken token : tokens) {
            // clang-format off
            switch (token) {
                case MATE_IN_ONE:         name_builder << "M1"; preference_engine->add_preference<MateInOne>();   break;
                case PREVENT_MATE_IN_ONE: name_builder << "PM1"; preference_engine->add_preference<PreventMateInOne>();   break;
                case PREVENT_STALEMATE: name_builder << "PST"; preference_engine->add_preference<PreventStalemate>();   break;
                case CHECK:       name_builder << "Chk"; preference_engine->add_preference<Check>();       break;
                case CAPTURE:     name_builder << "Cap"; preference_engine->add_preference<Capture>();     break;
                case CAPTURE_HANGING:     name_builder << "CAP"; preference_engine->add_preference<CaptureHanging>();     break;
                case FIRST:       name_builder << "Fst"; preference_engine->add_preference<First>();     break;
                case LAST:        name_builder << "Lst"; preference_engine->add_preference<Last>();     break;
                case REDUCE:      name_builder << "Red"; preference_engine->add_preference<Reduce>();      break;
                case SWARM:       name_builder << "Swm"; preference_engine->add_preference<Swarm>();       break;
                case HUDDLE:      name_builder << "Hud"; preference_engine->add_preference<Huddle>();      break;
                case ENERGETIC:   name_builder << "Nrg"; preference_engine->add_preference<Energetic>();   break;
                case LAZY:        name_builder << "Lzy"; preference_engine->add_preference<Lazy>();        break;
                case COORDINATED: name_builder << "Cor"; preference_engine->add_preference<Coordinated>(); break;
                case BLOCKADE:    name_builder << "Blk"; preference_engine->add_preference<Blockade>();    break;
                case DEFENDER:    name_builder << "Def"; preference_engine->add_preference<Defender>();    break;
                case OUTPOST:     name_builder << "Out"; preference_engine->add_preference<Outpost>();     break;
                case GAMBIT:      name_builder << "Gam"; preference_engine->add_preference<Gambit>();      break;
                case EXPLORE:     name_builder << "Exp"; preference_engine->add_preference<Explore>();      break;

            }
            // clang-format on
        }

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
     * @param verbose_level 0: nothing, 1: print result, 2: print game board
     * every move
     * @return ChessGame
     */
    ChessGame versus(ChessPlayer &black, int verbose_level = 1);

    [[nodiscard]] std::string get_name_with_elo(int precision) const;

}; // class ChessPlayer


#endif // SUCKER_CHESS_CHESS_PLAYER_HPP
