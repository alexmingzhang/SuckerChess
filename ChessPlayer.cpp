#include "ChessPlayer.hpp"

#include <iomanip>
#include <iostream> // for std::cout
#include <sstream>

const std::string &ChessPlayer::get_name() const { return name; }

std::string ChessPlayer::get_name_with_elo(int precision) const {
    std::ostringstream result;
    result << name << " (" << std::setprecision(precision) << std::fixed
           << get_elo() << ")";
    return result.str();
}


double ChessPlayer::get_elo() const { return elo; }

unsigned int ChessPlayer::get_num_wins() const { return num_wins; }

unsigned int ChessPlayer::get_num_draws() const { return num_draws; }

unsigned int ChessPlayer::get_num_losses() const { return num_losses; }


// *this white, other black
ChessGame ChessPlayer::versus(ChessPlayer &other) {
    ChessGame game;

    std::cout << std::right << std::setw(16) << this->get_name_with_elo(0)
              << " vs " << std::left << std::setw(16)
              << other.get_name_with_elo(0) << " Result: ";

    double this_transformed_elo = std::pow(10, this->get_elo() / 400.0);
    double other_transformed_elo = std::pow(10, other.get_elo() / 400.0);

    double this_expected_score =
        this_transformed_elo / (this_transformed_elo + other_transformed_elo);
    double other_expected_score =
        other_transformed_elo / (this_transformed_elo + other_transformed_elo);

    PieceColor winner = game.run(this->engine, other.engine, false);

    switch (winner) {
        case PieceColor::WHITE:
            std::cout << this->name << " won!" << std::endl;
            this->num_wins++;
            other.num_losses++;
            this->elo += KFACTOR * (1.0 - this_expected_score);
            other.elo += KFACTOR * (0.0 - other_expected_score);
            break;
        case PieceColor::BLACK:
            std::cout << other.name << " won!" << std::endl;
            this->num_losses++;
            other.num_wins++;
            this->elo += KFACTOR * (0.0 - this_expected_score);
            other.elo += KFACTOR * (1.0 - other_expected_score);
            break;
        case PieceColor::NONE:
            std::cout << "Draw." << std::endl;
            this->num_draws++;
            other.num_draws++;
            this->elo += KFACTOR * (0.5 - this_expected_score);
            other.elo += KFACTOR * (0.5 - other_expected_score);
            break;
        default: __builtin_unreachable();
    }

    return game;
}
