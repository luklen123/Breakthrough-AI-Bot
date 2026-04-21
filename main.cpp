#include "bits/stdc++.h"
#include "game.cpp"
using namespace std;

int main(int argc, char* argv[]){
    if(argc < 2){
        start_interactive_mode();
        return 0;
    }

    string mode = argv[1];
    if(mode == "bot" && argc == 3){
        string side = argv[2];
        int ai_player = (side == "W") ? 0 : 1;
        start_bot_mode(ai_player);
    } else {
        cout << "Nieznane argumenty. Wpisz: ./program bot [W/B]\n";
    }
    return 0;
}