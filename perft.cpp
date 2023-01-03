#include <chrono>   // for std::chrono
#include <iostream> // for std::cout, std::cerr, std::endl
#include <string>   // for std::string

#include "src/ChessMove.hpp"
#include "src/ChessPosition.hpp"


static constexpr unsigned long long
perft(const ChessPosition &pos, int depth) noexcept {
    if (depth <= 0) {
        return 1;
    } else {
        unsigned long long result = 0;
        pos.visit_legal_moves([&](ChessMove, const ChessPosition &next) {
            result += perft(next, depth - 1);
        });
        return result;
    }
}


static bool perft_test(
    const std::string &fen, int depth, unsigned long long expected_result
) noexcept {
    const ChessPosition pos(fen);
    const unsigned long long actual_result = perft(pos, depth);
    if (actual_result == expected_result) {
        return true;
    } else {
        std::cerr << "ERROR: Incorrect perft count for position: " << fen
                  << std::endl;
        std::cerr << "Computed " << actual_result << ", expected "
                  << expected_result << std::endl;
        return false;
    }
}


int main() {

    // Reference perft data from:
    // https://www.chessprogramming.org/Perft_Results

    bool all_tests_passed = true;
    const auto begin = std::chrono::high_resolution_clock::now();

    // Initial Position
    // clang-format off
    all_tests_passed &= perft_test("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -", 0, 1ULL);
    all_tests_passed &= perft_test("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -", 1, 20ULL);
    all_tests_passed &= perft_test("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -", 2, 400ULL);
    all_tests_passed &= perft_test("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -", 3, 8'902ULL);
    all_tests_passed &= perft_test("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -", 4, 197'281ULL);
    all_tests_passed &= perft_test("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -", 5, 4'865'609ULL);
    // clang-format on

    // Position 2
    // clang-format off
    all_tests_passed &= perft_test("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", 0, 1ULL);
    all_tests_passed &= perft_test("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", 1, 48ULL);
    all_tests_passed &= perft_test("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", 2, 2'039ULL);
    all_tests_passed &= perft_test("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", 3, 97'862ULL);
    all_tests_passed &= perft_test("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", 4, 4'085'603ULL);
    // clang-format on

    // Position 3
    // clang-format off
    all_tests_passed &= perft_test("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -", 0, 1ULL);
    all_tests_passed &= perft_test("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -", 1, 14ULL);
    all_tests_passed &= perft_test("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -", 2, 191ULL);
    all_tests_passed &= perft_test("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -", 3, 2'812ULL);
    all_tests_passed &= perft_test("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -", 4, 43'238ULL);
    all_tests_passed &= perft_test("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -", 5, 674'624ULL);
    all_tests_passed &= perft_test("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -", 6, 11'030'083ULL);
    // clang-format on

    // Position 4
    // clang-format off
    all_tests_passed &= perft_test("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq -", 0, 1ULL);
    all_tests_passed &= perft_test("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq -", 1, 6ULL);
    all_tests_passed &= perft_test("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq -", 2, 264ULL);
    all_tests_passed &= perft_test("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq -", 3, 9'467ULL);
    all_tests_passed &= perft_test("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq -", 4, 422'333ULL);
    all_tests_passed &= perft_test("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq -", 5, 15'833'292ULL);
    // clang-format on

    // Position 4 Mirrored
    // clang-format off
    all_tests_passed &= perft_test("r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1", 0, 1ULL);
    all_tests_passed &= perft_test("r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1", 1, 6ULL);
    all_tests_passed &= perft_test("r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1", 2, 264ULL);
    all_tests_passed &= perft_test("r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1", 3, 9'467ULL);
    all_tests_passed &= perft_test("r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1", 4, 422'333ULL);
    // clang-format on

    // Position 5
    // clang-format off
    all_tests_passed &= perft_test("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ -", 0, 1ULL);
    all_tests_passed &= perft_test("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ -", 1, 44ULL);
    all_tests_passed &= perft_test("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ -", 2, 1'486ULL);
    all_tests_passed &= perft_test("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ -", 3, 62'379ULL);
    all_tests_passed &= perft_test("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ -", 4, 2'103'487ULL);
    // clang-format on

    // Position 6
    // clang-format off
    all_tests_passed &= perft_test("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - -", 0, 1ULL);
    all_tests_passed &= perft_test("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - -", 1, 46ULL);
    all_tests_passed &= perft_test("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - -", 2, 2'079ULL);
    all_tests_passed &= perft_test("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - -", 3, 89'890ULL);
    all_tests_passed &= perft_test("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - -", 4, 3'894'594ULL);
    // clang-format on

    if (all_tests_passed) {
        const auto end = std::chrono::high_resolution_clock::now();
        const auto delta = end - begin;
        const double duration =
            static_cast<double>(
                std::chrono::duration_cast<std::chrono::nanoseconds>(delta)
                    .count()
            ) /
            1'000'000'000.0;
        std::cout << "All tests passed in " << duration << " seconds."
                  << std::endl;
    }
}
