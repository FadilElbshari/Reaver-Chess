#include "chess.h"

Bitboard Chess::perft(int depth, int* mates, int originalDepth) {
    
    if (originalDepth == -1) originalDepth = depth;
    if (depth == 0) return 1;

    Bitboard nodes = 0;
    Move moveBuffer[256];

    int moveCount = GenerateMoves(moveBuffer);
    
    for (int i=0; i<moveCount; i++) {
        Move move = moveBuffer[i];
        makeMove(move);
        
        bool isLegal = !isInCheck();
        if (isLegal) {
            Bitboard inc = perft(depth - 1, mates, originalDepth);;
            nodes += inc;

            if (depth == originalDepth) std::cout << notationFromSquare(move) << ": " << inc << " - " << *mates<< "\n"; 

        }
        // if (isCheckMate()) {
        //     (*mates)++;
        //     displayBoard();
        // }
        
        undoMove();
    }

    return nodes;
}