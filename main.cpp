#include <cstddef>
#include <iostream>
#include <string>

#include "ChessEngine.hpp"
#include "ChessGame.hpp"
#include "ChessTournament.hpp"


int main() {
    ChessPlayer *random_player = new ChessPlayer("Random", new RandomEngine());
    ChessPlayer *cccp_player = new ChessPlayer("CCCP", new CCCP_Engine());
    ChessPlayer *reducer_player =
        new ChessPlayer("Reducer", new ReducerEngine());

    ChessTournament tourney("SuckerChess Tournament");
    tourney.add_player(random_player);
    tourney.add_player(cccp_player);
    tourney.add_player(reducer_player);

    tourney.run(10);

    std::cout << "Tournament results: \n";
    for (const ChessPlayer *p : tourney.get_players()) {
        std::cout << p->get_name_with_elo(2) << ": " << p->get_num_wins() << '-'
                  << p->get_num_draws() << '-' << p->get_num_losses()
                  << std::endl;
    }

    return 0;
}
