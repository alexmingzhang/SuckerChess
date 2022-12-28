#include "CastlingRights.hpp"

#include <stdexcept> // for std::invalid_argument


CastlingRights::CastlingRights(const std::string &fen_rights_str)
    : data(0) {
    if (fen_rights_str == "-") { return; }
    for (char c : fen_rights_str) {
        switch (c) {
            case 'K': data |= WHITE_SHORT; break;
            case 'Q': data |= WHITE_LONG; break;
            case 'k': data |= BLACK_SHORT; break;
            case 'q': data |= BLACK_LONG; break;
            default:
                throw std::invalid_argument(
                    "FEN castling rights string "
                    "contains invalid character " +
                    std::string{c}
                );
        }
    }
}


std::ostream &operator<<(std::ostream &os, CastlingRights rights) {
    bool found = false;
    if (rights.white_can_short_castle()) {
        os << 'K';
        found = true;
    }
    if (rights.white_can_long_castle()) {
        os << 'Q';
        found = true;
    }
    if (rights.black_can_short_castle()) {
        os << 'k';
        found = true;
    }
    if (rights.black_can_long_castle()) {
        os << 'q';
        found = true;
    }
    if (!found) { os << '-'; }
    return os;
}
