#include <cstddef>  // for std::size_t
#include <iostream> // for std::cout, std::cerr, std::endl
#include <memory>   // for std::unique_ptr, std::make_unique
#include <string>   // for std::string

#include "src/ChessEngine.hpp"
#include "src/ChessGame.hpp"
#include "src/ChessPlayer.hpp"
#include "src/ChessPosition.hpp"
#include "src/ChessTournament.hpp"
#include "src/Utilities.hpp"


static void self_test(unsigned long long num_games) {
    auto rng = properly_seeded_random_engine();
    for (unsigned long long count = 0; count < num_games; ++count) {
        ChessPosition pos;
        while (true) {
            if (!pos.check_consistency()) {
                std::cerr << "FATAL ERROR: Inconsistent chess position found."
                          << std::endl;
                std::cerr << pos << std::endl;
                std::cerr << pos.get_fen() << std::endl;
                return;
            }
            if (pos.get_board().has_insufficient_material()) { break; }
            const auto moves = pos.get_legal_moves();
            if (moves.empty()) { break; }
            const auto chosen_move = random_choice(rng, moves);
            if (!pos.is_valid(chosen_move)) {
                std::cerr << "FATAL ERROR: Invalid chess move found."
                          << std::endl;
                std::cerr << pos << std::endl;
                std::cerr << pos.get_fen() << std::endl;
                std::cerr << chosen_move << std::endl;
                return;
            }
            pos.make_move(random_choice(rng, moves));
        }
        std::cout << "Completed " << (count + 1)
                  << ((count == 0) ? " random self-test game."
                                   : " random self-test games.")
                  << std::endl;
    }
}


static void benchmark(unsigned long long num_games) {
    std::unique_ptr<ChessEngine> white = std::make_unique<Engine::Preference>();
    std::unique_ptr<ChessEngine> black = std::make_unique<Engine::Preference>();
    for (unsigned long long i = 0; i < num_games; ++i) {
        ChessGame game;
        game.run(white.get(), black.get(), false);
    }
}

/*
int main(int argc, char **argv) {
    for (int i = 0; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--test") {
            self_test(100);
            return EXIT_SUCCESS;
        } else if (arg == "--benchmark") {
            benchmark(1000);
            return EXIT_SUCCESS;
        }
    }
    return EXIT_FAILURE;
}
*/

int main() {
    std::unique_ptr<Engine::Preference> engine_1 =
        std::make_unique<Engine::Preference>();
    engine_1->add_preference<Preference::MateInOne>();
    engine_1->add_preference<Preference::PreventMateInOne>();
    engine_1->add_preference<Preference::PreventDraw>();
    engine_1->add_preference<Preference::SmartCapture>();
    engine_1->add_preference<Preference::Outpost>();
    engine_1->add_preference<Preference::Greedy>();


    std::unique_ptr<Engine::Preference> engine_2 =
        std::make_unique<Engine::Preference>();
    engine_2->add_preference<Preference::MateInOne>();
    engine_2->add_preference<Preference::PreventMateInOne>();
    engine_2->add_preference<Preference::PreventDraw>();
    engine_2->add_preference<Preference::CaptureHanging>();
    engine_2->add_preference<Preference::Outpost>();

    std::unique_ptr<ChessEngine> CCCP_engine = std::make_unique<Engine::CCCP>();

    // Play against engine
    // ChessGame game;
    // game.current_pos.load_fen(
    //     "rnbqkbnr/ppp1p1pp/8/3p1p2/4P3/5P1P/PPPP2P1/RNBQKBNR b KQkq - 0 3"
    // );
    // game.run(nullptr, engine_1.get(), true);
    // std::cout << game.get_pgn() << std::endl;

    std::unique_ptr<ChessPlayer> smartie_1 = std::make_unique<ChessPlayer>(
        ChessPlayer(std::move(engine_1), "Smartie 1")
    );

    std::unique_ptr<ChessPlayer> smartie_2 = std::make_unique<ChessPlayer>(
        ChessPlayer(std::move(engine_2), "Smartie 2")
    );


    std::unique_ptr<ChessPlayer> CCCP_Player = std::make_unique<ChessPlayer>(
        ChessPlayer(std::move(CCCP_engine), "CCCP")
    );


    std::unique_ptr<ChessPlayer> dumbie = std::make_unique<ChessPlayer>(
        ChessPlayer({PreferenceToken::MATE_IN_ONE}, "Dumbie")
    );

    // Engine 1v1
    // ChessGame one_v_one = smartie_1->versus(*smartie_2, 0.0, 2);
    // std::cout << one_v_one.get_pgn() << std::endl;

    // Round-Robin Chess Tournament
    ChessTournament tourney;
    tourney.add_player(std::move(smartie_1));
    tourney.add_player(std::move(smartie_2));
    tourney.add_player(std::move(CCCP_Player));
    tourney.add_player(std::move(dumbie));
    tourney.run(100, 1, false);
}
