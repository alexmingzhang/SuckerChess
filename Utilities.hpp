#ifndef SUCKER_CHESS_UTILITIES_HPP
#define SUCKER_CHESS_UTILITIES_HPP

#include <random> // for std::mt19937
#include <string> // for std::string


std::mt19937 properly_seeded_random_engine();

std::string get_YMD_date(char sep);


#endif // SUCKER_CHESS_UTILITIES_HPP
