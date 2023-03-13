#ifndef SUCKER_CHESS_UTILITIES_HPP
#define SUCKER_CHESS_UTILITIES_HPP

#include <array>   // for std::array
#include <cassert> // for assert
#include <cstddef> // for std::size_t
#include <ostream> // for std::ostream
#include <random>  // for std::mt19937, std::uniform_int_distribution
#include <string>  // for std::string
#include <utility> // for std::swap
#include <vector>  // for std::vector


template <typename T>
constexpr bool contains(const std::vector<T> &vec, const T &x) {
    for (const T &y : vec) {
        if (x == y) { return true; }
    }
    return false;
}


template <typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &vec) {
    os << '[';
    bool first = true;
    for (const T &obj : vec) {
        if (!first) {
            os << ", ";
        } else {
            first = false;
        }
        os << obj;
    }
    os << ']';
    return os;
}


template <typename T, std::size_t N>
const T &random_choice(std::mt19937 &rng, const std::array<T, N> &arr) {
    static_assert(N > 0);
    std::uniform_int_distribution<std::size_t> index_dist(0, N - 1);
    return arr[index_dist(rng)];
}


template <typename T>
const T &random_choice(std::mt19937 &rng, const std::vector<T> &vec) {
    assert(!vec.empty());
    std::uniform_int_distribution<typename std::vector<T>::size_type>
        index_dist(0, vec.size() - 1);
    return vec[index_dist(rng)];
}


template <typename T>
void random_insert(std::mt19937 &rng, std::vector<T> &vec, const T &val) {
    std::uniform_int_distribution<typename std::vector<T>::size_type>
        index_dist(0, vec.size());
    vec.insert(
        vec.begin() +
            static_cast<typename std::vector<T>::iterator::difference_type>(
                index_dist(rng)
            ),
        val
    );
}


template <typename T>
void random_delete(std::mt19937 &rng, std::vector<T> &vec) {
    if (vec.empty()) { return; }
    std::uniform_int_distribution<typename std::vector<T>::size_type>
        index_dist(0, vec.size() - 1);
    vec.erase(
        vec.begin() +
        static_cast<typename std::vector<T>::iterator::difference_type>(
            index_dist(rng)
        )
    );
}


template <typename T>
void random_replace(std::mt19937 &rng, std::vector<T> &vec, const T &val) {
    std::uniform_int_distribution<typename std::vector<T>::size_type>
        index_dist(0, vec.size() - 1);
    vec[index_dist(rng)] = val;
}


template <typename T>
void random_swap(std::mt19937 &rng, std::vector<T> &vec) {
    if (vec.size() <= 1) { return; }
    std::uniform_int_distribution<typename std::vector<T>::size_type>
        index_dist(0, vec.size() - 1);
    while (true) {
        const typename std::vector<T>::size_type i = index_dist(rng);
        const typename std::vector<T>::size_type j = index_dist(rng);
        if (i != j) {
            std::swap(vec[i], vec[j]);
            return;
        }
    }
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
