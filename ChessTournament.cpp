// TODO: add tournament name in PGN of individual games

#include "ChessTournament.hpp"

#include <cmath> // for std::exp10
#include <iostream>

const std::string &ChessPlayer::get_name() const { return name; }

double ChessPlayer::get_elo() const { return elo; }

unsigned int ChessPlayer::get_num_wins() const { return num_wins; }

unsigned int ChessPlayer::get_num_draws() const { return num_draws; }

unsigned int ChessPlayer::get_num_losses() const { return num_losses; }


// *this white, other black
ChessGame ChessPlayer::versus(ChessPlayer &other) {
    ChessGame game;

    std::cout << "NEW GAME! White is " << this->get_name() << " ("
              << this->get_elo() << ") , black is " << other.get_name() << " ("
              << other.get_elo() << ")" << std::endl;

    double this_transformed_elo = std::pow(10, this->get_elo() / 400.0);
    double other_transformed_elo = std::pow(10, other.get_elo() / 400.0);

    double this_expected_score =
        this_transformed_elo / (this_transformed_elo + other_transformed_elo);
    double other_expected_score =
        other_transformed_elo / (this_transformed_elo + other_transformed_elo);

    PieceColor winner = game.run(this->engine, other.engine, false);

    switch (winner) {
        case PieceColor::WHITE:
            std::cout << this->name << " WON!" << std::endl;
            this->num_wins++;
            other.num_losses++;
            this->elo += KFACTOR * (1.0 - this_expected_score);
            other.elo += KFACTOR * (0.0 - other_expected_score);
            break;
        case PieceColor::BLACK:
            std::cout << other.name << " WON!" << std::endl;
            this->num_losses++;
            other.num_wins++;
            this->elo += KFACTOR * (0.0 - this_expected_score);
            other.elo += KFACTOR * (1.0 - other_expected_score);
            break;
        case PieceColor::NONE:
            std::cout << "DRAW!" << std::endl;
            this->num_draws++;
            other.num_draws++;
            this->elo += KFACTOR * (0.5 - this_expected_score);
            other.elo += KFACTOR * (0.5 - other_expected_score);
            break;
        default: __builtin_unreachable();
    }

    return game;
}

void ChessTournament::add_player(ChessPlayer *player) {
    players.push_back(player);
}

const std::vector<ChessPlayer *> &ChessTournament::get_players() const {
    return players;
}

const std::vector<ChessGame> &ChessTournament::get_game_history() const {
    return game_history;
}

// Run every possible distinct matchup
void ChessTournament::run(unsigned int times) {
    for (unsigned int n = 0; n < times; ++n) {
        for (std::size_t i = 0; i < players.size(); ++i) {
            ChessPlayer *p1 = players[i];

            for (std::size_t j = i + 1; j < players.size(); ++j) {
                ChessPlayer *p2 = players[j];

                p1->versus(*p2);
                p2->versus(*p1);
            }
        }
    }
}