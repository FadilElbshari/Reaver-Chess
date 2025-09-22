#include "chess.h"

bool Chess::isInCheck(int kingsqr, bool flip) {
    bool isWhite = (kingsqr == -1) ? !currentTurn : currentTurn;
    if (flip) isWhite = (kingsqr == -1) ? currentTurn : !currentTurn;

    int K_SQUARE = (kingsqr == -1) ? kingSquare[isWhite] : kingsqr;

    if(pawnAttacks[isWhite][K_SQUARE] & bitboards[PAWN][!isWhite]) return true;
    if(knightAttacks[K_SQUARE] & bitboards[KNIGHT][!isWhite]) return true;
    if(kingAttacks[K_SQUARE] & bitboards[KING][!isWhite]) return true;
    if(rookAttacks(K_SQUARE, allPieces) & (bitboards[ROOK][!isWhite] | bitboards[QUEEN][!isWhite])) return true;
    if(bishopAttacks(K_SQUARE, allPieces) & (bitboards[BISHOP][!isWhite] | bitboards[QUEEN][!isWhite])) return true;


    return false;
}

bool Chess::isLegalMove(U8 from, U8 to, int promotionPiece) {

    int moveCount = 0;
    U8 pieceType = getPieceType(from);
    U8 pieceColor = getPieceColor(from);

    if (pieceType==NO_PIECE || pieceColor!=currentTurn) return false;

    Move moveBuffer[MAX_MOVES];
    if (pieceType==PAWN) {
        moveCount = getPawnMoves(moveBuffer, from);
    } else if (pieceType==BISHOP) {
        moveCount = getBishopMoves(moveBuffer, from);
    } else if (pieceType==KNIGHT) {
        moveCount = getKnightMoves(moveBuffer, from);
    } else if (pieceType==ROOK) {
        moveCount = getRookMoves(moveBuffer, from);
    } else if (pieceType==QUEEN) {
        moveCount = getQueenMoves(moveBuffer, from);
    } else if (pieceType==KING) {
        moveCount = getKingMoves(moveBuffer, from);
    } 

    // Perform "Checks" and possible "Pins"
    bool isLegal=false;
    for (int i=0; i<moveCount; i++) {
        Move move = moveBuffer[i];
        
        if(getFromSquare(move) == from && getToSquare(move) == to && promotionPiece == getPromotedPiece(move)) {

            makeMove(move);
            isLegal = (isInCheck() ? false : true);
            
            if (!isLegal){
                undoMove();
            }
        }
    }

    
    return isLegal;

}

bool Chess::isCheckMate() {
    if (!isInCheck(-1, true)) return false;
        Move moveBuffer[MAX_MOVES];

        int moveCount = GenerateMoves(moveBuffer);

        for (int i=0; i<moveCount; i++) {
            Move move = moveBuffer[i];
            makeMove(move);
            bool isLegal = !isInCheck();
            undoMove();
            if (isLegal) return false;
        }

        return true;
    } 

bool Chess::isStaleMate() {
    if (isInCheck(-1, true)) return false;
        Move moveBuffer[MAX_MOVES];

        int moveCount = GenerateMoves(moveBuffer);
        for (int i=0; i<moveCount; i++) {
            Move move = moveBuffer[i];
            makeMove(move);
            bool isLegal = !isInCheck();
            undoMove();
            if (isLegal) return false;
        }

        return true;
    }

U8 Chess::isGameOver() {

    U8 RESULT = STALE_MATE;
    if (isInCheck(kingSquare[currentTurn])) RESULT=CHECK_MATE;

    Move moveBuffer[MAX_MOVES];

    int moveCount = GenerateMoves(moveBuffer);
    for (int i=0; i<moveCount; i++) {
        Move move = moveBuffer[i];
        makeMove(move);
        bool isLegal = !isInCheck();
        undoMove();
        if (isLegal) return NOT_OVER;

    }
    return RESULT;
}