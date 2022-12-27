GCC_WARNING_FLAGS="-Wall -Wextra -pedantic"

CLANG_WARNING_FLAGS="\
-Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic \
-Wno-poison-system-directories -Wno-weak-vtables -Wno-padded -Wno-conditional-uninitialized"

SOURCES="\
ChessPiece.cpp ChessMove.cpp ChessPosition.cpp \
Utilities.cpp ChessEngine.cpp ChessGame.cpp main.cpp"

GCC_OPT_FLAGS="-DNDEBUG -O3 -march=native -flto"

CLANG_OPT_FLAGS="-DNDEBUG -O3 -mcpu=apple-m1 -flto"

rm SuckerChess*

g++-12 -std=c++20 ${GCC_WARNING_FLAGS} ${GCC_OPT_FLAGS} -DSUCKER_CHESS_USE_DETERMINISTIC_SEED ${SOURCES} -o SuckerChessGCC
clang++ -std=c++20 ${CLANG_WARNING_FLAGS} ${CLANG_OPT_FLAGS} -DSUCKER_CHESS_USE_DETERMINISTIC_SEED ${SOURCES} -o SuckerChessClang
g++-12 -std=c++20 ${GCC_WARNING_FLAGS} ${GCC_OPT_FLAGS} -DSUCKER_CHESS_USE_DETERMINISTIC_SEED -DSUCKER_CHESS_USE_COMPRESSED_CHESS_PIECE ${SOURCES} -o SuckerChessGCCP
clang++ -std=c++20 ${CLANG_WARNING_FLAGS} ${CLANG_OPT_FLAGS} -DSUCKER_CHESS_USE_DETERMINISTIC_SEED -DSUCKER_CHESS_USE_COMPRESSED_CHESS_PIECE ${SOURCES} -o SuckerChessClangP
