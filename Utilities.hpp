#ifndef SUCKER_CHESS_UTILITIES_HPP
#define SUCKER_CHESS_UTILITIES_HPP

#include <iostream> // for std::cout
#include <random>   // for std::mt19937
#include <string>   // for std::string

template <typename T>
void print(bool verbose, const T &obj) {
    if (verbose) { std::cout << obj << std::endl; }
}

std::mt19937 properly_seeded_random_engine();

std::string get_YMD_date(char sep);


#endif // SUCKER_CHESS_UTILITIES_HPP
