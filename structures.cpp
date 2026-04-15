#include "bits/stdc++.h"

struct Move {
    uint8_t from;
    uint8_t to;
    bool is_beating;
};

struct GameState {
    uint64_t pieces[2];
    bool turn; // 1 - white, 0 - black
};



