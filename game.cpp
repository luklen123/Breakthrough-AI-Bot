#include "bits/stdc++.h"
#include "structures.cpp"
using namespace std;

// 
// index 0 is bottom left corner
// index 63 is top right corner

// functions 

GameState parse_initial_board(vector<char> board);

void print_board(const GameState& state, Move last_move);





GameState make_move(const GameState& state, const Move& move ); // tutaj move nie jest constem wedlug chata


bool is_game_over(const GameState& state);
