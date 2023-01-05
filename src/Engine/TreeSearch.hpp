#ifndef SUCKER_CHESS_ENGINE_TREE_SEARCH_HPP
#define SUCKER_CHESS_ENGINE_TREE_SEARCH_HPP

#include <iostream> // TODO: REMOVE

#include <algorithm>     // for std::max, std::min
#include <limits>        // for std::numeric_limits
#include <random>        // for std::mt19937
#include <string>        // for std::string
#include <unordered_map> // for std::unordered_map
#include <utility>       // for std::pair, std::make_pair

#include "../ChessEngine.hpp"
#include "../ChessPosition.hpp"
#include "../Utilities.hpp"


namespace Engine {


class TreeSearch final : public ChessEngine {

    using T = int;
    static constexpr T ONE = static_cast<T>(1);
    static constexpr T MAX = std::numeric_limits<T>::max();
    static constexpr T MIN = std::numeric_limits<T>::min();
    static constexpr T MATE_THRESHOLD = static_cast<T>(1000);

    std::mt19937 rng;
    std::string name;
    std::unordered_map<ChessPosition, std::pair<T, int>> evaluation_cache;

public: // =====================================================================

    explicit TreeSearch() noexcept
        : rng(properly_seeded_random_engine())
        , name("TreeSearch")
        , evaluation_cache() {}

    static constexpr int unsigned_material_value(ChessPiece piece) noexcept {
        switch (piece.get_type()) {
            case PieceType::NONE: return 0;
            case PieceType::KING: return 0;
            case PieceType::QUEEN: return 900;
            case PieceType::ROOK: return 500;
            case PieceType::BISHOP: return 300;
            case PieceType::KNIGHT: return 300;
            case PieceType::PAWN: return 100;
        }
        __builtin_unreachable();
    }

    static constexpr int material_value(ChessPiece piece) noexcept {
        switch (piece.get_color()) {
            case PieceColor::NONE: return 0;
            case PieceColor::WHITE: return +unsigned_material_value(piece);
            case PieceColor::BLACK: return -unsigned_material_value(piece);
        }
        __builtin_unreachable();
    }

    static constexpr int leaf_evaluation_function(const ChessPosition &pos
    ) noexcept {
        int result = 0;
        for (coord_t file = 0; file < NUM_FILES; ++file) {
            for (coord_t rank = 0; rank < NUM_RANKS; ++rank) {
                result += material_value(pos.get_board().get_piece(file, rank));
            }
        }
        return result;
    }

    static constexpr T adjust(T value) noexcept {
        if (value >= MAX - MATE_THRESHOLD) { value -= ONE; }
        if (value <= MIN + MATE_THRESHOLD) { value += ONE; }
        return value;
    }

    T evaluate(
        ChessEngineInterface &interface,
        const ChessPosition &pos,
        int depth,
        T alpha,
        T beta
    ) noexcept {

        // Look up current position in interface cache.
        const PositionInfo &info = interface.lookup(pos);

        // If there are no legal moves, then the game is over.
        if (info.legal_moves.empty()) {
            if (info.in_check) { // checkmate
                switch (pos.get_color_to_move()) {
                    case PieceColor::NONE: __builtin_unreachable();
                    case PieceColor::WHITE: return MIN + ONE;
                    case PieceColor::BLACK: return MAX - ONE;
                }
                __builtin_unreachable();
            } else { // stalemate
                return static_cast<T>(0);
            }
        }

        // Otherwise, we need to evaluate the position.
        if (depth <= 0) { return leaf_evaluation_function(pos); }

        const auto iter = evaluation_cache.find(pos);
        if (iter != evaluation_cache.end()) {
            const auto [prev_eval, prev_depth] = iter->second;
            if (prev_depth >= depth) { return prev_eval; }
        }

        T result;
        switch (pos.get_color_to_move()) {

            case PieceColor::NONE: __builtin_unreachable();

            case PieceColor::WHITE:
                result = std::numeric_limits<T>::min();
                for (ChessMove move : info.legal_moves) {
                    ChessPosition next_pos = pos;
                    next_pos.make_move(move);
                    const T next_value = adjust(
                        evaluate(interface, next_pos, depth - 1, alpha, beta)
                    );
                    result = std::max(result, next_value);
                    if (result > beta) { break; }
                    alpha = std::max(alpha, result);
                }
                break;

            case PieceColor::BLACK:
                result = std::numeric_limits<T>::max();
                for (ChessMove move : info.legal_moves) {
                    ChessPosition next_pos = pos;
                    next_pos.make_move(move);
                    const T next_value = adjust(
                        evaluate(interface, next_pos, depth - 1, alpha, beta)
                    );
                    result = std::min(result, next_value);
                    if (result < alpha) { break; }
                    beta = std::min(beta, result);
                }
                break;
        }

        evaluation_cache.insert(
            std::make_pair(pos, std::make_pair(result, depth))
        );
        return result;
    }

    ChessMove pick_move(
        ChessEngineInterface &interface,
        [[maybe_unused]] const std::vector<ChessPosition> &pos_history,
        [[maybe_unused]] const std::vector<ChessMove> &move_history
    ) override {
        switch (interface.get_color_to_move()) {
            case PieceColor::NONE: __builtin_unreachable();
            case PieceColor::WHITE:
                return random_choice(
                    rng,
                    maximal_elements(
                        interface.get_legal_moves(),
                        [&](ChessMove move) {
                            std::cout << "Considering move: " << move;
                            ChessPosition next = interface.get_current_pos();
                            next.make_move(move);
                            const T value =
                                evaluate(interface, next, 4, MIN, MAX);
                            std::cout << " : " << value << std::endl;
                            return value;
                        }
                    )
                );
            case PieceColor::BLACK:
                return random_choice(
                    rng,
                    minimal_elements(
                        interface.get_legal_moves(),
                        [&](ChessMove move) {
                            std::cout << "Considering move: " << move;
                            ChessPosition next = interface.get_current_pos();
                            next.make_move(move);
                            const T value =
                                evaluate(interface, next, 4, MIN, MAX);
                            std::cout << " : " << value << std::endl;
                            return value;
                        }
                    )
                );
        }
        __builtin_unreachable();
    }

    const std::string &get_name() noexcept override { return name; }

}; // class TreeSearch


} // namespace Engine


#endif // SUCKER_CHESS_ENGINE_TREE_SEARCH_HPP
