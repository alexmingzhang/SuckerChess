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
        "src/UCI.cpp"
        "src/ChessGame.cpp"
        "src/ChessPlayer.cpp"
        "src/ChessTournament.cpp"
        "main.cpp")

add_executable(SuckerChess ${SuckerChessSourcesList})

add_executable(SuckerChessOptimized ${SuckerChessSourcesList})

target_compile_definitions(SuckerChessOptimized PRIVATE
        SUCKER_CHESS_USE_COMPRESSED_CHESS_PIECE
        SUCKER_CHESS_USE_COMPRESSED_CHESS_MOVE
        SUCKER_CHESS_TRACK_KING_LOCATIONS)
