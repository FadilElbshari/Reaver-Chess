#include <chess.h>
#include <iostream>
#include <cctype>
#include <algorithm>

void print(auto value, int breakln = 1, bool padding = 0) {
    std::cout << value << (breakln ? "\n" : "");
    if (padding) std::cout << "\n";
}

void displayWelcomeMessage() {
    print("");
    print("---------------------Welcome to Reaver Chess engine---------------------", 1, 1);
    print("[1]     Enter 'init' to initialise a board with starting position.");
    print("[2]     Enter 'custom' to start a board with a custom position (FEN).");
    print("[3]     Enter 'clear' to clear the board and start over.");
    print("[4]     Enter 'move' followed by a move in chess notation to attempt making a move (e2e4).");
    print("[5]     Enter 'undo' to undo a previously made move.");
    print("[6]     Enter 'board' to display the current board.");
    print("[7]     Enter 'evaluate' followed by a depth value to calculate the evaluation of a given position.");
    print("[8]     Enter 'perft' followed by a depth value to run a timed performance test.");
    print("[9]     Enter 'over' to check if checkmate has occured.");
    print("[10]    Enter 'fen' to display the current fen.");
    print("[11]    Enter 'quit' to quit the program.", 1, 1);

}

void runInterface() {
    std::string initFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    bool initialised = false;

    displayWelcomeMessage();
    print("Enter: ", 0);

    //Chess Board;
    Chess* Board = new Chess();

    std::string input = "";
    while (true) {
        print("Enter: ", 0);
        getline(std::cin, input);

        if (input == "quit" || input == "-1") break;

        if (input=="1" || input == "init") {
            if (initialised) {
                print("Board is already initialised.", 1, 1);
                continue;
            }
            Board->init(initFen);
            initialised = true;
        } else if (input == "2" || input == "custom") {
            if (initialised) {
                print("Board is already initialised.", 1, 1);
                continue;
            }
            std::string customFen = "";
            print("Please paste the FEN here: ", 0);
            getline(std::cin, customFen);
            try
            {
                Board->init(customFen.substr(0));
                initialised = true;
            }
            catch(const std::exception& e)
            {
                print("Please enter a valid FEN.", 1, 1);
            }
            
        } else if (input == "3" || input == "clear") {
            delete Board;
            Board = new Chess();
            initialised = false;
        } else if (input.size() == 9 && input.substr(0, 5) == "move ") {
            
            if (!initialised) {
                print("Please initialise the board first.", 1, 1);
                continue;
            }
                std::string move = input.substr(5);
                if(move.size() == 4) {
                    print("Making move: ", 0);
                    print(move, 0);
                    print("........", 1, 1);
                    Board->move(move);
                } else {
                    print("Invalid move format", 1, 1);
                }
                

            } else if ((input.size()==4 && input.substr(0) == "undo") || input == "5") {
            if (!initialised) {
                print("Please initialise the board first.", 1, 1);
                continue;
            }
            if (Board->stackPointer==0) {
                print("Please make a move first.", 1, 1);
                continue;
            }

            Board->undoMove();
            print("Move Undone.", 1, 1);

        } else if (input.substr(0, 9) == "evaluate ") {
            if (!initialised) {
                print("Please initialise the board first.", 1, 1);
                continue;
            }
            std::string depthStr = input.substr(9);
            bool valid = !depthStr.empty() && std::all_of(depthStr.begin(), depthStr.end(), ::isdigit);
            if (valid) {
                int depth = std::stoi(depthStr);
                print("You entered evaluate depth: ", 0);
                print(depth);

                print(Board->negaMax(depth, -INF, INF) * (Board->currentTurn ? -1 : 1));

            } else {
                print("Invalid depth. Please provide a number after 'evaluate '.", 1, 1);
            }

            
         } else if (input.substr(0, 6) == "perft ") {
            if (!initialised) {
                print("Please initialise the board first.", 1, 1);
                continue;
            }
            std::string depthStr = input.substr(6);

            bool valid = !depthStr.empty() && std::all_of(depthStr.begin(), depthStr.end(), ::isdigit);
            if (valid) {
                int depth = std::stoi(depthStr);
                print("You entered perft depth: ", 0);
                print(depth);

                using std::chrono::high_resolution_clock;
                using std::chrono::duration_cast;
                using std::chrono::duration;
                using std::chrono::milliseconds;

                int mates = 0;
                auto t1 = high_resolution_clock::now();
                print(Board->perft(depth, &mates));
                auto t2 = high_resolution_clock::now();
                duration<double, std::milli> ms_double = t2 - t1;
                print(ms_double.count(), 0);
                print("ms", 1, 1);

                // Call your perft function here with 'depth'
            } else {
                print("Invalid depth. Please provide a number after 'perft '.", 1, 1);
            }
        } else if (input == "board" || input == "6") {
            if (!initialised) {
                print("Please initialise the board first.", 1, 1);
                continue;
            }

            Board->displayBoard();

        } else if (input == "over" || input == "9") {
            if (!initialised) {
                print("Please initialise the board first.", 1, 1);
                continue;
            }
            U8 isOver = Board->isGameOver();
            if (isOver > Board->NOT_OVER) {
                print(isOver==1 ? "Check Mate" : "Stale Mate", 0);
                print(" for ", 0);
                print(Board->currentTurn ? "Black!" : "White!", 1, 1);
                continue;
            }
            print("Game is not over.");
        } else if (input == "fen" || input == "10") {
            if (!initialised) {
                print("Please initialise the board first.", 1, 1);
                continue;
            }
            std::string fen = Board->getFen();
            print(fen, 1, 1);

        } else {
            print("Please enter a valid input.", 1, 1);
        }
    }
}

int main() {
    runInterface();
    return 0;
}