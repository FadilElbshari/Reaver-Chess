#include "chess.h"


int Chess::negaMax(int depth, int alpha, int beta) {
    if (depth==0) return evaluate();

    int maxEval = -INF;

    Move moveBuffer[256];
    int moveCount = GenerateLegalMoves(moveBuffer);
    if (moveCount == 0) return isInCheck() ? -INF + depth : 0; // checkmate or stalemate
    
    for (int i=0; i<moveCount; i++) {
        Move move = moveBuffer[i];

        makeMove(move);
        int eval = -negaMax(depth - 1, -beta, -alpha);
        undoMove();

        if (eval >= beta)
            return beta;
        if (eval > maxEval)
            maxEval = eval;
        if (eval > alpha)
            alpha = eval;

    }
    
    return maxEval;
}