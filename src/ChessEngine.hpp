#ifndef SUCKER_CHESS_CHESS_ENGINE_HPP
#define SUCKER_CHESS_CHESS_ENGINE_HPP

#include <string>        // for std::string
#include <unordered_map> // for std::unordered_map
#include <vector>        // for std::vector

#include "ChessMove.hpp"
#include "ChessPiece.hpp"
#include "ChessPosition.hpp"


struct PositionInfo {

    std::vector<ChessMove> legal_moves;
    bool in_check;

}; // class PositionInfo


class ChessEngineInterface {

    std::unordered_map<ChessPosition, PositionInfo> cache;
    ChessPosition current_pos;
    const PositionInfo *current_info;

public: // ========================================================= CONSTRUCTOR

    explicit ChessEngineInterface() noexcept;

public: // =========================================================== ACCESSORS

    [[nodiscard]] constexpr const ChessPosition &
    get_current_pos() const noexcept {
        return current_pos;
    }

    [[nodiscard]] constexpr PieceColor get_color_to_move() const noexcept {
        return current_pos.get_color_to_move();
    }

public: // ======================================================== CACHE LOOKUP

    const PositionInfo &lookup(const ChessPosition &pos) noexcept;

public: // ======================================================= STATE TESTING

    const std::vector<ChessMove> &get_legal_moves(const ChessPosition &pos
    ) noexcept;

    const std::vector<ChessMove> &get_legal_moves() noexcept;

    bool checkmated(const ChessPosition &pos) noexcept;

    bool checkmated() noexcept;

    bool stalemated(const ChessPosition &pos) noexcept;

    bool stalemated() noexcept;

public: // ====================================================== MOVE EXECUTION

    void make_move(ChessMove move) noexcept;

}; // class ChessEngineInterface


class ChessEngine {

public:

    virtual ~ChessEngine() noexcept = 0;

    virtual ChessMove pick_move(
        ChessEngineInterface &interface,
        const std::vector<ChessPosition> &pos_history,
        const std::vector<ChessMove> &move_history
    ) = 0;

    virtual const std::string &get_name() noexcept = 0;

}; // class ChessEngine


#endif // SUCKER_CHESS_CHESS_ENGINE_HPP
