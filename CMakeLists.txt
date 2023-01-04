cmake_minimum_required(VERSION 3.24)
project(SuckerChess)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

if ((CMAKE_CXX_COMPILER_ID STREQUAL "Clang") OR (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang"))
    add_compile_options(-Weverything
            -Wno-c++98-compat -Wno-c++98-compat-pedantic
            -Wno-zero-as-null-pointer-constant
            -Wno-weak-vtables -Wno-padded)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wall -Wextra -pedantic)
endif ()

set(SuckerChessSourcesList
        "src/ChessPiece.cpp"
        "src/ChessMove.cpp"
        "src/ChessBoard.cpp"
        "src/CastlingRights.cpp"
        "src/ChessPosition.cpp"
        "src/Utilities.cpp"
        "src/ChessEngine.cpp"
        "src/Engine/Random.cpp"
        "src/ChessGame.cpp")

add_executable(SuckerChessMain ${SuckerChessSourcesList} "main.cpp")

add_executable(SuckerChessMainOptimized ${SuckerChessSourcesList} "main.cpp")

add_executable(SuckerChessPerft ${SuckerChessSourcesList} "perft.cpp")

add_executable(SuckerChessPerftOptimized ${SuckerChessSourcesList} "perft.cpp")

add_executable(SuckerChessBenchmark ${SuckerChessSourcesList} "benchmark.cpp")

add_executable(SuckerChessBenchmarkOptimized ${SuckerChessSourcesList} "benchmark.cpp")

target_compile_definitions(SuckerChessMainOptimized PRIVATE
        SUCKER_CHESS_USE_COMPRESSED_CHESS_PIECE
        SUCKER_CHESS_USE_COMPRESSED_CHESS_MOVE
        SUCKER_CHESS_TRACK_KING_LOCATIONS
        SUCKER_CHESS_USE_ZOBRIST_HASH)

target_compile_definitions(SuckerChessPerftOptimized PRIVATE
        SUCKER_CHESS_USE_COMPRESSED_CHESS_PIECE
        SUCKER_CHESS_USE_COMPRESSED_CHESS_MOVE
        SUCKER_CHESS_TRACK_KING_LOCATIONS
        SUCKER_CHESS_USE_ZOBRIST_HASH)

target_compile_definitions(SuckerChessBenchmarkOptimized PRIVATE
        SUCKER_CHESS_USE_COMPRESSED_CHESS_PIECE
        SUCKER_CHESS_USE_COMPRESSED_CHESS_MOVE
        SUCKER_CHESS_TRACK_KING_LOCATIONS
        SUCKER_CHESS_USE_ZOBRIST_HASH)
