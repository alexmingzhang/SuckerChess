#include "ChessGame.hpp"

#include <cassert>  // for assert
#include <cstddef>  // for std::size_t
#include <iostream> // for std::cin, std::cout, std::cerr, std::endl
#include <sstream>  // for std::istringstream, std::ostringstream
#include <string>   // for std::getline, std::string

#include "Utilities.hpp"


GameStatus ChessGame::get_status() const {

    using enum PieceColor;
    using enum GameStatus;

    // before anything else, check for threefold repetition
    int count = 0;
    for (const ChessPosition &pos : pos_history) {
        if (current_pos == pos) { ++count; }
        if (count >= 2) { return DRAWN_BY_REPETITION; }
    }

    // check for game-end conditions
    if (current_pos.checkmated()) {
        switch (current_pos.get_color_to_move()) {
            case NONE: __builtin_unreachable();
            case WHITE: return BLACK_WON_BY_CHECKMATE;
            case BLACK: return WHITE_WON_BY_CHECKMATE;
        }
    } else if (current_pos.stalemated()) {
        return DRAWN_BY_STALEMATE;
    } else if (current_pos.has_insufficient_material()) {
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
    pos_history.push_back(current_pos.get());
    move_history.push_back(move);

    // reset half-move clock on captures and pawn moves
    if (current_pos.is_capture_or_pawn_move(move)) {
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


ChessMove ChessGame::get_console_move() {

    // retrieve legal moves and names, with and without +/# suffixes
    const std::vector<ChessMove> &legal_moves = current_pos.get_legal_moves();
    std::vector<std::string> base_names;
    std::vector<std::string> suffixed_names;
    for (const ChessMove &move : legal_moves) {
        base_names.push_back(current_pos.get_move_name(move, false));
        suffixed_names.push_back(current_pos.get_move_name(move, true));
    }

    // loop until user supplies a legal move
    while (true) {

        // get command from stdin and trim leading/trailing whitespace
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
            // search for move with name matching command
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

        // use if statement to avoid unnecessary evaluation of get_fen()
        if (verbose) {
            std::cout << current_pos.get() << current_pos.get_fen()
                      << std::endl;
        }
        assert(current_pos.get().check_consistency());

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

        const std::vector<ChessMove> &legal_moves =
            current_pos.get_legal_moves();
        ChessEngine *const current_engine =
            current_pos.get_color_to_move() == PieceColor::WHITE ? white
                                                                 : black;
        if (current_engine == nullptr) {
            const ChessMove chosen_move = get_console_move();
            if (chosen_move != NULL_MOVE) {
                assert(current_pos.get().is_legal(chosen_move));
                make_move(chosen_move);
            }
        } else {
            const ChessMove chosen_move = current_engine->pick_move(
                current_pos.get(), legal_moves, pos_history, move_history
            );
            // use if statement to avoid unnecessary
            // evaluation of get_move_name()
            if (verbose) {
                std::cout << "Chosen move: "
                          << current_pos.get_move_name(chosen_move, true)
                          << std::endl;
            }
            assert(current_pos.get().is_legal(chosen_move));
            make_move(chosen_move);
        }
    }
}


std::string ChessGame::get_pgn(
    const std::string &event_name,
    long long num_round,
    const std::string &white_name,
    const std::string &black_name
) const {

    using enum GameStatus;
    std::ostringstream result;

    // metadata
    if (!event_name.empty()) { result << "[Event \"" << event_name << "\"]\n"; }
    result << "[Site \"https://github.com/alexmingzhang/SuckerChess/\"]\n";
    result << "[Date \"" << get_ymd_date('.') << "\"]\n";
    if (num_round != -1) { result << "[Round \"" << num_round << "\"]\n"; }
    if (!white_name.empty()) { result << "[White \"" << white_name << "\"]\n"; }
    if (!black_name.empty()) { result << "[Black \"" << black_name << "\"]\n"; }

    // game status
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
    result << "\"]\n\n";

    for (std::size_t i = 0; i < move_history.size(); ++i) {
        const std::string move_name = pos_history[i].get_move_name(
            pos_history[i].get_legal_moves(), move_history[i], true
        );
        if (i % 2 == 0) {
            if (i > 0) { result << ' '; }
            result << (i / 2 + 1) << ". " << move_name;
        } else {
            result << ' ' << move_name;
        }
    }
    switch (current_status) {
        case IN_PROGRESS: break;
        case WHITE_WON_BY_CHECKMATE: result << " 1-0"; break;
        case BLACK_WON_BY_CHECKMATE: result << " 0-1"; break;
        case DRAWN_BY_STALEMATE: [[fallthrough]];
        case DRAWN_BY_INSUFFICIENT_MATERIAL: [[fallthrough]];
        case DRAWN_BY_REPETITION: [[fallthrough]];
        case DRAWN_BY_50_MOVE_RULE: result << " 1/2-1/2"; break;
    }
    result << '\n';

    return result.str();
}