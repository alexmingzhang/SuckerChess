#ifndef SUCKER_CHESS_CHESS_MOVE_HPP
#define SUCKER_CHESS_CHESS_MOVE_HPP

#include <cstdint> // for std::int8_t
#include <ostream> // for std::ostream

#include "ChessPiece.hpp"


using coord_t = std::int8_t;


struct ChessMove {

    coord_t src_file;
    coord_t src_rank;
    coord_t dst_file;
    coord_t dst_rank;
    PieceType promotion_type;

}; // struct ChessMove


std::ostream &operator<<(std::ostream &os, const ChessMove &move);


#endif // SUCKER_CHESS_CHESS_MOVE_HPP
