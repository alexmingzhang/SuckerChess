#include "ChessGame.hpp"

#include <iostream> // for std::cout


void ChessGame::make_move(const ChessMove &move) {
    pos_history.push_back(current_pos);
    move_history.push_back(move);
    if (current_pos.is_legal_cap(move.dst_file, move.dst_rank) ||
        current_pos(move.src_file, move.src_rank).type == PieceType::PAWN) {
        half_move_clock = 0;
    } else {
        ++half_move_clock;
    }
    current_pos.make_move(move);
    if (current_pos.get_color_to_move() == PieceColor::WHITE) {
        ++full_move_count;
    }
}


bool ChessGame::drawn() const {
    if (current_pos.stalemated()) { return true; }
    if (half_move_clock >= 100) { return true; }
    int count = 0;
    for (const ChessPosition &pos : pos_history) {
        if (pos == current_pos) { ++count; }
        if (count >= 2) { return true; }
    }
    return false;
}


PieceColor
ChessGame::run(ChessPlayer &white, ChessPlayer &black, bool verbose) {
    while (true) {
        if (verbose) {
            std::cout << current_pos << std::endl;
            switch (current_pos.get_color_to_move()) {
                case PieceColor::NONE: __builtin_unreachable();
                case PieceColor::WHITE:
                    std::cout << "White to move." << std::endl;
                    break;
                case PieceColor::BLACK:
                    std::cout << "Black to move." << std::endl;
                    break;
            }
        }
        if (current_pos.checkmated()) {
            switch (current_pos.get_color_to_move()) {
                case PieceColor::NONE: __builtin_unreachable();
                case PieceColor::WHITE: return PieceColor::BLACK;
                case PieceColor::BLACK: return PieceColor::WHITE;
            }
        }
        if (drawn()) { return PieceColor::NONE; }
        ChessPlayer &current_player =
            current_pos.get_color_to_move() == PieceColor::WHITE ? white
                                                                 : black;
        make_move(current_player.pick_move(
            current_pos, current_pos.get_legal_moves(), pos_history,
            move_history
        ));
    }
}
