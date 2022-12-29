#include "ChessGame.hpp"

#include <algorithm> // for std::find
#include <cassert>   // for assert
#include <cstddef>   // for std::size_t
#include <iostream>  // for std::cin, std::cout, std::cerr, std::endl
#include <sstream>   // for std::istringstream, std::ostringstream
#include <string>    // for std::getline, std::string

#include "Utilities.hpp"


GameStatus ChessGame::get_status() const {

    using enum GameStatus;

    // before anything else, check for threefold repetition
    int count = 0;
    for (const ChessPosition &pos : pos_history) {
        if (pos == current_pos) { ++count; }
        if (count >= 2) { return DRAWN_BY_REPETITION; }
    }

    // check for game-end conditions
    if (current_pos.checkmated()) {
        switch (current_pos.get_color_to_move()) {
            case PieceColor::NONE: __builtin_unreachable();
            case PieceColor::WHITE: return BLACK_WON_BY_CHECKMATE;
            case PieceColor::BLACK: return WHITE_WON_BY_CHECKMATE;
        }
    } else if (current_pos.stalemated()) {
        return DRAWN_BY_STALEMATE;
    } else if (current_pos.get_board().has_insufficient_material()) {
        return DRAWN_BY_INSUFFICIENT_MATERIAL;
    } else if (half_move_clock >= 100) {
        return DRAWN_BY_50_MOVE_RULE;
    }

    return IN_PROGRESS;
}


void ChessGame::make_move(const ChessMove &move) {

    if (current_status != GameStatus::IN_PROGRESS) {
        std::cerr << "WARNING: Making moves on an already-completed chess game."
                  << std::endl;
    }

    // save current state in history vectors
    pos_history.push_back(current_pos);
    move_history.push_back(move);

    // reset half-move clock on captures and pawn moves
    if (current_pos.is_capture(move) ||
        current_pos[move.get_src()].get_type() == PieceType::PAWN) {
        half_move_clock = 0;
    } else {
        ++half_move_clock;
    }

    // make move
    current_pos.make_move(move);

    // update full-move count after black's move
    if (current_pos.get_color_to_move() == PieceColor::WHITE) {
        ++full_move_count;
    }

    // update status
    current_status = get_status();
}


ChessMove ChessGame::get_console_move(const std::vector<ChessMove> &legal_moves
) {

    std::vector<std::string> base_names;
    std::vector<std::string> suffixed_names;
    for (const ChessMove &move : legal_moves) {
        base_names.push_back(current_pos.get_move_name(legal_moves, move, false)
        );
        suffixed_names.push_back(
            current_pos.get_move_name(legal_moves, move, true)
        );
    }

    while (true) {

        std::string command;
        std::cout << "> ";
        std::getline(std::cin, command);
        trim(command);

        if (command == "ls") {
            std::cout << "Legal moves:";
            for (const std::string &name : suffixed_names) {
                std::cout << ' ' << name;
            }
            std::cout << std::endl;
        } else if (command == "fen") {
            std::cout << current_pos.get_fen() << " " << half_move_clock << " "
                      << full_move_count << std::endl;
        } else if (command.starts_with("fen ")) {
            command.erase(0, 4);
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
            pos_history.clear();
            move_history.clear();
            current_status = get_status();
            return NULL_MOVE;
        } else {
            assert(base_names.size() == legal_moves.size());
            assert(suffixed_names.size() == legal_moves.size());
            for (std::size_t i = 0; i < legal_moves.size(); ++i) {
                if ((command == base_names[i]) ||
                    (command == suffixed_names[i])) {
                    return legal_moves[i];
                }
            }
            std::cout << "ERROR: " << command
                      << " is not a legal move in this position." << std::endl;
        }
    }
}


PieceColor
ChessGame::run(ChessEngine *white, ChessEngine *black, bool verbose) {

    using enum GameStatus;
    while (true) {

        print(verbose, current_pos);
        println(verbose, current_pos.get_fen());
        assert(current_pos.check_consistency());

        if (current_status != IN_PROGRESS) {
            switch (current_status) {
                case IN_PROGRESS: __builtin_unreachable();
                case WHITE_WON_BY_CHECKMATE:
                    println(verbose, "White wins! Game over.");
                    return PieceColor::WHITE;
                case BLACK_WON_BY_CHECKMATE:
                    println(verbose, "Black wins! Game over.");
                    return PieceColor::BLACK;
                case DRAWN_BY_STALEMATE:
                    println(verbose, "Draw by stalemate. Game over.");
                    return PieceColor::NONE;
                case DRAWN_BY_INSUFFICIENT_MATERIAL:
                    println(
                        verbose, "Draw by insufficient material. Game over."
                    );
                    return PieceColor::NONE;
                case DRAWN_BY_REPETITION:
                    println(
                        verbose, "Draw by threefold repetition. Game over."
                    );
                    return PieceColor::NONE;
                case DRAWN_BY_50_MOVE_RULE:
                    println(verbose, "Draw 50 move rule. Game over.");
                    return PieceColor::NONE;
            }
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
    using enum GameStatus;
    std::ostringstream result;
    result << "[Result \"";
    switch (current_status) {
        case IN_PROGRESS: result << "*"; break;
        case WHITE_WON_BY_CHECKMATE: result << "1-0"; break;
        case BLACK_WON_BY_CHECKMATE: result << "0-1"; break;
        case DRAWN_BY_STALEMATE: [[fallthrough]];
        case DRAWN_BY_INSUFFICIENT_MATERIAL: [[fallthrough]];
        case DRAWN_BY_REPETITION: [[fallthrough]];
        case DRAWN_BY_50_MOVE_RULE: result << "1/2-1/2"; break;
    }
    result << "\"]\n";
    return result.str();
}


std::string ChessGame::get_PGN_move_text() const {
    std::ostringstream move_text;
    for (std::size_t i = 0; i < move_history.size(); ++i) {
        const std::string name = pos_history[i].get_move_name(
            pos_history[i].get_legal_moves(), move_history[i], true
        );
        if (i % 2 == 0) {
            if (i > 0) { move_text << ' '; }
            move_text << (i / 2 + 1) << ". " << name;
        } else {
            move_text << ' ' << name;
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
