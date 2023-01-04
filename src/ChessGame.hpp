#ifndef SUCKER_CHESS_CHESS_GAME_HPP
#define SUCKER_CHESS_CHESS_GAME_HPP

#include <cassert>  // for assert
#include <cstdint>  // for std::uint8_t
#include <iostream> // for std::cout, std::endl
#include <vector>   // for std::vector

#include "ChessEngine.hpp"
#include "ChessMove.hpp"
#include "ChessPiece.hpp"
#include "ChessPosition.hpp"


enum class GameStatus : std::uint8_t {
    IN_PROGRESS,
    WHITE_WON_BY_CHECKMATE,
    BLACK_WON_BY_CHECKMATE,
    DRAWN_BY_STALEMATE,
    DRAWN_BY_INSUFFICIENT_MATERIAL,
    DRAWN_BY_REPETITION,
    DRAWN_BY_50_MOVE_RULE,
}; // enum class GameStatus


class ChessGame final {

    ChessEngineInterface m_interface;
    GameStatus m_status;
    std::vector<ChessPosition> m_pos_history;
    std::vector<ChessMove> m_move_history;
    int m_half_move_clock;
    int m_full_move_count;

public: // ========================================================= CONSTRUCTOR

    explicit ChessGame() noexcept
        : m_interface()
        , m_status(GameStatus::IN_PROGRESS)
        , m_pos_history()
        , m_move_history()
        , m_half_move_clock(0)
        , m_full_move_count(1) {}

public: // =========================================================== ACCESSORS

    [[nodiscard]] constexpr GameStatus get_current_status() const noexcept {
        return m_status;
    }

    [[nodiscard]] constexpr int get_half_move_clock() const noexcept {
        return m_half_move_clock;
    }

    [[nodiscard]] constexpr int get_full_move_count() const noexcept {
        return m_full_move_count;
    }

private: // ====================================================================

    GameStatus compute_current_status() noexcept {

        using enum PieceColor;
        using enum GameStatus;

        // before anything else, check for threefold repetition
        int count = 0;
        const ChessPosition &cur = m_interface.get_current_pos();
        for (const ChessPosition &pos : m_pos_history) {
            if (cur == pos) {
                ++count;
                if (count >= 2) { return DRAWN_BY_REPETITION; }
            }
        }

        // check for game-end conditions
        if (m_interface.checkmated()) {
            switch (m_interface.get_color_to_move()) {
                case NONE: __builtin_unreachable();
                case WHITE: return BLACK_WON_BY_CHECKMATE;
                case BLACK: return WHITE_WON_BY_CHECKMATE;
            }
            __builtin_unreachable();
        } else if (m_interface.stalemated()) {
            return DRAWN_BY_STALEMATE;
        } else if (m_interface.get_current_pos()
                       .get_board()
                       .has_insufficient_material()) {
            return DRAWN_BY_INSUFFICIENT_MATERIAL;
        } else if (get_half_move_clock() >= 100) {
            return DRAWN_BY_50_MOVE_RULE;
        }

        return IN_PROGRESS;
    }

public: // =====================================================================

    void make_move(ChessMove move) noexcept {

        assert(get_current_status() == GameStatus::IN_PROGRESS);

        // save current state in history vectors
        m_pos_history.push_back(m_interface.get_current_pos());
        m_move_history.push_back(move);

        // reset half-move clock on captures and pawn moves
        if (m_interface.get_current_pos().is_capture_or_pawn_move(move)) {
            m_half_move_clock = 0;
        } else {
            ++m_half_move_clock;
        }

        // make move
        m_interface.make_move(move);

        // update full-move count after black's move
        if (m_interface.get_color_to_move() == PieceColor::WHITE) {
            ++m_full_move_count;
        }

        // update status
        m_status = compute_current_status();
    }

private: // ====================================================================

    static void println(bool verbose, const char *str) noexcept {
        if (verbose) { std::cout << str << std::endl; }
    }

    template <typename T>
    static constexpr bool
    contains(const std::vector<T> &v, const T &x) noexcept {
        for (const T &y : v) {
            if (x == y) { return true; }
        }
        return false;
    }

public: // =====================================================================

    PieceColor
    run(ChessEngine *white, ChessEngine *black, bool verbose = true) {

        using enum GameStatus;

        while (get_current_status() == IN_PROGRESS) {

            if (verbose) {
                std::cout << m_interface.get_current_pos()
                          << m_interface.get_current_pos().get_fen()
                          << std::endl;
            }

            assert(m_interface.get_current_pos().check_consistency());

            if (verbose) {
                switch (m_interface.get_color_to_move()) {
                    case PieceColor::NONE: __builtin_unreachable();
                    case PieceColor::WHITE:
                        println(verbose, "White to move.");
                        break;
                    case PieceColor::BLACK:
                        println(verbose, "Black to move.");
                        break;
                }
            }

            ChessEngine *const player =
                (m_interface.get_color_to_move() == PieceColor::BLACK) ? black
                                                                       : white;
            if (player == nullptr) {
                // TODO: re-implement command-line player
                assert(false);
            } else {
                const ChessMove move = player->pick_move(
                    m_interface, m_pos_history, m_move_history
                );
                if (verbose) {
                    std::cout << "Chosen move: "
                              << m_interface.get_current_pos().get_move_name(
                                     m_interface.get_legal_moves(), move, true
                                 )
                              << std::endl;
                }
                assert(m_interface.get_current_pos().is_valid(move));
                assert(contains(m_interface.get_legal_moves(), move));
                make_move(move);
            }
        }

        if (verbose) {
            std::cout << m_interface.get_current_pos()
                      << m_interface.get_current_pos().get_fen()
                      << std::endl;
        }

        switch (get_current_status()) {
            case IN_PROGRESS: __builtin_unreachable();
            case WHITE_WON_BY_CHECKMATE:
                println(verbose, "White wins by checkmate! Game over.");
                return PieceColor::WHITE;
            case BLACK_WON_BY_CHECKMATE:
                println(verbose, "Black wins by checkmate! Game over.");
                return PieceColor::BLACK;
            case DRAWN_BY_STALEMATE:
                println(verbose, "Draw by stalemate. Game over.");
                return PieceColor::NONE;
            case DRAWN_BY_INSUFFICIENT_MATERIAL:
                println(verbose, "Draw by insufficient material. Game over.");
                return PieceColor::NONE;
            case DRAWN_BY_REPETITION:
                println(verbose, "Draw by threefold repetition. Game over.");
                return PieceColor::NONE;
            case DRAWN_BY_50_MOVE_RULE:
                println(verbose, "Draw by 50 move rule. Game over.");
                return PieceColor::NONE;
        }
        __builtin_unreachable();
    }

}; // class ChessGame


#endif // SUCKER_CHESS_CHESS_GAME_HPP
