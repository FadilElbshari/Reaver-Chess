#include "chess.h"

Eval Chess::evaluate() {
    Eval eval = 0;
    // Count Materials
    eval += countBits(bitboards[PAWN][WHITE])   * PAWN_VALUE;
    eval += countBits(bitboards[KNIGHT][WHITE]) * KNIGHT_VALUE;
    eval += countBits(bitboards[BISHOP][WHITE]) * BISHOP_VALUE;
    eval += countBits(bitboards[ROOK][WHITE])   * ROOK_VALUE;
    eval += countBits(bitboards[QUEEN][WHITE])  * QUEEN_VALUE;

    // Black material
    eval -= countBits(bitboards[PAWN][BLACK])   * PAWN_VALUE;
    eval -= countBits(bitboards[KNIGHT][BLACK]) * KNIGHT_VALUE;
    eval -= countBits(bitboards[BISHOP][BLACK]) * BISHOP_VALUE;
    eval -= countBits(bitboards[ROOK][BLACK])   * ROOK_VALUE;
    eval -= countBits(bitboards[QUEEN][BLACK])  * QUEEN_VALUE;

    
    return eval;
}