#!/usr/bin/env python3

import itertools
import os
import shutil
import subprocess

PERFT_SOURCE_FILES = [
    "src/ChessPiece.cpp",
    "src/ChessMove.cpp",
    "src/ChessBoard.cpp",
    "src/CastlingRights.cpp",
    "src/ChessPosition.cpp",
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

if os.path.isdir("bin"):
    shutil.rmtree("bin")

os.mkdir("bin")

for compiler_name, program, warning_flags, optimization_flags in COMPILERS:
    for i in range(len(FLAGS)):
        for selected in itertools.combinations(FLAGS, i):
            variant_name = compiler_name + "".join(
                suffix for flag, suffix in selected
            )
            program_name = "bin/SuckerChessPerft" + variant_name
            print("Building", program_name)
            command = [program, "-std=c++20"]
            command += warning_flags
            command += optimization_flags
            command += [flag for flag, suffix in selected]
            command += PERFT_SOURCE_FILES
            command += ["perft.cpp", "-o", program_name]
            subprocess.run(command)
            print("Running", program_name)
            subprocess.run([program_name])
