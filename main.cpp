#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

#include "ChessMove.hpp"
#include "ChessPosition.hpp"


int main() {
    ChessPosition pos;
    bool pos_changed = true;

    while (true) {

        if (pos.checkmated()) {
            if (pos.get_color_to_move() == PieceColor::WHITE) {
                std::cout << "Checkmate! Black wins." << std::endl;
            } else if (pos.get_color_to_move() == PieceColor::BLACK) {
                std::cout << "Checkmate! White wins." << std::endl;
            }
            break;
        }

        if (pos.stalemated()) {
            std::cout << "Stalemate! The game is a draw." << std::endl;
            break;
        }

        if (pos_changed) {
            std::cout << pos << std::endl;
            if (pos.get_color_to_move() == PieceColor::WHITE) {
                std::cout << "White to move." << std::endl;
            } else if (pos.get_color_to_move() == PieceColor::BLACK) {
                std::cout << "Black to move." << std::endl;
            }
        }

        std::vector<ChessMove> moves = pos.get_legal_moves();
        std::vector<std::string> names;
        for (const ChessMove &move : moves) {
            names.push_back(pos.get_move_name(moves, move));
        }

        std::string command;
        std::cout << "> ";
        std::cin >> command;

        if (command == "ls") {
            std::cout << "Legal moves:";
            for (const std::string &name : names) { std::cout << ' ' << name; }
            std::cout << std::endl;
            pos_changed = false;
        } else {
            bool found = false;
            for (std::size_t i = 0; i < moves.size(); ++i) {
                if (command == names[i]) {
                    assert(!found);
                    pos.make_move(moves[i]);
                    pos_changed = true;
                    found = true;
                }
            }
            if (!found) {
                std::cout << "ERROR: " << command
                          << " is not a legal move in this position."
                          << std::endl;
                pos_changed = false;
            }
        }
    }
    return 0;
}
