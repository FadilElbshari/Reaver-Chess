#include "chess.h"

void Chess::initAttackTables() {
    for (int sq = 0; sq < 64; ++sq) {
        int rank = sq / 8;
        int file = sq % 8;

        // Pawn Attacks
        // White
        if (file > 0 && rank < 7) pawnAttacks[WHITE][sq] |= getWithSetBit(sq + 7);
        if (file < 7 && rank < 7) pawnAttacks[WHITE][sq] |= getWithSetBit(sq + 9);
        // Black
        if (file > 0 && rank > 0) pawnAttacks[BLACK][sq] |= getWithSetBit(sq - 9);
        if (file < 7 && rank > 0) pawnAttacks[BLACK][sq] |= getWithSetBit(sq - 7);

        // Knight Attacks
        Bitboard knight = 0ULL;
        int knightHops[8][2] = {
            { 2, 1}, { 1, 2}, {-1, 2}, {-2, 1},
            {-2,-1}, {-1,-2}, { 1,-2}, { 2,-1}
        };
        for (auto& m : knightHops) {
            int r = rank + m[0];
            int f = file + m[1];
            if (r >= 0 && r < 8 && f >= 0 && f < 8)
                knight |= getWithSetBit(r * 8 + f);
        }
        knightAttacks[sq] = knight;

        // King Attacks
        Bitboard king = 0ULL;
        for (int dr = -1; dr <= 1; ++dr) {
            for (int df = -1; df <= 1; ++df) {
                if (dr == 0 && df == 0) continue;
                int r = rank + dr;
                int f = file + df;
                if (r >= 0 && r < 8 && f >= 0 && f < 8)
                    king |= getWithSetBit(r * 8 + f);
            }
        }
        kingAttacks[sq] = king;


        // Bishop Attacks

        // top-right, top-left, bottom-left, bottom-right
        for (int f=file+1, r=rank+1; f<7 && r<7; f++, r++) bishopMasks[sq] |= getWithSetBit(r*8 + f);
        for (int f=file-1, r=rank+1; f>0 && r<7; f--, r++) bishopMasks[sq] |= getWithSetBit(r*8 + f);
        for (int f=file-1, r=rank-1; f>0 && r>0; f--, r--) bishopMasks[sq] |= getWithSetBit(r*8 + f);
        for (int f=file+1, r=rank-1; f<7 && r>0; f++, r--) bishopMasks[sq] |= getWithSetBit(r*8 + f);

        // Rook Attacks

        // up, left, down, right
        for (int f=file, r=rank+1; r<7; r++) rookMasks[sq] |= getWithSetBit(r*8 + f);
        for (int f=file-1, r=rank; f>0; f--) rookMasks[sq] |= getWithSetBit(r*8 + f);
        for (int f=file, r=rank-1; r>0; r--) rookMasks[sq] |= getWithSetBit(r*8 + f);
        for (int f=file+1, r=rank; f<7; f++) rookMasks[sq] |= getWithSetBit(r*8 + f);
    }
}

Bitboard Chess::rookAttacks(int square, Bitboard occ) {
    Bitboard attacks = 0ULL;
    int rank = square / 8;
    int file = square % 8;

    // NORTH
    for (int r = rank + 1; r < 8; ++r) {
        int sq = r * 8 + file;
        if (occ & (1ULL << sq)){
            attacks |= (1ULL << sq);
            break;
        } 
    }

    // SOUTH
    for (int r = rank - 1; r >= 0; --r) {
        int sq = r * 8 + file;
        if (occ & (1ULL << sq)){
            attacks |= (1ULL << sq);
            break;
        } 
    }

    // EAST
    for (int f = file + 1; f < 8; ++f) {
        int sq = rank * 8 + f;
        if (occ & (1ULL << sq)){
            attacks |= (1ULL << sq);
            break;
        } 
    }

    // WEST
    for (int f = file - 1; f >= 0; --f) {
        int sq = rank * 8 + f;
        if (occ & (1ULL << sq)){
            attacks |= (1ULL << sq);
            break;
        } 
    }

    return attacks;
}

Bitboard Chess::bishopAttacks(int square, Bitboard occ) {
    Bitboard attacks = 0ULL;
    int rank = square / 8;
    int file = square % 8;

    // NE
    for (int r = rank + 1, f = file + 1; r < 8 && f < 8; ++r, ++f) {
        int sq = r * 8 + f;
        if (occ & (1ULL << sq)){
            attacks |= (1ULL << sq);
            break;
        } 
    }

    // NW
    for (int r = rank + 1, f = file - 1; r < 8 && f >= 0; ++r, --f) {
        int sq = r * 8 + f;
        if (occ & (1ULL << sq)){
            attacks |= (1ULL << sq);
            break;
        } 
    }

    // SE
    for (int r = rank - 1, f = file + 1; r >= 0 && f < 8; --r, ++f) {
        int sq = r * 8 + f;
        if (occ & (1ULL << sq)){
            attacks |= (1ULL << sq);
            break;
        } 
    }

    // SW
    for (int r = rank - 1, f = file - 1; r >= 0 && f >= 0; --r, --f) {
        int sq = r * 8 + f;
        if (occ & (1ULL << sq)){
            attacks |= (1ULL << sq);
            break;
        } 
    }

    return attacks;
}

