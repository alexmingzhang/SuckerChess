# SuckerChess
Chess engines that suck, inspired by Tom7's [Elo World](https://www.youtube.com/watch?v=DpXy041BIlA)

Take an Elo World engine like CCCP. It prioritizes four preferences (in order): checkmate, check, capture, push. What if instead, we wanted to capture first and then check? Or even push before we captured? SuckerChess aims to solve this problem by providing modular chess engines, built on chains of preferences.

## Installation
    git clone https://github.com/alexmingzhang/SuckerChess.git
    mkdir SuckerChess/build
    cd SuckerChess/build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make
    
## Usage
To play against an engine, run `SuckerChessMainOptimized`. Input move names like `e4` to play against an engine.
```
    a   b   c   d   e   f   g   h
  ┌───┬───┬───┬───┬───┬───┬───┬───┐
8 │ ♖ │ ♘ │ ♗ │ ♕ │ ♔ │ ♗ │ ♘ │ ♖ │
  ├───┼───┼───┼───┼───┼───┼───┼───┤
7 │ ♙ │ ♙ │ ♙ │ ♙ │ ♙ │ ♙ │ ♙ │ ♙ │
  ├───┼───┼───┼───┼───┼───┼───┼───┤
6 │   │   │   │   │   │   │   │   │
  ├───┼───┼───┼───┼───┼───┼───┼───┤
5 │   │   │   │   │   │   │   │   │
  ├───┼───┼───┼───┼───┼───┼───┼───┤
4 │   │   │   │   │   │   │   │   │
  ├───┼───┼───┼───┼───┼───┼───┼───┤
3 │   │   │   │   │   │   │   │   │
  ├───┼───┼───┼───┼───┼───┼───┼───┤
2 │ ♟ │ ♟ │ ♟ │ ♟ │ ♟ │ ♟ │ ♟ │ ♟ │
  ├───┼───┼───┼───┼───┼───┼───┼───┤
1 │ ♜ │ ♞ │ ♝ │ ♛ │ ♚ │ ♝ │ ♞ │ ♜ │
  └───┴───┴───┴───┴───┴───┴───┴───┘
rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -
White to move.
> 
```

To run an evolution simulation, run `SuckerChessEvolutionOptimized`. This pits a bunch of engines against eachother, where losers die and winners create offspring with mutated preference chains. Over time, these will create optimal preference chains.
```Round 6
      Organism Wins   Draws  Losses W/L
 0.  SCpPM1Ma1 69     109    12     5.75
 1.     SCpMa1 139    209    32     4.34
 2.  ExpPDrSCp 152    383    35     4.34
 3.  SCpPDrExp 42     138    10     4.20
 4.     PM1SCp 67     487    16     4.19
 5.     ExpSCp 183    717    50     3.66
 6.     SCpPM1 40     329    11     3.64
 7.        SCp 187    893    60     3.12
 8.        SCp 141    762    47     3.00
 9.        SCp 186    890    64     2.91
10.  ExpPM1SCp 22     160    8      2.75
11.        Red 238    807    95     2.51
12.     SCpCow 22     158    10     2.20
13.     SCpExp 48     309    23     2.09
14.        SCp 18     152    20     0.90
15.     CapRed 20     143    27     0.74
16.     SCpHud 2      181    7      0.29
17.        PDr 9      118    63     0.14
18.  GenSCpMa1 6      88     96     0.06
19.            3      124    63     0.05
```
