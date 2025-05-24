#include "chess.h"

void Chess::makeMove(Move &move) {
    U8 from = getFromSquare(move);
    U8 to = getToSquare(move);

    U8 pieceType = getPieceType(from);
    U8 flags = getFlags(move);

    U8 color = currentTurn;


    GameState gs;

    gs.currentTurn = currentTurn;
    gs.castlingRights = castlingRights;
    gs.enPassantSquare = enPassantSquare;
    gs.fiftyMoveRuleCounter = moveRule50Count;
    gs.capturedPiece = NO_PIECE;
    gs.promotedPiece = NO_PIECE;

    gs.kingSquare[0] = kingSquare[0];
    gs.kingSquare[1] = kingSquare[1];

    memcpy(gs.bitboards, bitboards, sizeof(Bitboard)*12);

    gs.whitePieces = whitePieces;
    gs.blackPieces = blackPieces;
    gs.allPieces = allPieces;

    gameStateStack[stackPointer] = gs;
    movesStack[stackPointer] = move;
    stackPointer++;

    bool isCapture = isCaptureMove(move);

    promotedPiece = getPromotedPiece(move);
    capturedPiece = getPieceType(to);

    // Handling En Passant
    enPassantSquare = 64;
    if ((pieceType == PAWN) && abs(to-from) == 16) {
        enPassantSquare = to + (currentTurn ? -8 : 8);
    }

    // Handling Castling Rights
    if (pieceType==KING){
        castlingRights &= ~(1ULL << (currentTurn ? 3 : 1)); // King Side
        castlingRights &= ~(1ULL << (currentTurn ? 2 : 0)); // Queen Side
        kingSquare[currentTurn] = to;
    } else if (pieceType==ROOK) {
        if (from == (currentTurn ? 0 : 56)) {
            castlingRights &= ~(1ULL << (currentTurn ? 2 : 0));
        } else if (from == (currentTurn ? 7 : 63)) {
            castlingRights &= ~(1ULL << (currentTurn ? 3 : 1));
        }
    }

    
    
    if (promotedPiece != NO_PIECE) { // Handle Promotion
        unsetBit(bitboards[pieceType][color], from);
        setBit(bitboards[promotedPiece][color], to);
        if (isCapture) {
            
            unsetBit(bitboards[capturedPiece][!color], to);
        }
    } else {
        movePiece(pieceType, color, from, to);
        if (flags == EN_PASSANT_FLAG) {
            capturedPiece = PAWN;
            unsetBit(bitboards[capturedPiece][!color], to + (!color ? 8 : -8));
        } else if (isCapture) {
            unsetBit(bitboards[capturedPiece][!color], to);
        } else if (flags == CASTLE_FLAG) {
            if (getFile(to) == KING_SIDE_CASTLE_FILE) {
                unsetBit(bitboards[ROOK][color], from+3);
                setBit(bitboards[ROOK][color], from+1);
            } else if (getFile(to) == QUEEN_SIDE_CASTLE_FILE) {
                unsetBit(bitboards[ROOK][color], from-4);
                setBit(bitboards[ROOK][color], from-1);
            }
        }
    }

    if ((pieceType == PAWN) || isCapture) {
        moveRule50Count=0;
    } else {
        moveRule50Count++;
    }

    currentTurn ^= 1;

    saveChanges();
}

void Chess::undoMove() {

    if (stackPointer==0) return;

    stackPointer--;
    GameState gs = gameStateStack[stackPointer];

    currentTurn ^= 1;
    castlingRights = gs.castlingRights;
    enPassantSquare = gs.enPassantSquare;
    moveRule50Count = gs.fiftyMoveRuleCounter;
    capturedPiece = gs.capturedPiece;
    promotedPiece = gs.promotedPiece;

    kingSquare[0] = gs.kingSquare[0];
    kingSquare[1] = gs.kingSquare[1];

    memcpy(bitboards, gs.bitboards, sizeof(Bitboard)*12);

    whitePieces = gs.whitePieces;
    blackPieces = gs.blackPieces;
    allPieces = gs.allPieces;
}

void Chess::move(std::string move) {

    U8 from = squareFromNotation(move.substr(0, 2));
    U8 to = squareFromNotation(move.substr(2, 2));

    bool legality = isLegalMove(from, to);
    if (legality) {
        std::cout << "Move Made\n";
        return;
    }
    std::cout << "Illegal Move\n";
}

// ------------------------------------------- JS MODULE -------------------------------------------

void Chess::makeMoveJS(Move move) {
    U8 from = getFromSquare(move);
    U8 to = getToSquare(move);

    U8 pieceType = getPieceType(from);
    U8 flags = getFlags(move);

    U8 color = currentTurn;


    GameState gs;

    gs.currentTurn = currentTurn;
    gs.castlingRights = castlingRights;
    gs.enPassantSquare = enPassantSquare;
    gs.fiftyMoveRuleCounter = moveRule50Count;
    gs.capturedPiece = NO_PIECE;
    gs.promotedPiece = NO_PIECE;

    gs.kingSquare[0] = kingSquare[0];
    gs.kingSquare[1] = kingSquare[1];

    memcpy(gs.bitboards, bitboards, sizeof(Bitboard)*12);

    gs.whitePieces = whitePieces;
    gs.blackPieces = blackPieces;
    gs.allPieces = allPieces;

    gameStateStack[stackPointer] = gs;
    movesStack[stackPointer] = move;
    stackPointer++;

    bool isCapture = isCaptureMove(move);

    promotedPiece = getPromotedPiece(move);
    capturedPiece = getPieceType(to);

    // Handling En Passant
    enPassantSquare = 64;
    if ((pieceType == PAWN) && abs(to-from) == 16) {
        enPassantSquare = to + (currentTurn ? -8 : 8);
    }

    // Handling Castling Rights
    if (pieceType==KING){
        castlingRights &= ~(1ULL << (currentTurn ? 3 : 1)); // King Side
        castlingRights &= ~(1ULL << (currentTurn ? 2 : 0)); // Queen Side
        kingSquare[currentTurn] = to;
    } else if (pieceType==ROOK) {
        if (from == (currentTurn ? 0 : 56)) {
            castlingRights &= ~(1ULL << (currentTurn ? 2 : 0));
        } else if (from == (currentTurn ? 7 : 63)) {
            castlingRights &= ~(1ULL << (currentTurn ? 3 : 1));
        }
    }

    
    
    if (promotedPiece != NO_PIECE) { // Handle Promotion
        unsetBit(bitboards[pieceType][color], from);
        setBit(bitboards[promotedPiece][color], to);
        if (isCapture) {
            
            unsetBit(bitboards[capturedPiece][!color], to);
        }
    } else {
        movePiece(pieceType, color, from, to);
        if (flags == EN_PASSANT_FLAG) {
            capturedPiece = PAWN;
            unsetBit(bitboards[capturedPiece][!color], to + (!color ? 8 : -8));
        } else if (isCapture) {
            unsetBit(bitboards[capturedPiece][!color], to);
        } else if (flags == CASTLE_FLAG) {
            if (getFile(to) == KING_SIDE_CASTLE_FILE) {
                unsetBit(bitboards[ROOK][color], from+3);
                setBit(bitboards[ROOK][color], from+1);
            } else if (getFile(to) == QUEEN_SIDE_CASTLE_FILE) {
                unsetBit(bitboards[ROOK][color], from-4);
                setBit(bitboards[ROOK][color], from-1);
            }
        }
    }

    if ((pieceType == PAWN) || isCapture) {
        moveRule50Count=0;
    } else {
        moveRule50Count++;
    }

    currentTurn ^= 1;

    saveChanges();
}
