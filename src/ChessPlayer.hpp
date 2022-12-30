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
                case MATE_IN_ONE: name_builder << 'M'; preference_engine->add_preference<MateInOne>();   break;
                case CHECK:       name_builder << 'C'; preference_engine->add_preference<Check>();       break;
                case CAPTURE:     name_builder << 'X'; preference_engine->add_preference<Capture>();     break;
                case FIRST:       name_builder << 'A'; preference_engine->add_preference<First>();     break;
                case LAST:        name_builder << 'Z'; preference_engine->add_preference<Last>();     break;
                case REDUCE:      name_builder << 'R'; preference_engine->add_preference<Reduce>();      break;
                case SWARM:       name_builder << 'S'; preference_engine->add_preference<Swarm>();       break;
                case HUDDLE:      name_builder << 'H'; preference_engine->add_preference<Huddle>();      break;
                case ENERGETIC:   name_builder << 'E'; preference_engine->add_preference<Energetic>();   break;
                case LAZY:        name_builder << 'L'; preference_engine->add_preference<Lazy>();        break;
                case COORDINATED: name_builder << 'I'; preference_engine->add_preference<Coordinated>(); break;
                case BLOCKADE:    name_builder << 'B'; preference_engine->add_preference<Blockade>();    break;
                case DEFENDER:    name_builder << 'D'; preference_engine->add_preference<Defender>();    break;
                case OUTPOST:     name_builder << 'O'; preference_engine->add_preference<Outpost>();     break;
                case GAMBIT:      name_builder << 'G'; preference_engine->add_preference<Gambit>();      break;
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

    ChessGame versus(ChessPlayer &black, bool verbose);

    [[nodiscard]] std::string get_name_with_elo(int precision) const;

}; // class ChessPlayer


#endif // SUCKER_CHESS_CHESS_PLAYER_HPP
