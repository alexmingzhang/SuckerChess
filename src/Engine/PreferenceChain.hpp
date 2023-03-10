#ifndef SUCKER_CHESS_ENGINE_PREFERENCE_CHAIN_HPP
#define SUCKER_CHESS_ENGINE_PREFERENCE_CHAIN_HPP

#include <memory> // for std::unique_ptr
#include <random> // for std::mt19937
#include <string> // for std::string
#include <vector> // for std::vector

#include "../ChessEngine.hpp"
#include "../ChessMove.hpp"
#include "../ChessPosition.hpp"


class ChessPreference {

public:

    virtual ~ChessPreference() noexcept = 0;

    virtual std::vector<ChessMove> pick_preferred_moves(
        ChessEngineInterface &interface,
        const std::vector<ChessMove> &allowed_moves
    ) = 0;

}; // class ChessPreference


// clang-format off
#define DECLARE_PREFERENCES(MACRO) \
    MACRO(MateInOne,        MATE_IN_ONE,         "Ma1", "always give checkmate if possible") \
    MACRO(PreventMateInOne, PREVENT_MATE_IN_ONE, "PM1", "avoid moving to positions that give opponent checkmate") \
    MACRO(PreventDraw,      PREVENT_DRAW,        "PDr", "avoid stalemate and threefold repetition") \
    MACRO(Check,            CHECK,               "Chk", "always check opponent if possible") \
    MACRO(Capture,          CAPTURE,             "Cap", "always capture any piece if possible") \
    MACRO(CaptureHanging,   CAPTURE_HANGING,     "CHa", "always capture hanging pieces if possible") \
    MACRO(SmartCapture,     SMART_CAPTURE,       "SCp", "always capture pieces that have less defenders than attackers") \
    MACRO(Castle,           CASTLE,              "Cst", "always castle if possible and avoid giving up castling rights") \
    MACRO(First,            FIRST,               "Fst", "always make first available move") \
    MACRO(Last,             LAST,                "Lst", "always make last available move") \
    MACRO(Extend,           EXTEND,              "Ext", "make moves that maximize opponent's number of legal responses") \
    MACRO(Reduce,           REDUCE,              "Red", "make moves that minimize opponent's number of legal responses") \
    MACRO(Greedy,           GREEDY,              "Grd", "make moves that maximize own material advantage") \
    MACRO(Generous,         GENEROUS,            "Gen", "make moves that minimize own material advantage") \
    MACRO(Swarm,            SWARM,               "Swm", "move pieces toward enemy king") \
    MACRO(Huddle,           HUDDLE,              "Hud", "move pieces toward own king") \
    MACRO(Sniper,           SNIPER,              "Sni", "make moves that maximize distance travelled") \
    MACRO(Sloth,            SLOTH,               "Slo", "make moves that minimize distance travelled") \
    MACRO(Conqueror,        CONQUEROR,           "Cnq", "make moves that maximize number of squares own pieces can see") \
    MACRO(Constrictor,      CONSTRICTOR,         "Cns", "make moves that minimize number of squares enemy pieces can see") \
    MACRO(Reinforced,       REINFORCED,          "Rei", "always move to squares that are attacked by self if possible") \
    MACRO(Outpost,          OUTPOST,             "Out", "always move to squares that are not attacked by enemy if possible") \
    MACRO(Gambit,           GAMBIT,              "Gam", "always move to squares that are attacked by both self and enemy if possible") \
    MACRO(Explore,          EXPLORE,             "Exp", "always move to squares that are attacked by neither self nor enemy if possible") \
    MACRO(Coward,           COWARD,              "Cow", "move pieces that are attacked") \
    MACRO(Hero,             HERO,                "Hro", "move pieces that are not attacked")
// clang-format on


enum class PreferenceToken {

#define CREATE_PREFERENCE_TOKEN(CLASS_NAME, TOKEN_NAME, STRING_NAME, COMMENT)  \
    TOKEN_NAME,

    DECLARE_PREFERENCES(CREATE_PREFERENCE_TOKEN)

#undef CREATE_PREFERENCE_TOKEN

}; // enum class PreferenceToken


namespace Preference {

#define CREATE_PREFERENCE_CLASS(CLASS_NAME, TOKEN_NAME, STRING_NAME, COMMENT)  \
    class CLASS_NAME final : public ChessPreference {                          \
        std::vector<ChessMove> pick_preferred_moves(                           \
            ChessEngineInterface &interface,                                   \
            const std::vector<ChessMove> &allowed_moves                        \
        ) override;                                                            \
    };

DECLARE_PREFERENCES(CREATE_PREFERENCE_CLASS)

#undef CREATE_PREFERENCE_CLASS

} // namespace Preference


namespace Engine {

class PreferenceChain final : public ChessEngine {

    std::mt19937 rng;
    std::vector<std::unique_ptr<ChessPreference>> preferences;
    std::string name;

public:

    explicit PreferenceChain(const std::vector<PreferenceToken> &tokens);

    ChessMove pick_move(
        ChessEngineInterface &interface,
        const std::vector<ChessPosition> &pos_history,
        const std::vector<ChessMove> &move_history
    ) override;

    const std::string &get_name() noexcept override;

}; // class PreferenceChain

} // namespace Engine


#endif // SUCKER_CHESS_ENGINE_PREFERENCE_CHAIN_HPP
