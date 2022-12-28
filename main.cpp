#include <algorithm>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <string>

#include "ChessEngine.hpp"
#include "ChessGame.hpp"
#include "ChessTournament.hpp"

ChessGame find_cool_game() {
    PieceColor winner;
    while (1) {
        ChessGame game;
        if (game.run(new Engine::CCCP(), new Engine::FirstLegalMove(), false) ==
            PieceColor::BLACK) {
            return game;
        }
    }
}

int main() {
    ChessPlayer *flm_player =
        new ChessPlayer("FLM", new Engine::FirstLegalMove());
    ChessPlayer *random_player =
        new ChessPlayer("Random", new Engine::Random());
    ChessPlayer *lazy_player = new ChessPlayer("Lazy", new Engine::Lazy());
    ChessPlayer *energetic_player =
        new ChessPlayer("Energetic", new Engine::Lazy());

    ChessPlayer *cccp_player = new ChessPlayer("CCCP", new Engine::CCCP());
    ChessPlayer *reducer_player =
        new ChessPlayer("Reducer", new Engine::Reducer());

    ChessTournament tourney("SuckerChess Tournament");
    tourney.add_player(flm_player);
    tourney.add_player(random_player);
    tourney.add_player(lazy_player);
    tourney.add_player(energetic_player);
    tourney.add_player(cccp_player);
    tourney.add_player(reducer_player);

    tourney.run(100, 10);
    tourney.sort_players_by_elo();

    tourney.print_info();

    return 0;
}
