#include "bits/stdc++.h"
#include "structures.cpp"
using namespace std;

#define MAX_DEPTH 7
#define INFINITY_VAL 1000000000

// index 0 is bottom left corner
// index 63 is top right corner

int visited_nodes;

void print_board(const GameState& state, Move last_move){
    if(state.turn == 0){
        cout << "  a b c d e f g h\n";
        for(int i=56; i>=0; i-=8){
            cout << ((i / 8) + 1) << " ";
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
            cout << ((i / 8) + 1) << "\n";
        }
        cout << "  a b c d e f g h\n";
    } else {
        cout << "  h g f e d c b a\n";
        for(int i=0; i<=56; i+=8){
            cout << ((i / 8) + 1) << " ";
            for(int j=7; j>=0; j--){
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
            cout << ((i / 8) + 1) << "\n";
        }
        cout << "  h g f e d c b a\n";
    }
}

bool is_game_over(const GameState& state){

    uint64_t row_1 = 0x00000000000000FFULL;
    uint64_t row_8 = 0xFF00000000000000ULL;

    if(state.pieces[0] & row_8) return true;
    if(state.pieces[1] & row_1) return true;

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

int material_eval(const GameState& state){
    return __builtin_popcountll(state.pieces[0]) - __builtin_popcountll(state.pieces[1]);
}

int progression_eval(const GameState& state){
    static constexpr uint64_t rows[] = {0x00000000000000FFULL, 0x000000000000FF00ULL, 0x0000000000FF0000ULL, 0x00000000FF000000ULL,
                       0x000000FF00000000ULL, 0x0000FF0000000000ULL, 0x00FF000000000000ULL, 0xFF00000000000000ULL};

    static constexpr int rows_w[] = {0, 0, 1, 2,
                      5, 10, 20, 10000};

    int white_score = 0;
    int black_score = 0;

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

int safety_eval(const GameState& state){
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

int evaluate(const GameState& state, const int& ai_player){
    int w1 = 10;
    int w2 = 60;
    int w3 = 30;

    int total_pieces = __builtin_popcountll(state.pieces[0]) + __builtin_popcountll(state.pieces[1]);
    if(total_pieces <= 12){
        w1 = 5;
        w2 = 85;
        w3 = 10;
    }

    int f1 = material_eval(state);
    int f2 = progression_eval(state);
    int f3 = safety_eval(state);

    int final_result = (w1 * f1) + (w2 * f2) + (w3 * f3);

    // final_result > 0 white is winning
    // final_result = 0 draw
    // final_result < 0 black is winning

    if(ai_player == 0){
        return final_result;
    } else {
        return -final_result;
    }
}

int minimax(const GameState& state, int depth, int ai_player, bool is_maximizing){
    visited_nodes ++;

    if(is_game_over(state)){
        if(is_maximizing) return -INFINITY_VAL;
        return INFINITY_VAL;
    }

    if(depth == MAX_DEPTH) return evaluate(state, ai_player);

    vector<Move> possible_move = generate_possible_moves(state);
    int temp = 0;
    if(is_maximizing){
        int max_score = -INFINITY_VAL;
        for(auto& move : possible_move){
            temp = minimax(make_move(state, move), depth + 1, ai_player, !is_maximizing);
            max_score = max(max_score, temp);
        }
        return max_score;

    } else {
        int min_score = INFINITY_VAL;
        for(auto& move : possible_move){
            temp = minimax(make_move(state, move), depth + 1, ai_player, !is_maximizing);
            min_score = min(min_score, temp);
        }
        return min_score;
    }
}

int alpha_beta_pruning(const GameState& state, int depth, int ai_player, bool is_maximizing, int alpha, int beta){
    visited_nodes ++;

    if(is_game_over(state)){
        if(is_maximizing) return -INFINITY_VAL;
        return INFINITY_VAL;
    }

    if(depth == MAX_DEPTH) return evaluate(state, ai_player);

    vector<Move> possible_moves = generate_possible_moves(state);

    if(is_maximizing){
        int max_score = -INFINITY_VAL;
        for(auto& move : possible_moves){
            int temp = alpha_beta_pruning(make_move(state, move), depth + 1, ai_player, !is_maximizing, alpha, beta);
            max_score = max(max_score, temp);
            alpha = max(alpha, temp);
            if(beta <= alpha) break;
        }  
        return max_score; 

    } else {
        int min_score = INFINITY_VAL;
        for(auto& move : possible_moves){
            int temp = alpha_beta_pruning(make_move(state, move), depth + 1, ai_player, !is_maximizing, alpha, beta);
            min_score = min(min_score, temp);
            beta = min(beta, temp);
            if(beta <= alpha) break;
        }  
        return min_score; 
    }
}

Move get_best_move(const GameState& state, int ai_player){
    auto start_time = chrono::high_resolution_clock::now();

    int best_score = -INFINITY_VAL;
    int best_score_idx = 0;
    visited_nodes = 0;

    vector<Move> possible_moves = generate_possible_moves(state);

    if(possible_moves.empty()) return Move();

    for(int i=0; i<possible_moves.size(); i++){
        GameState next_state = make_move(state, possible_moves[i]);
        int score = alpha_beta_pruning(next_state, 1, ai_player, false, best_score, INFINITY_VAL);
        //int score = minimax(next_state, 1, ai_player, false);
        if(best_score < score){
            best_score = score;
            best_score_idx = i;
        }
    }

    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double, std::milli> duration = end_time - start_time;

    cerr << "--------------------------------------\n";
    cerr << "AI info:\n";
    cerr << "Odwiedzone wezly: " << visited_nodes << "\n";
    cerr << "Czas: " << duration.count() << " ms\n";
    cerr << "--------------------------------------\n";

    return possible_moves[best_score_idx];
}

GameState init_board(){
    GameState state;
    state.pieces[0] = 0x000000000000FFFFULL;
    state.pieces[1] = 0xFFFF000000000000ULL;
    state.turn = 0;

    return state;
}

int decode(string idx){
    if(idx.size() != 2 || !('a' <= idx[0] && idx[0] <= 'h') || !('1' <= idx[1] && idx[1] <= '8')) return 255;
    return ((idx[0] - 'a') + ((idx[1] - '1') * 8));
}

string encode(int pos){
    char n = ((pos / 8) + '1');
    char l = ((pos % 8) + 'a');
    return string(1, l) + string(1, n);
}

bool validate_move(const GameState& state, const Move& move){
    if(move.from == 255 || move.to == 255) return false;

    if((state.pieces[state.turn] & (1ULL << move.from)) == 0) return false;

    vector<Move> possible_moves = generate_possible_moves(state);
    for(const auto& possible_move : possible_moves){
        if(possible_move.from == move.from && possible_move.to == move.to) return true;
    }

    return false;
}

Move get_player_move(GameState state, Move last_move){
    Move move;
    string idx_from;
    string idx_to;

    cout << "Game state:\n\n";
    print_board(state, last_move);

    bool valid_move = false;
    while(!valid_move){
        cout << "\nPlease provide your move.\n";
        cout << "From (enter current position of Your piece ex. c2): ";
        cin >> idx_from;
        cout << "To (enter desired position ex. h7): ";
        cin >> idx_to;
        move.from = decode(idx_from);
        move.to = decode(idx_to);
        valid_move = validate_move(state, move);
        if(!valid_move) cout << "Entered move is invalid!!!\n";
    }

    return move;
}

void game(int ai_player){
    GameState state = init_board();
    Move last_move;
    Move selected_move;

    while(!is_game_over(state)){
        if(state.turn == ai_player || ai_player == 2){
            int current_ai = (ai_player == 2) ? state.turn : ai_player;
            selected_move = get_best_move(state, current_ai);
        } else {
            selected_move = get_player_move(state, last_move);
        }
        state = make_move(state, selected_move);
        last_move = selected_move;
        if(ai_player == 2 && state.turn == 1) print_board(state, last_move);
    }

    cout << "\n==============================\n";
    cout << "        GAME FINISHED!        \n";
    cout << "==============================\n\n";
    print_board(state, last_move);

    cout << "\n" << (state.turn == 0 ? "BLACKS wins!!!" : "WHITES wins!!!") << "\n";
}

