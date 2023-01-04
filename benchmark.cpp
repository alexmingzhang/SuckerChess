#include <chrono>   // for std::chrono
#include <cstdlib>  // for EXIT_SUCCESS, EXIT_FAILURE
#include <iostream> // for std::cout, std::endl
#include <memory>   // for std::unique_ptr, std::make_unique

#include "src/ChessEngine.hpp"
#include "src/ChessGame.hpp"
#include "src/Engine/Random.hpp"


int main() {

    std::unique_ptr<ChessEngine> white = std::make_unique<Engine::Random>();
    std::unique_ptr<ChessEngine> black = std::make_unique<Engine::Random>();
    const auto begin = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1000; ++i) {
        ChessGame game;
        game.run(white.get(), black.get(), false);
    }

    const auto end = std::chrono::high_resolution_clock::now();
    const auto delta = end - begin;
    const double duration =
        static_cast<double>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(delta).count()
        ) /
        1'000'000'000.0;
    std::cout << "Completed benchmark in " << duration << " seconds."
              << std::endl;

    return EXIT_SUCCESS;
}
