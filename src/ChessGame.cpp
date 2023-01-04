#include "ChessGame.hpp"

#include <cassert>  // for assert
#include <cstddef>  // for std::size_t
#include <iostream> // for std::cin, std::cout, std::endl
#include <sstream>  // for std::ostringstream
#include <string>   // for std::getline

#include "Utilities.hpp"


ChessGame::ChessGame() noexcept
    : m_interface()
    , m_status(GameStatus::IN_PROGRESS)
    , m_pos_history()
    , m_move_history()
    , m_half_move_clock(0)
    , m_full_move_count(1) {}


GameStatus ChessGame::compute_current_status() noexcept {

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


ChessMove ChessGame::get_console_move() {

    // retrieve legal moves and names, with and without +/# suffixes
    const std::vector<ChessMove> &legal_moves = m_interface.get_legal_moves();
    std::vector<std::string> base_names;
    std::vector<std::string> suffixed_names;
    for (const ChessMove &move : legal_moves) {
        base_names.push_back(m_interface.get_current_pos().get_move_name(
            legal_moves, move, false
        ));
        suffixed_names.push_back(
            m_interface.get_current_pos().get_move_name(legal_moves, move, true)
        );
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
            std::cout << get_fen() << std::endl;
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


void ChessGame::make_move(ChessMove move) noexcept {

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


static void println(bool verbose, const char *str) noexcept {
    if (verbose) { std::cout << str << std::endl; }
}


PieceColor
ChessGame::run(ChessEngine *white, ChessEngine *black, bool verbose) {

    using enum GameStatus;

    while (get_current_status() == IN_PROGRESS) {

        if (verbose) {
            std::cout << m_interface.get_current_pos()
                      << m_interface.get_current_pos().get_fen() << std::endl;
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
            const ChessMove move = get_console_move();
            if (move != NULL_MOVE) {
                assert(m_interface.get_current_pos().is_valid(move));
                assert(contains(m_interface.get_legal_moves(), move));
                make_move(move);
            }
        } else {
            const ChessMove move =
                player->pick_move(m_interface, m_pos_history, m_move_history);
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
                  << m_interface.get_current_pos().get_fen() << std::endl;
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


std::string ChessGame::get_fen() {
    std::ostringstream result;
    result << m_interface.get_current_pos().get_fen() << ' '
           << get_half_move_clock() << ' ' << get_full_move_count();
    return result.str();
}


std::string ChessGame::get_pgn(
    const std::string &event_name,
    long long int num_round,
    const std::string &white_name,
    const std::string &black_name
) {
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
    switch (get_current_status()) {
        case IN_PROGRESS: result << "*"; break;
        case WHITE_WON_BY_CHECKMATE: result << "1-0"; break;
        case BLACK_WON_BY_CHECKMATE: result << "0-1"; break;
        case DRAWN_BY_STALEMATE: [[fallthrough]];
        case DRAWN_BY_INSUFFICIENT_MATERIAL: [[fallthrough]];
        case DRAWN_BY_REPETITION: [[fallthrough]];
        case DRAWN_BY_50_MOVE_RULE: result << "1/2-1/2"; break;
    }
    result << "\"]\n\n";

    // move text
    const auto &pos_history = get_pos_history();
    const auto &move_history = get_move_history();
    for (std::size_t i = 0; i < move_history.size(); ++i) {
        const std::string move_name = pos_history[i].get_move_name(
            m_interface.get_legal_moves(pos_history[i]), move_history[i], true
        );
        if (i % 2 == 0) {
            if (i > 0) { result << ' '; }
            result << (i / 2 + 1) << ". " << move_name;
        } else {
            result << ' ' << move_name;
        }
    }
    switch (get_current_status()) {
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
