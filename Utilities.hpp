#ifndef SUCKER_CHESS_UTILITIES_HPP
#define SUCKER_CHESS_UTILITIES_HPP

#include <iostream> // for std::cout, std::endl
#include <random>   // for std::mt19937
#include <string>   // for std::string


template <typename T>
void println(bool verbose, const T &obj) {
    if (verbose) { std::cout << obj << std::endl; }
}

void trim(std::string &str);


std::mt19937 properly_seeded_random_engine();


std::string get_ymd_date(char sep);


#endif // SUCKER_CHESS_UTILITIES_HPP
