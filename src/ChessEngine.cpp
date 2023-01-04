#include "ChessEngine.hpp"


const PositionInfo &ChessEngineInterface::lookup(const ChessPosition &pos
) noexcept {
    if (!cache.contains(pos)) {
        std::vector<ChessMove> legal_moves;
        pos.visit_legal_moves([&](ChessMove move, const ChessPosition &) {
            legal_moves.push_back(move);
        });
        const bool in_check = pos.in_check();
        cache.insert({
            pos, {std::move(legal_moves), in_check}
        });
    }
    return cache[pos];
}


const std::vector<ChessMove> &
ChessEngineInterface::get_legal_moves(const ChessPosition &pos) noexcept {
    return lookup(pos).legal_moves;
}


const std::vector<ChessMove> &ChessEngineInterface::get_legal_moves() noexcept {
    return get_legal_moves(current_pos);
}


bool ChessEngineInterface::checkmated(const ChessPosition &pos) noexcept {
    const PositionInfo &info = lookup(pos);
    return info.in_check && info.legal_moves.empty();
}


bool ChessEngineInterface::checkmated() noexcept {
    return checkmated(current_pos);
}


bool ChessEngineInterface::stalemated(const ChessPosition &pos) noexcept {
    const PositionInfo &info = lookup(pos);
    return (!info.in_check) && info.legal_moves.empty();
}


bool ChessEngineInterface::stalemated() noexcept {
    return stalemated(current_pos);
}


void ChessEngineInterface::make_move(ChessMove move) noexcept {
    current_pos.make_move(move);
}


ChessEngine::~ChessEngine() noexcept = default;
