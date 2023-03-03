#include "ChessTournament.hpp"
#include "ChessEngine.hpp"
#include "Utilities.hpp"

#include <algorithm> // for std::shuffle, std::sort
#include <climits>   // for INT_MAX
#include <cstddef>   // for std::size_t
#include <iomanip>   // for std::left, std::right, std::setw
#include <iostream>  // for std::cout, std::endl, std::flush


void ChessTournament::add_engine(std::unique_ptr<ChessEngine> &&engine
) noexcept {
    const std::size_t w = engine->get_name().size();
    if (w >= static_cast<std::size_t>(INT_MAX)) {
        name_width = INT_MAX;
    } else if (static_cast<int>(w) > name_width) {
        name_width = static_cast<int>(w);
    }
    engines.emplace_back(std::move(engine), PerformanceInfo{0, 0, 0, 0, 0});
}


void ChessTournament::sort_players_by_win_ratio() {
    std::sort(
        engines.begin(),
        engines.end(),
        [&](const auto &a, const auto &b) {
            const std::size_t a_wins = a.second.total_wins();
            const std::size_t a_losses = a.second.total_losses();
            const std::size_t b_wins = b.second.total_wins();
            const std::size_t b_losses = b.second.total_losses();

            const double a_ratio =
                static_cast<double>(a_wins) / static_cast<double>(a_losses);
            const double b_ratio =
                static_cast<double>(b_wins) / static_cast<double>(b_losses);

            if (a_ratio == b_ratio) {
                if (a_wins == b_wins) {
                    return a.second.num_draws < b.second.num_draws;
                }
                return a_wins > b_wins;
            }
            return a_ratio > b_ratio;
        }
    );
}


void ChessTournament::run(long long num_rounds, long long print_frequency) {

    const bool infinite_rounds = (num_rounds == -1);
    const bool enable_printing = (print_frequency != -1);
    const bool verbose = (print_frequency == 0);

    // Pairs of player indices; 1st player is white, 2nd player is black
    std::vector<std::pair<std::size_t, std::size_t>> matchups;
    for (std::size_t i = 0; i < engines.size(); ++i) {
        for (std::size_t j = 0; j < engines.size(); ++j) {
            if (i != j) { matchups.emplace_back(i, j); }
        }
    }

    const long long final_round = current_round + num_rounds;
    while (infinite_rounds || (current_round < final_round)) {

        ++current_round;
        if (verbose) {
            std::cout << get_name() << ": Running round " << current_round
                      << "..." << std::endl;
        }

        // Randomize all matchups
        std::shuffle(matchups.begin(), matchups.end(), rng);

        // Play each matchup
        for (const auto [i, j] : matchups) {
            auto &[white_engine, white_info] = engines[i];
            auto &[black_engine, black_info] = engines[j];
            if (verbose) {
                std::cout << std::right << std::setw(name_width)
                          << white_engine->get_name() << " vs. " << std::left
                          << std::setw(name_width) << black_engine->get_name()
                          << ": ";
            }
            ChessGame game;
            PieceColor winner =
                game.run(white_engine.get(), black_engine.get(), false);
            switch (winner) {
                case PieceColor::NONE:
                    if (verbose) { std::cout << "Draw." << std::endl; }
                    ++white_info.num_draws;
                    ++black_info.num_draws;
                    break;
                case PieceColor::WHITE:
                    if (verbose) {
                        std::cout << white_engine->get_name() << " won!"
                                  << std::endl;
                    }
                    ++white_info.num_wins_as_white;
                    ++black_info.num_losses_as_black;
                    break;
                case PieceColor::BLACK:
                    if (verbose) {
                        std::cout << black_engine->get_name() << " won!"
                                  << std::endl;
                    }
                    ++white_info.num_losses_as_white;
                    ++black_info.num_wins_as_black;
                    break;
            }
        }

        const bool should_print =
            verbose ||
            (enable_printing && (current_round % print_frequency == 0));

        if (should_print) {
            sort_players_by_win_ratio();
            print_info();
        }
    }
}


void ChessTournament::print_info() const {

    const std::size_t num_games = static_cast<std::size_t>(current_round) *
                                  engines.size() * (engines.size() - 1);

    std::cout << "Results after " << current_round
              << ((current_round == 1) ? " round (" : " rounds (") << num_games
              << " games):\n";
    std::cout << "      " << std::setw(name_width) << "Engine"
              << " :   W (w/b)   :   D   :   L (w/b)   : "
                 "  WLR   \n";

    for (std::size_t i = 0; i < engines.size(); ++i) {
        const auto &[engine, info] = engines[i];
        std::cout << std::right << std::setw(4) << (i + 1) << ". ";
        std::cout << std::left << std::setw(name_width) << engine->get_name()
                  << " : ";
        std::cout << std::right << std::setw(5) << info.num_wins_as_white << '/'
                  << std::left << std::setw(5) << info.num_wins_as_black
                  << " : ";
        std::cout << std::right << std::setw(5) << info.num_draws << " : ";
        std::cout << std::right << std::setw(5) << info.num_losses_as_white
                  << '/' << std::left << std::setw(5)
                  << info.num_losses_as_black << " : ";
        std::cout << std::setw(5)
                  << (static_cast<double>(info.total_wins()) /
                      static_cast<double>(info.total_losses()));
        std::cout << '\n';
    }
    std::cout << std::flush;
}
