cmake_minimum_required(VERSION 3.24)
project(SuckerChess)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

message(${CMAKE_CXX_COMPILER_ID})

if ((CMAKE_CXX_COMPILER_ID STREQUAL "Clang") OR (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang"))
    add_compile_options(-Weverything
            -Wno-c++98-compat -Wno-c++98-compat-pedantic
            -Wno-zero-as-null-pointer-constant
            -Wno-weak-vtables -Wno-padded)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wall -Wextra -pedantic)
endif ()

add_executable(SuckerChess main.cpp
        src/ChessPiece.hpp src/ChessPiece.cpp
        src/ChessMove.cpp src/ChessMove.hpp
        ChessPosition.cpp ChessPosition.hpp
        ChessEngine.cpp ChessEngine.hpp
        ChessGame.cpp ChessGame.hpp
        ChessPlayer.cpp ChessPlayer.hpp
        ChessTournament.cpp ChessTournament.hpp
        Utilities.cpp Utilities.hpp src/CastlingRights.cpp src/CastlingRights.hpp src/ChessBoard.cpp src/ChessBoard.hpp)
