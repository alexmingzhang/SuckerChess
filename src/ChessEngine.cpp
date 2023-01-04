#include "ChessEngine.hpp"

#include <cassert> // for assert
#include <utility> // for std::make_pair, std::move


ChessEngineInterface::ChessEngineInterface() noexcept
    : cache()
    , current_pos()
    , current_info(&lookup(current_pos)) {}


const PositionInfo &ChessEngineInterface::lookup(const ChessPosition &pos
) noexcept {
    const auto location = cache.find(pos);
    if (location != cache.end()) {
        return location->second;
    } else {
        std::vector<ChessMove> legal_moves;
        pos.visit_legal_moves([&](ChessMove move, const ChessPosition &) {
            legal_moves.push_back(move);
        });
        const bool in_check = pos.in_check();
        const auto [iterator, inserted] = cache.insert(
            std::make_pair(pos, PositionInfo{std::move(legal_moves), in_check})
        );
        assert(inserted);
        return iterator->second;
    }
}


const std::vector<ChessMove> &
ChessEngineInterface::get_legal_moves(const ChessPosition &pos) noexcept {
    return lookup(pos).legal_moves;
}


const std::vector<ChessMove> &ChessEngineInterface::get_legal_moves() noexcept {
    return current_info->legal_moves;
}


bool ChessEngineInterface::checkmated(const ChessPosition &pos) noexcept {
    if (pos.in_check()) {
        return get_legal_moves(pos).empty();
    } else {
        return false;
    }
}


bool ChessEngineInterface::checkmated() noexcept {
    return current_info->in_check && current_info->legal_moves.empty();
}


bool ChessEngineInterface::stalemated(const ChessPosition &pos) noexcept {
    if (pos.in_check()) {
        return false;
    } else {
        return get_legal_moves(pos).empty();
    }
}


bool ChessEngineInterface::stalemated() noexcept {
    return (!current_info->in_check) && current_info->legal_moves.empty();
}


void ChessEngineInterface::make_move(ChessMove move) noexcept {
    current_pos.make_move(move);
    current_info = &lookup(current_pos);
}


ChessEngine::~ChessEngine() noexcept = default;
