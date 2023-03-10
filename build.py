#!/usr/bin/env python3

import itertools
import os
import shutil
import subprocess


def build_and_run(compiler, flags, source_files, program_name):
    print("Building", program_name)
    command = [compiler, "-std=c++20"]
    command.extend(flags)
    command.extend(source_files)
    command.extend(["-o", program_name])
    subprocess.run(command)
    print("Running", program_name)
    subprocess.run([program_name])


PERFT_SOURCE_FILES = [
    "src/ChessPiece.cpp",
    "src/ChessMove.cpp",
    "src/ChessBoard.cpp",
    "src/CastlingRights.cpp",
    "src/ChessPosition.cpp",
    "perft.cpp",
]

BENCHMARK_SOURCE_FILES = [
    "src/ChessPiece.cpp",
    "src/ChessMove.cpp",
    "src/ChessBoard.cpp",
    "src/CastlingRights.cpp",
    "src/ChessPosition.cpp",
    "src/ChessEngine.cpp",
    "src/ChessGame.cpp",
    "src/Utilities.cpp",
    "src/Engine/Random.cpp",
    "benchmark.cpp",
]

COMPILERS = [
    (
        "GCC",
        "g++-12",
        ["-Wall", "-Wextra", "-pedantic"],
        ["-DNDEBUG", "-O3", "-march=native", "-flto=auto"]
    ),
    (
        "Clang",
        "clang++",
        [
            "-Weverything",
            "-Wno-c++98-compat",
            "-Wno-c++98-compat-pedantic",
            "-Wno-zero-as-null-pointer-constant",
            "-Wno-poison-system-directories",
            "-Wno-weak-vtables",
            "-Wno-padded",
        ],
        ["-DNDEBUG", "-O3", "-mcpu=apple-m1", "-flto"]
    ),
]

FLAGS = [
    ("-DSUCKER_CHESS_USE_DETERMINISTIC_SEED", 'D'),
    ("-DSUCKER_CHESS_USE_COMPRESSED_CHESS_PIECE", 'P'),
    ("-DSUCKER_CHESS_USE_COMPRESSED_CHESS_MOVE", 'M'),
    ("-DSUCKER_CHESS_USE_COMPRESSED_CHESS_BOARD", 'B'),
    ("-DSUCKER_CHESS_TRACK_KING_LOCATIONS", 'K'),
]


def main():
    if os.path.isdir("bin"):
        shutil.rmtree("bin")

    os.mkdir("bin")

    for compiler_name, program, warn_flags, opt_flags in COMPILERS:
        for i in range(len(FLAGS)):
            for selected in itertools.combinations(FLAGS, i):

                variant_name = compiler_name + "".join(
                    suffix for flag, suffix in selected
                )
                flags = ["-std=c++20"] + warn_flags + opt_flags
                flags.extend(flag for flag, suffix in selected)

                build_and_run(program, flags, PERFT_SOURCE_FILES,
                              "bin/SuckerChessPerft" + variant_name)

                if ("-DSUCKER_CHESS_USE_DETERMINISTIC_SEED", 'D') in selected:
                    build_and_run(program, flags, BENCHMARK_SOURCE_FILES,
                                  "bin/SuckerChessBenchmark" + variant_name)


if __name__ == "__main__":
    main()
