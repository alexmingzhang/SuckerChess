#include "Utilities.hpp"

#include <algorithm>  // for std::find_if, std::generate
#include <cctype>     // for std::isspace
#include <ctime>      // for std::time, std::time_t, std::tm, std::localtime
#include <functional> // for std::ref
#include <iterator>   // for std::begin, std::end
#include <random>     // for std::random_device, std::seed_seq
#include <sstream>    // for std::ostringstream


void trim(std::string &str) {
    const auto first_char = std::find_if(str.begin(), str.end(), [](char ch) {
        return !std::isspace(ch);
    });
    str.erase(str.begin(), first_char);
    const auto last_char = std::find_if(str.rbegin(), str.rend(), [](char ch) {
        return !std::isspace(ch);
    });
    str.erase(last_char.base(), str.end());
}


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


std::string get_ymd_date(char sep) {
    std::ostringstream date;
    std::time_t time = std::time(nullptr);
    std::tm *now = std::localtime(&time);
    date << (now->tm_year + 1900) << sep << (now->tm_mon + 1) << sep
         << now->tm_mday;
    return date.str();
}
