# Breakthrough AI Engine ♟️🤖

A high-performance Artificial Intelligence engine for the **Breakthrough** board game, written in C++. The engine utilizes the **Minimax algorithm with Alpha-Beta pruning** and an advanced board representation based on **Bitboards**, allowing for extreme computational efficiency and deep game tree analysis.

## 🚀 Main Features
* **Bitboards:** The game state is represented by two 64-bit unsigned integers (`uint64_t`). This allows for ultra-fast move generation and state validation using direct bitwise CPU operations (shifts, edge masks, popcount).
* **Minimax & Alpha-Beta Pruning:** Implementation of the game tree search algorithm optimized with Alpha-Beta pruning. This drastically reduces the number of evaluated nodes and allows for deeper analysis within a given time limit.
* **Complex Heuristics:** The evaluation function (`evaluate`) is based on three strategic pillars:
    1.  **Material:** Numerical advantage of pieces on the board.
    2.  **Progression:** Promoting pieces closer to the opponent's edge (using exponential weights).
    3.  **Safety:** Bonus for creating chains of mutually supporting pieces diagonally.
* **Tournament Mode (IPC):** Standard input/output handling enabling seamless communication with external referee scripts or playing matches between two independent automated processes.

## 📂 File Structure
The project is modularized following Clean Code C++ practices:
* `structures.cpp` – Data structure definitions (`Move`, `GameState`).
* `Game.cpp` – Game logic and board handling.
* `functions.cpp` – The decision-making "brain", heuristics, and the Alpha-Beta algorithm.
* `main.cpp` – Entry point, managing execution modes (interactive vs. bot).
* `judge.py` – A Python referee script for conducting automated Bot vs. Bot tournaments.

## ⚙️ Compilation and Execution

### Requirements:
* C++ Compiler supporting at least C++11 (e.g., `g++` or `clang`).
* Python 3.x (optional, only required to run the automated Referee).

### Compilation:
For the best AI performance, compiling with the **`-O3` optimization flag is highly recommended**:
```bash
g++ -O3 main.cpp Game.cpp AI.cpp -o breakthrough