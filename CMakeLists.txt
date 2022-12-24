cmake_minimum_required(VERSION 3.24)
project(SuckerChess)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -pedantic)

add_executable(SuckerChess main.cpp ChessPiece.hpp ChessPiece.cpp)
