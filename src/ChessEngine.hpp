#ifndef SUCKER_CHESS_CHESS_ENGINE_HPP
#define SUCKER_CHESS_CHESS_ENGINE_HPP

#include <memory>  // for std::unique_ptr, std::make_unique
#include <random>  // for std::mt19937
#include <utility> // for std::forward
#include <vector>  // for std::vector

#include "ChessMove.hpp"
#include "ChessPosition.hpp"


class ChessPreference {

public:

    virtual ~ChessPreference() noexcept = default;

    virtual std::vector<ChessMove> pick_preferred_moves(
        const ChessPosition &current_pos,
        const std::vector<ChessMove> &allowed_moves,
        const std::vector<ChessPosition> &pos_history,
        const std::vector<ChessMove> &move_history
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
    MACRO(Hero,             HERO,                "Hro", "move pieces that are unattacked")

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
            const ChessPosition &current_pos,                                  \
            const std::vector<ChessMove> &allowed_moves,                       \
            const std::vector<ChessPosition> &pos_history,                     \
            const std::vector<ChessMove> &move_history                         \
        ) override;                                                            \
    };

DECLARE_PREFERENCES(CREATE_PREFERENCE_CLASS)

#undef CREATE_PREFERENCE_CLASS

} // namespace Preference


class ChessEngine {

public:

    virtual ~ChessEngine() noexcept = default;

    virtual ChessMove pick_move(
        const ChessPosition &current_pos,
        const std::vector<ChessMove> &legal_moves,
        const std::vector<ChessPosition> &pos_history,
        const std::vector<ChessMove> &move_history
    ) = 0;

}; // class ChessEngine


namespace Engine {


class Preference final : public ChessEngine {

    std::mt19937 rng;
    std::vector<std::unique_ptr<ChessPreference>> preferences;

public:

    Preference();

    void add_preference(std::unique_ptr<ChessPreference> &&pref);

    template <typename T, typename... Args>
    void add_preference(Args &&...args) {
        add_preference(std::make_unique<T>(std::forward<Args>(args)...));
    }

    ChessMove pick_move(
        const ChessPosition &current_pos,
        const std::vector<ChessMove> &legal_moves,
        const std::vector<ChessPosition> &pos_history,
        const std::vector<ChessMove> &move_history
    ) override;

}; // class Preference


class CCCP final : public ChessEngine {

    std::mt19937 rng;

public:

    CCCP();

    ChessMove pick_move(
        const ChessPosition &current_pos,
        const std::vector<ChessMove> &legal_moves,
        const std::vector<ChessPosition> &pos_history,
        const std::vector<ChessMove> &move_history
    ) override;

    [[nodiscard]] static constexpr int unsigned_material_value(ChessPiece piece
    ) noexcept {
        switch (piece.get_type()) {
            case PieceType::NONE: return 0;
            case PieceType::KING: return 0;
            case PieceType::QUEEN: return 9;
            case PieceType::ROOK: return 5;
            case PieceType::BISHOP: return 3;
            case PieceType::KNIGHT: return 3;
            case PieceType::PAWN: return 1;
        }
        __builtin_unreachable();
    }

    [[nodiscard]] static constexpr int material_value(ChessPiece piece
    ) noexcept {
        switch (piece.get_color()) {
            case PieceColor::NONE: return 0;
            case PieceColor::WHITE: return +unsigned_material_value(piece);
            case PieceColor::BLACK: return -unsigned_material_value(piece);
        }
        __builtin_unreachable();
    }

    [[nodiscard]] static constexpr int
    material_advantage(const ChessPosition &pos) noexcept {
        int result = 0;
        for (coord_t file = 0; file < NUM_FILES; ++file) {
            for (coord_t rank = 0; rank < NUM_RANKS; ++rank) {
                result += material_value(pos.get_board().get_piece(file, rank));
            }
        }
        return result;
    }

}; // class CCCP


} // namespace Engine


#endif // SUCKER_CHESS_CHESS_ENGINE_HPP
