#include <algorithm>
#include <cstddef>
#include <iostream>
#include <string>

#include "ChessEngine.hpp"
#include "ChessGame.hpp"
#include "ChessTournament.hpp"


int main() {
    ChessPlayer *flm_player =
        new ChessPlayer("FLM", new Engine::FirstLegalMove());
    ChessPlayer *random_player =
        new ChessPlayer("Random", new Engine::Random());
    ChessPlayer *slug_player = new ChessPlayer("Slug", new Engine::Slug());

    ChessPlayer *cccp_player = new ChessPlayer("CCCP", new Engine::CCCP());
    ChessPlayer *reducer_player =
        new ChessPlayer("Reducer", new Engine::Reducer());

    ChessTournament tourney("SuckerChess Tournament");
    tourney.add_player(flm_player);
    tourney.add_player(random_player);
    tourney.add_player(slug_player);
    tourney.add_player(cccp_player);
    tourney.add_player(reducer_player);

    tourney.run(100, true);
    tourney.sort_players_by_elo();

    std::cout << "Tournament results: \n";
    for (const ChessPlayer *p : tourney.get_players()) {
        std::cout << p->get_name_with_elo(2) << ": " << p->get_num_wins() << '-'
                  << p->get_num_draws() << '-' << p->get_num_losses()
                  << std::endl;
    }
    return 0;
}
