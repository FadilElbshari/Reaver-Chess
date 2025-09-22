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

    gs.bitboards[PAWN][WHITE] = bitboards[PAWN][WHITE];
    gs.bitboards[BISHOP][WHITE] = bitboards[BISHOP][WHITE];
    gs.bitboards[KNIGHT][WHITE] = bitboards[KNIGHT][WHITE];
    gs.bitboards[ROOK][WHITE] = bitboards[ROOK][WHITE];
    gs.bitboards[QUEEN][WHITE] = bitboards[QUEEN][WHITE];
    gs.bitboards[KING][WHITE] = bitboards[KING][WHITE];

    gs.bitboards[PAWN][BLACK] = bitboards[PAWN][BLACK];
    gs.bitboards[BISHOP][BLACK] = bitboards[BISHOP][BLACK];
    gs.bitboards[KNIGHT][BLACK] = bitboards[KNIGHT][BLACK];
    gs.bitboards[ROOK][BLACK] = bitboards[ROOK][BLACK];
    gs.bitboards[QUEEN][BLACK] = bitboards[QUEEN][BLACK];
    gs.bitboards[KING][BLACK] = bitboards[KING][BLACK];

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

    currentTurn = gs.currentTurn;
    castlingRights = gs.castlingRights;
    enPassantSquare = gs.enPassantSquare;
    moveRule50Count = gs.fiftyMoveRuleCounter;
    capturedPiece = gs.capturedPiece;
    promotedPiece = gs.promotedPiece;

    kingSquare[0] = gs.kingSquare[0];
    kingSquare[1] = gs.kingSquare[1];

    bitboards[PAWN][WHITE] = gs.bitboards[PAWN][WHITE];
    bitboards[BISHOP][WHITE] = gs.bitboards[BISHOP][WHITE];
    bitboards[KNIGHT][WHITE] = gs.bitboards[KNIGHT][WHITE];
    bitboards[ROOK][WHITE] = gs.bitboards[ROOK][WHITE];
    bitboards[QUEEN][WHITE] = gs.bitboards[QUEEN][WHITE];
    bitboards[KING][WHITE] = gs.bitboards[KING][WHITE];

    bitboards[PAWN][BLACK] = gs.bitboards[PAWN][BLACK];
    bitboards[BISHOP][BLACK] = gs.bitboards[BISHOP][BLACK];
    bitboards[KNIGHT][BLACK] = gs.bitboards[KNIGHT][BLACK];
    bitboards[ROOK][BLACK] = gs.bitboards[ROOK][BLACK];
    bitboards[QUEEN][BLACK] = gs.bitboards[QUEEN][BLACK];
    bitboards[KING][BLACK] = gs.bitboards[KING][BLACK];

    whitePieces = gs.whitePieces;
    blackPieces = gs.blackPieces;
    allPieces = gs.allPieces;
}

bool Chess::move(std::string move) {

    U8 from = squareFromNotation(move.substr(0, 2));
    U8 to = squareFromNotation(move.substr(2, 2));

    int promotionPiece = NO_PIECE;
    if (move.length() == 5) {
        switch (move[4])
        {
        case 'Q':
            promotionPiece = QUEEN;
            break;
        case 'R':
            promotionPiece = ROOK;
            break;
        case 'N':
            promotionPiece = KNIGHT;
            break;
        case 'B':
            promotionPiece = BISHOP;
            break;
        
        default:
            break;
        }
    }

    bool legality = isLegalMove(from, to, promotionPiece);
    if (legality) {
        return true;
    }
    return false;
}

// ------------------------------------------- JS MODULE -------------------------------------------

MoveData Chess::makeMoveJS(Move move) {
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
    std::string flagInstance = "";

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
            flagInstance += "e";
            capturedPiece = PAWN;
            unsetBit(bitboards[capturedPiece][!color], to + (!color ? 8 : -8));
        } else if (isCapture) {
            unsetBit(bitboards[capturedPiece][!color], to);
        } else if (flags == CASTLE_FLAG) {
            if (getFile(to) == KING_SIDE_CASTLE_FILE) {
                flagInstance += "k";
                unsetBit(bitboards[ROOK][color], from+3);
                setBit(bitboards[ROOK][color], from+1);
            } else if (getFile(to) == QUEEN_SIDE_CASTLE_FILE) {
                flagInstance += "q";
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

    std::string san;

    if(pieceType!=PAWN) san+=notations[pieceType][0];
    if (isCapture) {
        if(pieceType==PAWN) {
            san+=getNotation(from)[0];
        }
        san+="x";
    }

    san+=getNotation(to);

    if (isInCheck()) san+="+";

    if (getFlags(move) == KING_SIDE_CASTLE_FILE) {
        san = "O-O";
    } else if (getFlags(move) == QUEEN_SIDE_CASTLE_FILE) {
        san = "O-O-O";
    }

    MoveData data = {1, san, (currentTurn ? "w" : "b"), flagInstance};

    currentTurn ^= 1;

    saveChanges();

    return data;
    
}

MoveData Chess::moveJS(std::string move) {

    U8 from = squareFromNotation(move.substr(0, 2));
    U8 to = squareFromNotation(move.substr(2, 2));

    int promotionPiece = NO_PIECE;
    if (move.length() == 5) {
        switch (move[4])
        {
        case 'Q':
            promotionPiece = QUEEN;
            break;
        case 'R':
            promotionPiece = ROOK;
            break;
        case 'N':
            promotionPiece = KNIGHT;
            break;
        case 'B':
            promotionPiece = BISHOP;
            break;
        
        default:
            break;
        }
    }


    std::vector<Move> Moves = GenerateLegalMovesJS();
    MoveData data;

    for (Move move : Moves) {
        if (from == getFromSquare(move) && to == getToSquare(move) && promotionPiece == getPromotedPiece(move)) {
            data = makeMoveJS(move);
            return data;
        }
    }
    data = {0, "", "", ""};
    return data;
}