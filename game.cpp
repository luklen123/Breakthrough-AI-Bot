#include "bits/stdc++.h"
#include "functions.cpp"
using namespace std;

void start_interactive_mode(){
    cout << "-----------------------------------\n";
    cout << "Hi, welcome in Breakthrough game!\n";
    cout << "-----------------------------------\n\n";
    cout << "Choose Your side!\nEnter W (for whites), B (for blacks), N (for none, game between bots): ";

    char player_side;
    cin >> player_side;

    switch (player_side){
        case 'W':
            game(1);
            break;
        case 'B':
            game(0);
            break;
        case 'N':
            game(2);
            break;
        default:
            cout << "Entered invalid value :(";
    }    
}


void start_bot_mode(int ai_player){
    GameState state = init_board();

    while(!is_game_over(state)){
        if(state.turn == ai_player){
            // this program turn
            Move selected_move = get_best_move(state, ai_player);

            cout << encode(selected_move.from) << " " << encode(selected_move.to) << "\n";
            cout.flush();

            state = make_move(state, selected_move);
        } else {
            // oponent's program turn
            string idx_from, idx_to;
            if(!(cin >> idx_from >> idx_to)) break;

            Move enemy_move;
            enemy_move.from = decode(idx_from);
            enemy_move.to = decode(idx_to);

            state = make_move(state, enemy_move);
        }
    }
}

