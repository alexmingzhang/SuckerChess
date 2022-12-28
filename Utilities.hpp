#ifndef SUCKER_CHESS_UTILITIES_HPP
#define SUCKER_CHESS_UTILITIES_HPP

#include <iostream> // for std::cout, std::endl
#include <random>   // for std::mt19937, std::uniform_int_distribution
#include <string>   // for std::string
#include <vector>   // for std::vector


template <typename T>
void println(bool verbose, const T &obj) {
    if (verbose) { std::cout << obj << std::endl; }
}


template <typename T>
const T &random_choice(std::mt19937 &rng, const std::vector<T> &vec) {
    assert(vec.size() > 0);
    std::uniform_int_distribution<typename std::vector<T>::size_type> dist(
        0, vec.size() - 1
    );
    return vec[dist(rng)];
}


void trim(std::string &str);


std::mt19937 properly_seeded_random_engine();


std::string get_ymd_date(char sep);


#endif // SUCKER_CHESS_UTILITIES_HPP
