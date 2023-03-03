#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>

#include "src/ChessGame.hpp"
#include "src/GenePool.hpp"
#include "src/Utilities.hpp"

int main() {
    using enum PreferenceToken;
    std::vector<PreferenceToken> best = {
        MATE_IN_ONE,
        PREVENT_MATE_IN_ONE,
        OUTPOST,
        CAPTURE,
        SMART_CAPTURE,
        PREVENT_DRAW,
        CONQUEROR,
        GENEROUS,
        HUDDLE,
        HERO,
        SLOTH,
        SNIPER,
        EXPLORE,
        FIRST,
        EXTEND};

    Engine::PreferenceChain *engine = new Engine::PreferenceChain(best);

    ChessGame game;
    game.run(nullptr, engine);

    return EXIT_SUCCESS;
}
