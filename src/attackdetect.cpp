#include "chess.h"

void Chess::initAttackTables() {
    for (int sq = 0; sq < 64; ++sq) {
        int rank = sq / 8;
        int file = sq % 8;

        // --- Pawn Attacks ---
        // White
        if (file > 0 && rank < 7) pawnAttacks[WHITE][sq] |= getWithSetBit(sq + 7);
        if (file < 7 && rank < 7) pawnAttacks[WHITE][sq] |= getWithSetBit(sq + 9);
        // Black
        if (file > 0 && rank > 0) pawnAttacks[BLACK][sq] |= getWithSetBit(sq - 9);
        if (file < 7 && rank > 0) pawnAttacks[BLACK][sq] |= getWithSetBit(sq - 7);

        // --- Knight Attacks ---
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

        // --- King Attacks ---
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

Bitboard Chess::rookMask(int square) {
    Bitboard mask = 0ULL;
    int rank = square / 8;
    int file = square % 8;

    // Vertical (up and down)
    for (int r = rank + 1; r <= 6; r++) mask |= (1ULL << (r * 8 + file));
    for (int r = rank - 1; r >= 1; r--) mask |= (1ULL << (r * 8 + file));

    // Horizontal (left and right)
    for (int f = file + 1; f <= 6; f++) mask |= (1ULL << (rank * 8 + f));
    for (int f = file - 1; f >= 1; f--) mask |= (1ULL << (rank * 8 + f));

    return mask;
}

Bitboard Chess::indexToBlockers(int index, Bitboard mask) {
    Bitboard blockers = 0ULL;
    int bits = countBits(mask);
    int i = 0;

    for (int j = 0; j < 64 && i < bits; j++) {
        if ((mask >> j) & 1ULL) {
            if (index & (1 << i))
                blockers |= (1ULL << j);
            i++;
        }
    }

    return blockers;
}