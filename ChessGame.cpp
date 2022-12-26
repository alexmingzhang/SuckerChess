#include "ChessGame.hpp"

#include <algorithm> // for std::find, std::find_if
#include <cctype>    // for std::isspace
#include <iostream>  // for std::cin, std::cout
#include <string>    // for std::getline, std::string


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


template <typename T> void print(bool verbose, const T &obj) {
    if (verbose) { std::cout << obj << std::endl; }
}


void trim(std::string &str) {
    const auto first_char = std::find_if(str.begin(), str.end(), [](char ch) {
        return !std::isspace(ch);
    });
    str.erase(str.begin(), first_char);
    const auto last_char = std::find_if(str.rbegin(), str.rend(), [](char ch) {
        return !std::isspace(ch);
    });
    str.erase(last_char.base(), str.end());
}


ChessMove ChessGame::get_console_move(const std::vector<ChessMove> &legal_moves
) {

    std::vector<std::string> names;
    for (const ChessMove &move : legal_moves) {
        names.push_back(current_pos.get_move_name(legal_moves, move));
    }

    while (true) {

        std::string command;
        std::cout << "> ";
        std::getline(std::cin, command);
        trim(command);

        if (command == "ls") {
            std::cout << "Legal moves:";
            for (const std::string &name : names) { std::cout << ' ' << name; }
            std::cout << std::endl;
        } else if (command == "fen") {
            std::cout << current_pos.get_fen() << " " << half_move_clock << " "
                      << full_move_count << std::endl;
        } else if (command.starts_with("fen")) {
            command.erase(0, 3);
            trim(command);
            current_pos.load_fen(command);
            std::istringstream fen(command);
            std::string dummy;
            fen >> dummy;
            fen >> dummy;
            fen >> dummy;
            fen >> dummy;
            fen >> half_move_clock;
            fen >> full_move_count;
            return NULL_MOVE;
        } else {
            for (std::size_t i = 0; i < legal_moves.size(); ++i) {
                if (command == names[i]) { return legal_moves[i]; }
            }
            std::cout << "ERROR: " << command
                      << " is not a legal move in this position." << std::endl;
        }
    }
}


PieceColor
ChessGame::run(ChessPlayer *white, ChessPlayer *black, bool verbose) {
    while (true) {

        print(verbose, current_pos);

        if (current_pos.checkmated()) {
            switch (current_pos.get_color_to_move()) {
                case PieceColor::NONE: __builtin_unreachable();
                case PieceColor::WHITE:
                    print(verbose, "Black wins! Game over.");
                    return PieceColor::BLACK;
                case PieceColor::BLACK:
                    print(verbose, "White wins! Game over.");
                    return PieceColor::WHITE;
            }
        }

        if (drawn()) {
            print(verbose, "Draw! Game over.");
            return PieceColor::NONE;
        }

        if (verbose) {
            switch (current_pos.get_color_to_move()) {
                case PieceColor::NONE: __builtin_unreachable();
                case PieceColor::WHITE: print(verbose, "White to move."); break;
                case PieceColor::BLACK: print(verbose, "Black to move."); break;
            }
        }

        ChessPlayer *const current_player =
            current_pos.get_color_to_move() == PieceColor::WHITE ? white
                                                                 : black;
        const std::vector<ChessMove> legal_moves =
            current_pos.get_legal_moves();

        if (current_player == nullptr) {
            const ChessMove chosen_move = get_console_move(legal_moves);
            if (chosen_move != NULL_MOVE) {
                assert(
                    std::find(
                        legal_moves.begin(), legal_moves.end(), chosen_move
                    ) != legal_moves.end()
                );
                make_move(chosen_move);
            }
        } else {
            const ChessMove chosen_move = current_player->pick_move(
                current_pos, legal_moves, pos_history, move_history
            );
            assert(
                std::find(
                    legal_moves.begin(), legal_moves.end(), chosen_move
                ) != legal_moves.end()
            );
            print(
                verbose, "Chosen move: " +
                             current_pos.get_move_name(legal_moves, chosen_move)
            );
            make_move(chosen_move);
        }
    }
}
