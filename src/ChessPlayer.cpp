#include "ChessPlayer.hpp"

#include <cmath> // for std::pow
#include <iomanip> // for std::left, std::right, std::setw, std::fixed, std::setprecision
#include <iostream> // for std::cout
#include <sstream>  // for std::ostringstream


std::string ChessPlayer::get_name_with_elo(int precision) const {
    std::ostringstream result;
    result << name << " (" << std::setprecision(precision) << std::fixed << elo
           << ")";
    return result.str();
}


ChessGame ChessPlayer::versus(ChessPlayer &black, int verbose_level) {

    ChessPlayer &white = *this; // local alias for naming symmetry
    ChessGame game;

    if (verbose_level > 0) {
        std::cout << std::right << std::setw(16) << white.get_name_with_elo(0)
                  << " vs " << std::left << std::setw(16)
                  << black.get_name_with_elo(0) << " Result: ";
    }

    white.accum_expected_score +=
        1.0 / (1.0 + std::pow(10.0, (black.elo - white.elo) / 400));

    black.accum_expected_score +=
        1.0 / (1.0 + std::pow(10.0, (white.elo - black.elo) / 400));


    PieceColor winner =
        game.run(white.engine.get(), black.engine.get(), verbose_level > 1);
    switch (winner) {
        case PieceColor::NONE:
            if (verbose_level > 0) { std::cout << "Draw.\n"; }
            ++white.num_draws;
            ++black.num_draws;
            white.accum_actual_score += 0.5;
            black.accum_actual_score += 0.5;
            break;
        case PieceColor::WHITE:
            if (verbose_level > 0) { std::cout << white.name << " won!\n"; }
            ++white.num_wins_as_white;
            ++black.num_losses_as_black;
            white.accum_actual_score += 1.0;
            break;
        case PieceColor::BLACK:
            if (verbose_level > 0) { std::cout << black.name << " won!\n"; }
            ++white.num_losses_as_white;
            ++black.num_wins_as_black;
            black.accum_actual_score += 1.0;
            break;
    }
    return game;
}


void ChessPlayer::update_elo(double elo_k_factor) {
    elo += elo_k_factor * (accum_actual_score - accum_expected_score);
    accum_actual_score = 0;
    accum_expected_score = 0;
}
