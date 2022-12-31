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


ChessGame ChessPlayer::versus(
    ChessPlayer &black, double elo_k_factor, int verbose_level
) {

    ChessPlayer &white = *this; // local alias for naming symmetry
    ChessGame game;

    if (verbose_level > 0) {
        std::cout << std::right << std::setw(16) << white.get_name_with_elo(0)
                  << " vs " << std::left << std::setw(16)
                  << black.get_name_with_elo(0) << " Result: ";
    }

    const double white_transformed_elo = std::pow(10.0, white.elo / 400.0);
    const double black_transformed_elo = std::pow(10.0, black.elo / 400.0);
    const double total = white_transformed_elo + black_transformed_elo;
    const double white_expected_score = white_transformed_elo / total;
    const double black_expected_score = black_transformed_elo / total;

    PieceColor winner =
        game.run(white.engine.get(), black.engine.get(), verbose_level > 1);
    switch (winner) {
        case PieceColor::NONE:
            if (verbose_level > 0) { std::cout << "Draw.\n"; }
            ++white.num_draws;
            ++black.num_draws;
            white.elo += elo_k_factor * (0.5 - white_expected_score);
            black.elo += elo_k_factor * (0.5 - black_expected_score);
            break;
        case PieceColor::WHITE:
            if (verbose_level > 0) { std::cout << white.name << " won!\n"; }
            ++white.num_wins_as_white;
            ++black.num_losses_as_black;
            white.elo += elo_k_factor * (1.0 - white_expected_score);
            black.elo += elo_k_factor * (0.0 - black_expected_score);
            break;
        case PieceColor::BLACK:
            if (verbose_level > 0) { std::cout << black.name << " won!\n"; }
            ++white.num_losses_as_white;
            ++black.num_wins_as_black;
            white.elo += elo_k_factor * (0.0 - white_expected_score);
            black.elo += elo_k_factor * (1.0 - black_expected_score);
            break;
    }
    return game;
}
