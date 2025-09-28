#include "chess.h"


Eval Chess::minimax(int depth, int alpha, int beta) {
    if (depth==0) return evaluate();

    Move moveBuffer[MAX_MOVES];
    int moveCount = GenerateLegalMoves(moveBuffer);
    if (moveCount == 0) return isInCheck(-1, true) ? (INF * (currentTurn ? -1 : 1)) : 0; // checkmate or stalemate

    if (currentTurn) {
        int maxEval = -INF;
        for (int i=0; i<moveCount; i++) {
            Move move = moveBuffer[i];

            makeMove(move);
            Eval eval = minimax(depth - 1, alpha, beta);
            undoMove();

            if (eval > maxEval) maxEval = eval;
            if (eval > alpha) alpha = eval;
            if (beta <= alpha) break;
        }
        return maxEval;

    } else {
        int minEval = INF;
        for (int i=0; i<moveCount; i++) {
            Move move = moveBuffer[i];

            makeMove(move);
            Eval eval = minimax(depth - 1, alpha, beta);
            undoMove();

            if (eval < minEval) minEval = eval;
            if (eval < beta) beta = eval;
            if (beta <= alpha) break;
        }
        return minEval;
    }
}