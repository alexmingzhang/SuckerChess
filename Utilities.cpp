#include "Utilities.hpp"

#include <algorithm>  // for std::generate
#include <cstddef>    // for std::size_t
#include <functional> // for std::ref
#include <iterator>   // for std::begin, std::end
#include <random>     // for std::random_device, std::seed_seq


std::mt19937 properly_seeded_random_engine() {
    constexpr std::size_t SEED_SIZE =
        std::mt19937::state_size * sizeof(typename std::mt19937::result_type);
    std::random_device seed_source;
    std::random_device::result_type
        seed_data[(SEED_SIZE - 1) / sizeof(seed_source()) + 1];
#ifdef SUCKER_CHESS_USE_DETERMINISTIC_SEED
    static std::random_device::result_type seed_value = 0;
    std::generate(std::begin(seed_data), std::end(seed_data), []() {
        return seed_value;
    });
    ++seed_value;
#else
    std::generate(
        std::begin(seed_data), std::end(seed_data), std::ref(seed_source)
    );
#endif
    std::seed_seq seed(std::begin(seed_data), std::end(seed_data));
    return std::mt19937(seed);
}
