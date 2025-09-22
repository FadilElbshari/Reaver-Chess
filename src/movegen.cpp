#include "chess.h"

void Chess::initMoveTables() {
    for (int sq=0; sq < 64; sq++) {
        int rank = sq / 8;
        int file = sq % 8;

        // White
        if (rank > WHITE_PAWN_STARTING_RANK && rank <= BLACK_PAWN_STARTING_RANK) pawnMoves[WHITE][sq] |= getWithSetBit(sq + 8);
        if (rank == WHITE_PAWN_STARTING_RANK) pawnMoves[WHITE][sq] |= getWithSetBit(sq + 16);
        if (file > 0 && rank < 7) pawnMoves[WHITE][sq] |= getWithSetBit(sq + 7);
        if (file < 7 && rank < 7) pawnMoves[WHITE][sq] |= getWithSetBit(sq + 9);
        // Black
        if (rank < BLACK_PAWN_STARTING_RANK && rank >= WHITE_PAWN_STARTING_RANK) pawnMoves[BLACK][sq] |= getWithSetBit(sq - 8);
        if (rank == BLACK_PAWN_STARTING_RANK) pawnMoves[BLACK][sq] |= getWithSetBit(sq - 16);
        if (file > 0 && rank > 0) pawnMoves[BLACK][sq] |= getWithSetBit(sq - 9);
        if (file < 7 && rank > 0) pawnMoves[BLACK][sq] |= getWithSetBit(sq - 7);

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
        knightMoves[sq] = knight;
    }
}

int Chess::getPawnMoves(Move* buffer, U8 square) {
    int count = 0;
    U8 isWhite = currentTurn;
    U8 multiplier = isWhite ? 1 : -1;
    U8 target;

    if (inPromoPosition(square)){

        // Promo captures
        target = square+9*multiplier;
        if ((getWithSetBit(target) & (isWhite ? blackPieces : whitePieces))) {
            buffer[count++] = constructMove(square, target, 1, QUEEN_PROM_FLAG);
            buffer[count++] = constructMove(square, target, 1, ROOK_PROM_FLAG);
            buffer[count++] = constructMove(square, target, 1, BISHOP_PROM_FLAG);
            buffer[count++] = constructMove(square, target, 1, KNIGHT_PROM_FLAG);
        }
        target = square+7*multiplier;
        if ((getWithSetBit(target) & (isWhite ? blackPieces : whitePieces))) {
            buffer[count++] = constructMove(square, target, 1, QUEEN_PROM_FLAG);
            buffer[count++] = constructMove(square, target, 1, ROOK_PROM_FLAG);
            buffer[count++] = constructMove(square, target, 1, BISHOP_PROM_FLAG);
            buffer[count++] = constructMove(square, target, 1, KNIGHT_PROM_FLAG);
        }
        
        
        target = square+8*multiplier;
        if (!(getWithSetBit(target) & allPieces)) { // Non-capture promotion
            buffer[count++] = constructMove(square, target, 0, QUEEN_PROM_FLAG);
            buffer[count++] = constructMove(square, target, 0, ROOK_PROM_FLAG);
            buffer[count++] = constructMove(square, target, 0, BISHOP_PROM_FLAG);
            buffer[count++] = constructMove(square, target, 0, KNIGHT_PROM_FLAG);
        }

    } else {

        // Pawn captures
        target = square+9*multiplier;
        if ((getWithSetBit(target) & (isWhite ? blackPieces : whitePieces) & pawnMoves[currentTurn][square])) {
            buffer[count++] = constructMove(square, target, 1, NOTHING_FLAG);
        }
        target = square+7*multiplier;
        if ((getWithSetBit(target) & (isWhite ? blackPieces : whitePieces) & pawnMoves[currentTurn][square])) {
            buffer[count++] = constructMove(square, target, 1, NOTHING_FLAG);
        }

        // Forward pawn moves
        target = square+8*multiplier;
        if (!(getWithSetBit(target) & allPieces)) {// This means one square move is free
            buffer[count++] = constructMove(square, target, 0, NOTHING_FLAG);
            if (inStartingPos(square)) { // This means the pawn is in initial position
                target = square+16*multiplier;
                if (!(getWithSetBit(target) & allPieces)) {// This means double square move is free
                    buffer[count++] = constructMove(square, target, 0, NOTHING_FLAG);
                }

            }
    }
    // En-passant
    target = enPassantSquare;
    if ((getRank(square) == (isWhite ? 4 : 3)) && enPassantSquare != 64) {
            // Check if it's an adjacent file
            if (abs((int)getFile(enPassantSquare) - (int)getFile(square)) == 1) {
                buffer[count++] = constructMove(square, target, 1, EN_PASSANT_FLAG);
            }
        }
    }
    return count;

}

int Chess::getBishopMoves(Move* buffer, U8 square) {
    int count = 0;
    U8 isWhite = currentTurn;

    Bitboard occupiedByUs = isWhite ? whitePieces : blackPieces;
    Bitboard occupiedByThem = isWhite ? blackPieces : whitePieces;

    // Define direction vectors
    static const int directions[4] = {9, 7, -7, -9};
    for (int dir : directions) {
        int offset = dir;
        int current = square;

        while (true) {
            int target = current + offset;

            if (target < 0 || target >= 64) break;
            if (abs(getFile(target) - getFile(current)) != 1) break;

            Bitboard targetMask = 1ULL << target;
            if (targetMask & occupiedByUs) break;

            if (targetMask & occupiedByThem) {
                buffer[count++] = constructMove(square, target, 1, NOTHING_FLAG);
                break;
            } 

            buffer[count++] = constructMove(square, target, 0, NOTHING_FLAG);
            current = target;
        }
    }
    return count;

}

int Chess::getKnightMoves(Move* buffer, U8 square) {
    int count = 0;
    U8 isWhite = currentTurn;

    Bitboard occupiedByUs = isWhite ? whitePieces : blackPieces;
    Bitboard occupiedByThem = isWhite ? blackPieces : whitePieces;

    const int knightOffsets[8] = {17, 15, 10, 6, -6, -10, -15, -17};

    for (int i = 0; i < 8; i++) {
        U8 target = square+knightOffsets[i];
        if (!(getWithSetBit(target) & knightMoves[square])) continue;

        Bitboard targetMask = 1ULL << target;
        if (targetMask & occupiedByUs) continue;

        if (targetMask & occupiedByThem) {
                buffer[count++] = constructMove(square, target, 1, NOTHING_FLAG);
                continue;
            }
            buffer[count++] = constructMove(square, target, 0, NOTHING_FLAG);
    }

    return count;
}

int Chess::getRookMoves(Move* buffer, U8 square) {
    int count = 0;
    U8 isWhite = currentTurn;

    // Directions: up, down, right, left
    const int directions[4] = {8, -8, 1, -1};

    Bitboard occupiedByUs = isWhite ? whitePieces : blackPieces;
    Bitboard occupiedByThem = isWhite ? blackPieces : whitePieces;

    for (int dir = 0; dir < 4; ++dir) {
        int offset = directions[dir];
        int current = square;

        while (true) {
            int target = current+offset;

            if (target < 0 || target >= 64) break;
            if ((offset == 1 || offset == -1) && (target / 8 != (target - offset) / 8)) break;

            Bitboard targetMask = 1ULL << target;
            if (targetMask & occupiedByUs) break;

            
            if (targetMask & occupiedByThem) {
                    buffer[count++] = constructMove(square, target, 1, NOTHING_FLAG);
                    break;
                }

            buffer[count++] = constructMove(square, target, 0, NOTHING_FLAG);
            current = target;
        }
    }
    return count;

}

int Chess::getQueenMoves(Move* buffer, U8 square) {
    int count = 0;

    count += getBishopMoves(buffer+count, square);
    count += getRookMoves(buffer+count, square);

    return count;

}

int Chess::getKingMoves(Move* buffer, U8 square) {
    int count = 0;
    U8 isWhite = currentTurn;

    Bitboard occupiedByUs = isWhite ? whitePieces : blackPieces;
    Bitboard occupiedByThem = isWhite ? blackPieces : whitePieces;

    const int squares[8] = {8, -8, -1, 1, 7, 9, -9, -7}; // Up, Down, Left, Right, Up-Left, Up-Right, Down-Left, Down-Right

    for (int i=0; i<8; i++) {
        U8 target = square+squares[i];
        if (target >= 64) continue;

        int fileDiff = abs(getFile(target) - getFile(square));
        if (fileDiff > 1) continue;

        Bitboard targetMask = 1ULL << target;
        if (targetMask & occupiedByUs) continue;

        if (targetMask & occupiedByThem) {
                buffer[count++] = constructMove(square, target, 1, NOTHING_FLAG);
                continue;
            }
        buffer[count++] = constructMove(square, target, 0, NOTHING_FLAG);
    }

    // King side castle
    U8 offset = 2;
    if (castlingRights & (1ULL << (isWhite ? 3 : 1))) { // K-side castling
        bool squaresEmpty =
            !((1ULL << (square + 1)) & allPieces) &&
            !((1ULL << (square + 2)) & allPieces);

        bool rookPresent = (1ULL << (square + 3)) & (bitboards[ROOK][isWhite]);

        if (squaresEmpty && rookPresent) {
            if (!isInCheck(square) &&
                !isInCheck(square + 1) &&
                !isInCheck(square + 2)) {

                buffer[count++] = constructMove(square, square+offset, 0, CASTLE_FLAG);
            }
        }
    }


    // Queen side castle
    offset = -2;
    if (castlingRights & (1ULL << (isWhite ? 2 : 0))) { // Q-side castling
        bool squaresEmpty =
            !((1ULL << (square - 1)) & allPieces) &&
            !((1ULL << (square - 2)) & allPieces) &&
            !((1ULL << (square - 3)) & allPieces);
    
        bool rookPresent = (1ULL << (square - 4)) & (bitboards[ROOK][isWhite]);
    
        if (squaresEmpty && rookPresent) {
            if (!isInCheck(square) &&
                !isInCheck(square - 1) &&
                !isInCheck(square - 2)) {
    
                buffer[count++] = constructMove(square, square+offset, 0, CASTLE_FLAG);
            }
        }
    }

    return count;

}

int Chess::GenerateMoves(Move* buffer) {

    int count = 0;

    for (int from = 0; from < 64; ++from) {
        U8 pieceType = getPieceType(from);
        if (pieceType==NO_PIECE || getPieceColor(from)!=currentTurn) continue;

        if (pieceType == PAWN) {
            count += getPawnMoves(buffer+count, from);
        }
        else if (pieceType == BISHOP) {
            count += getBishopMoves(buffer+count, from);
        }
        else if (pieceType == KNIGHT) {
            count += getKnightMoves(buffer+count, from);
        }
        else if (pieceType == ROOK) {
            count += getRookMoves(buffer+count, from);
        }
        else if (pieceType == QUEEN) {
            count += getQueenMoves(buffer+count, from);
        }
        else if (pieceType == KING) {
            count += getKingMoves(buffer+count, from);
        }

        
    }

    return count;
}

int Chess::GenerateLegalMoves(Move* buffer) {

    int count = 0;
    Move moveBuffer[256];

    for (int from = 0; from < 64; ++from) {
        U8 pieceType = getPieceType(from);
        if (pieceType==NO_PIECE || getPieceColor(from)!=currentTurn) continue;

        if (pieceType == PAWN) {
            count += getPawnMoves(moveBuffer+count, from);
        }
        else if (pieceType == BISHOP) {
            count += getBishopMoves(moveBuffer+count, from);
        }
        else if (pieceType == KNIGHT) {
            count += getKnightMoves(moveBuffer+count, from);
        }
        else if (pieceType == ROOK) {
            count += getRookMoves(moveBuffer+count, from);
        }
        else if (pieceType == QUEEN) {
            count += getQueenMoves(moveBuffer+count, from);
        }
        else if (pieceType == KING) {
            count += getKingMoves(moveBuffer+count, from);
        }
    }

    

    int legalCounter = 0;

    for (int i = 0; i < count; ++i) {
        makeMove(moveBuffer[i]);
        if (!isInCheck())
            buffer[legalCounter++] = moveBuffer[i];

        undoMove();
    }

    return legalCounter;
}


// ------------------------------------------- JS MODULE -------------------------------------------
std::vector<Move> Chess::GenerateLegalMovesJS() {
    int count = 0;
    std::vector<Move> LegalMoves;
    Move moveBuffer[256];

    for (int from = 0; from < 64; ++from) {
        U8 pieceType = getPieceType(from);
        if (pieceType==NO_PIECE || getPieceColor(from)!=currentTurn) continue;

        if (pieceType == PAWN) {
            count += getPawnMoves(moveBuffer+count, from);
        }
        else if (pieceType == BISHOP) {
            count += getBishopMoves(moveBuffer+count, from);
        }
        else if (pieceType == KNIGHT) {
            count += getKnightMoves(moveBuffer+count, from);
        }
        else if (pieceType == ROOK) {
            count += getRookMoves(moveBuffer+count, from);
        }
        else if (pieceType == QUEEN) {
            count += getQueenMoves(moveBuffer+count, from);
        }
        else if (pieceType == KING) {
            count += getKingMoves(moveBuffer+count, from);
        }
    }



    for (int i = 0; i < count; ++i) {
        makeMove(moveBuffer[i]);
        if (!isInCheck()) {
            Move move = moveBuffer[i];
            LegalMoves.push_back(move);
        }
        undoMove();
    }

    return LegalMoves;
}