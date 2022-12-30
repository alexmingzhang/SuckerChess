#ifndef SUCKER_CHESS_UTILITIES_HPP
#define SUCKER_CHESS_UTILITIES_HPP

#include <cassert>  // for assert
#include <iostream> // for std::cout, std::endl
#include <random>   // for std::mt19937, std::uniform_int_distribution
#include <string>   // for std::string
#include <vector>   // for std::vector


template <typename T>
void print(bool verbose, const T &obj) {
    if (verbose) { std::cout << obj; }
}


template <typename T>
void println(bool verbose, const T &obj) {
    if (verbose) { std::cout << obj << std::endl; }
}

template <typename T>
std::ostream &operator<<(std::ostream &out, const std::vector<T> &vec) {
    for (const T &obj : vec) { out << obj << ' '; }
    return out;
}


template <typename T>
const T &random_choice(std::mt19937 &rng, const std::vector<T> &vec) {
    assert(!vec.empty());
    std::uniform_int_distribution<typename std::vector<T>::size_type> dist(
        0, vec.size() - 1
    );
    return vec[dist(rng)];
}


template <typename T, typename F>
std::vector<T> maximal_elements(const std::vector<T> &vec, const F &f) {
    std::vector<T> result;
    if (vec.empty()) { return result; }
    using S = decltype(f(vec[0]));
    result.push_back(vec[0]);
    S best = f(vec[0]);
    for (typename std::vector<T>::size_type i = 1; i < vec.size(); ++i) {
        const S score = f(vec[i]);
        if (score > best) {
            result.clear();
            result.push_back(vec[i]);
            best = score;
        } else if (score == best) {
            result.push_back(vec[i]);
        }
    }
    assert(!result.empty());
    return result;
}


template <typename T, typename F>
std::vector<T> minimal_elements(const std::vector<T> &vec, const F &f) {
    std::vector<T> result;
    if (vec.empty()) { return result; }
    using S = decltype(f(vec[0]));
    result.push_back(vec[0]);
    S best = f(vec[0]);
    for (typename std::vector<T>::size_type i = 1; i < vec.size(); ++i) {
        const S score = f(vec[i]);
        if (score < best) {
            result.clear();
            result.push_back(vec[i]);
            best = score;
        } else if (score == best) {
            result.push_back(vec[i]);
        }
    }
    assert(!result.empty());
    return result;
}


void trim(std::string &str);


std::mt19937 properly_seeded_random_engine();


std::string get_ymd_date(char sep);


#endif // SUCKER_CHESS_UTILITIES_HPP
