# CheckersAI
An AI implemented with Monte Carlo Tree Search to play checkers against a human 

The AI is implemented in the file path : CheckersAI/src/checkers-cpp/StudentAI.cpp

It is a C++ implementation of a CheckersAI to play checkers that I implemented with a partner. 

To compile the .cpp file
1. naviagte to 'CheckersAI/src/checkers-cpp' in a terminal
2. Run the 'make' command The makefile will compile the files.
3. In that terminal the AI can be manually played against by running the line
' ./main {col} {row} {p} m {start_player (0 or 1)} '
M = number of rows 2P + Q
N = number of columns N*P is Even
P = number of rows occupied by pieces in the initial state
Q = number of unoccupied rows that separate the two sides in the initial state. Q > 0



