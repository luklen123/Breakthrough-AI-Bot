#include "bits/stdc++.h"
#include "structures.cpp"
using namespace std;

// index 0 is bottom left corner
// index 63 is top right corner

GameState parse_initial_board(vector<char> board);

void print_board(const GameState& state, Move last_move){
    for(int i=56; i>=0; i-=8){
        for(int j=0; j<8; j++){

            if((i + j) == last_move.from){
                cout << "o";
            } else if(state.pieces[0] & (1ULL << (i+j))){
                cout << "W";
            } else if(state.pieces[1] & (1ULL << (i+j))){
                cout << "B";
            } else {
                cout << "_";
            }

            cout << " ";
        }
        cout << "\n";
    }
}

bool is_game_over(const GameState& state){

    uint64_t row_1 = 0x00000000000000FFULL;
    uint64_t row_8 = 0xFF00000000000000ULL;

    if(state.pieces[0] & row_1) return true;
    if(state.pieces[1] & row_8) return true;

    if(state.pieces[0] == 0 || state.pieces[1] == 0) return true;

    return false;
}

void extract_positions(uint64_t move_board, int back_shift, vector<Move>& results){
    while(move_board){
        int to = __builtin_ctzll(move_board);

        Move move;
        move.to = to;
        move.from = to + back_shift;
        results.push_back(move);

        move_board &= move_board - 1;
    }
}

vector<Move> generate_possible_moves(const GameState& state){
    uint64_t not_l_edge = 0xFEFEFEFEFEFEFEFEULL;
    uint64_t not_r_edge = 0x7F7F7F7F7F7F7F7FULL;

    // 0 - white, 1 - black
    int player = state.turn;
    int enemy = (state.turn + 1) % 2;
    int mult = (player == 1 ? 1 : -1);

    uint64_t possible_forward_pos = ~(state.pieces[player] | state.pieces[enemy]);
    uint64_t possible_diagonal_pos = ~state.pieces[player];

    vector<Move> possible_moves = {};
    possible_moves.reserve(40);

    // 0 - white, 1 - black
    if(!player){

        // forward
        uint64_t forward = (state.pieces[player] << 8) & possible_forward_pos;

        // diagonal L
        uint64_t diagonal_l = ((state.pieces[player] & not_l_edge) << 7) & possible_diagonal_pos;

        // diagonal R
        uint64_t diagonal_r = ((state.pieces[player] & not_r_edge) << 9) & possible_diagonal_pos;

        extract_positions(forward, -8, possible_moves);
        extract_positions(diagonal_l, -7, possible_moves);
        extract_positions(diagonal_r, -9, possible_moves);

    } else {

        // forward
        uint64_t forward = (state.pieces[player] >> 8) & possible_forward_pos;

        // diagonal L
        uint64_t diagonal_l = ((state.pieces[player] & not_l_edge) >> 9) & possible_diagonal_pos;

        // diagonal R
        uint64_t diagonal_r = ((state.pieces[player] & not_r_edge) >> 7) & possible_diagonal_pos;

        extract_positions(forward, 8, possible_moves);
        extract_positions(diagonal_l, 9, possible_moves);
        extract_positions(diagonal_r, 7, possible_moves);
    }

    return possible_moves;
}

GameState make_move(const GameState& state, Move& move){
    // 0 - white, 1 - black 
    GameState new_state = state;
    int player = state.turn;
    int enemy = (player + 1) % 2;

    if(state.pieces[enemy] & (1ULL << move.to)){
        move.is_beating = true;
        new_state.pieces[enemy] ^= (1ULL << move.to);

    } else move.is_beating = false;

    new_state.pieces[player] ^= (1ULL << move.from);
    new_state.pieces[player] ^= (1ULL << move.to);

    new_state.turn = enemy;

    return new_state;
}

float evaluate(const GameState& state){
    float w1 = 0.33;
    float w2 = 0.33;
    float w3 = 0.33;

    float f1 = material_eval(state);
    float f2 = progression_eval(state);
    float f3 = safety_eval(state);

    float final_result = (w1 * f1) + (w2 * f2) + (w3 * f3);

    // final_result > 0 white is winning
    // final_result = 0 draw
    // final_result < 0 black is winning

    return final_result; 
}

float material_eval(const GameState& state){
    return __builtin_popcountll(state.pieces[0]) - __builtin_popcountll(state.pieces[1]);
}

float progression_eval(const GameState& state){
    uint64_t rows[] = {0x00000000000000FFULL, 0x000000000000FF00ULL, 0x0000000000FF0000ULL, 0x00000000FF000000ULL,
                       0x000000FF00000000ULL, 0x0000FF0000000000ULL, 0x00FF000000000000ULL, 0xFF00000000000000ULL};

    float rows_w[] = {0, 0, 1, 2.0,
                      5.0, 10.0, 20.0, 100.0};

    float white_score = 0.0;
    float black_score = 0.0;

    for(int i=0; i<8; i++){
        // WHITE
        uint64_t temp = state.pieces[0] & rows[i];
        white_score += __builtin_popcountll(temp) * rows_w[i];

        // BLACK
        temp = state.pieces[1] & rows[i];
        black_score += __builtin_popcountll(temp) * rows_w[7-i];
    }

    return white_score - black_score;
}

float safety_eval(const GameState& state){
    uint64_t not_l_edge = 0xFEFEFEFEFEFEFEFEULL;
    uint64_t not_r_edge = 0x7F7F7F7F7F7F7F7FULL;

    int white_score = 0;
    int black_score = 0;

    // WHITE 
    // diagonal_l
    uint64_t shifted = (state.pieces[0] & not_l_edge) << 7;
    shifted &= state.pieces[0];
    white_score += __builtin_popcountll(shifted);

    // diagonal_r
    shifted = (state.pieces[0] & not_r_edge) << 9;
    shifted &= state.pieces[0];
    white_score += __builtin_popcountll(shifted);

    // BLACK 
    // diagonal_l
    shifted = (state.pieces[1] & not_l_edge) >> 9;
    shifted &= state.pieces[1];
    black_score += __builtin_popcountll(shifted);

    // diagonal_r
    shifted = (state.pieces[1] & not_r_edge) >> 7;
    shifted &= state.pieces[1];
    black_score += __builtin_popcountll(shifted);

    return white_score - black_score;
}