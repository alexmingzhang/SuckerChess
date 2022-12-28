#include "ChessGame.hpp"
#include "Utilities.hpp"

#include <algorithm> // for std::find
#include <cstddef>   // for std::size_t
#include <iostream>  // for std::cin, std::cout, std::endl
#include <sstream>   // for std::istringstream, std::ostringstream
#include <string>    // for std::getline, std::string


void ChessGame::make_move(const ChessMove &move) {
    pos_history.push_back(current_pos);
    move_history.push_back(move);
    if (current_pos.is_capture(move) ||
        current_pos[move.get_src()].get_type() == PieceType::PAWN) {
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
ChessGame::run(ChessEngine *white, ChessEngine *black, bool verbose) {
    while (true) {

        println(verbose, current_pos);

        if (current_pos.checkmated()) {
            switch (current_pos.get_color_to_move()) {
                case PieceColor::NONE: __builtin_unreachable();
                case PieceColor::WHITE:
                    println(verbose, "Black wins! Game over.");
                    current_status = GameStatus::BLACK_WON_BY_CHECKMATE;
                    return PieceColor::BLACK;
                case PieceColor::BLACK:
                    println(verbose, "White wins! Game over.");
                    current_status = GameStatus::WHITE_WON_BY_CHECKMATE;
                    return PieceColor::WHITE;
            }
        }

        if (drawn()) {
            println(verbose, "Draw! Game over.");
            return PieceColor::NONE;
        }

        if (verbose) {
            switch (current_pos.get_color_to_move()) {
                case PieceColor::NONE: __builtin_unreachable();
                case PieceColor::WHITE:
                    println(verbose, "White to move.");
                    break;
                case PieceColor::BLACK:
                    println(verbose, "Black to move.");
                    break;
            }
        }

        ChessEngine *const current_engine =
            current_pos.get_color_to_move() == PieceColor::WHITE ? white
                                                                 : black;
        const std::vector<ChessMove> legal_moves =
            current_pos.get_legal_moves();

        if (current_engine == nullptr) {
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
            const ChessMove chosen_move = current_engine->pick_move(
                current_pos, legal_moves, pos_history, move_history
            );
            assert(
                std::find(
                    legal_moves.begin(), legal_moves.end(), chosen_move
                ) != legal_moves.end()
            );
            println(
                verbose,
                "Chosen move: " +
                    current_pos.get_move_name(legal_moves, chosen_move)
            );
            make_move(chosen_move);
        }
    }
}

std::string ChessGame::get_PGN_result() const {
    std::ostringstream result;
    result << "[Result \"";
    switch (current_status) {
        case GameStatus::IN_PROGRESS: result << "*"; break;
        case GameStatus::WHITE_WON_BY_CHECKMATE: result << "1-0"; break;
        case GameStatus::BLACK_WON_BY_CHECKMATE: result << "0-1"; break;
        case GameStatus::DRAWN_BY_STALEMATE: result << "1/2-1/2"; break;
        case GameStatus::DRAWN_BY_REPETITION: result << "1/2-1/2"; break;
        case GameStatus::DRAWN_BY_50_MOVE_RULE: result << "1/2-1/2"; break;
    }
    result << "\"]\n";
    return result.str();
}

std::string ChessGame::get_PGN_move_text() const {
    std::ostringstream move_text;

    for (std::size_t i = 0; i < move_history.size(); ++i) {
        const std::string name = pos_history[i].get_move_name(
            pos_history[i].get_legal_moves(), move_history[i]
        );

        if (i % 2 == 0) {
            move_text << (i / 2 + 1) << ". " << name;
        } else {
            move_text << " " << name << " ";
        }
    }

    return move_text.str();
}


std::string ChessGame::get_PGN() const {
    std::ostringstream PGN;

    PGN << "[Site \"https://github.com/alexmingzhang/SuckerChess/\"]\n";
    PGN << "[Date \"" << get_ymd_date('.') << "\"]\n";
    PGN << get_PGN_result();
    PGN << get_PGN_move_text();

    return PGN.str();
}

std::string ChessGame::get_full_PGN(
    const std::string &event_name,
    int num_round,
    const std::string &white_name,
    const std::string &black_name
) const {
    std::ostringstream PGN;

    PGN << "[Event \"" << event_name << "\"]\n";
    PGN << "[Site \"https://github.com/alexmingzhang/SuckerChess/\"]\n";
    PGN << "[Date \"" << get_ymd_date('.') << "\"]\n";
    PGN << "[Round \"" << num_round << "\"]\n";
    PGN << "[White \"" << white_name << "\"]\n";
    PGN << "[Black \"" << black_name << "\"]\n";
    PGN << get_PGN_result() << "\n";
    PGN << get_PGN_move_text();

    return PGN.str();
}
