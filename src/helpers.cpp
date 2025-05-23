#include "chess.h"


U8 Chess::getFromSquare(Move& move) {
    return (move & SQUARE_MASK);
}
U8 Chess::getToSquare(Move& move) {
    return ((move >> TO_SHIFT) & SQUARE_MASK);
}
U8 Chess::getFlags(Move& move) {
    return ((move >> FLAG_SHIFT) & FLAG_MASK);
}
U8 Chess::getPromotedPiece(Move& move) {
    U8 flags = getFlags(move);

    if (flags==QUEEN_PROM_FLAG) return QUEEN;
    if (flags==ROOK_PROM_FLAG) return ROOK;
    if (flags==BISHOP_PROM_FLAG) return BISHOP;
    if (flags==KNIGHT_PROM_FLAG) return KNIGHT;
    return NO_PIECE;
}
bool Chess::isCaptureMove(Move& move) {
    return ((move >> CAPTURE_SHIFT) & 1);
}
U8 Chess::squareFromNotation(const std::string& square) {
    return (square[1] - '1') * 8 + (square[0] - 'a');
}
std::string Chess::notationFromSquare(Move& move) {
    U8 from = getFromSquare(move);
    U8 to = getToSquare(move);
    std::string promPiece = getPromotionNotation(move);
    
    return std::string(1, getFile(from) + 'a') +
    std::string(1, getRank(from) + '1') +
    std::string(1, getFile(to) + 'a') +
    std::string(1, getRank(to) + '1') +
    promPiece;
}

U8 Chess::getRank(U8 square) {
    return (char)(square/8);
}
U8 Chess::getFile(U8 square) {
    return (char)(square%8);
}
U8 Chess::getPieceColor(U8 square) {
    return ((1ULL << square) & whitePieces) ? 1 : 0;
}
U8 Chess::getPieceType(int square) {
    Bitboard mask = (1ULL << square);

    if ((bitboards[PAWN][WHITE]   & mask) || (bitboards[PAWN][BLACK]   & mask)) return PAWN;
    if ((bitboards[KNIGHT][WHITE] & mask) || (bitboards[KNIGHT][BLACK] & mask)) return KNIGHT;
    if ((bitboards[BISHOP][WHITE] & mask) || (bitboards[BISHOP][BLACK] & mask)) return BISHOP;
    if ((bitboards[ROOK][WHITE]   & mask) || (bitboards[ROOK][BLACK]   & mask)) return ROOK;
    if ((bitboards[QUEEN][WHITE]  & mask) || (bitboards[QUEEN][BLACK]  & mask)) return QUEEN;
    if ((bitboards[KING][WHITE]   & mask) || (bitboards[KING][BLACK]   & mask)) return KING;

    return NO_PIECE;
}
bool Chess::inPromoPosition(int square) {
    return getRank(square) == (currentTurn ? WHITE_PROMOTION_RANK : BLACK_PROMOTION_RANK);
}
bool Chess::inStartingPos(int square) {
    return getRank(square) == (currentTurn ? WHITE_PAWN_STARTING_RANK : BLACK_PAWN_STARTING_RANK);
}

std::string Chess::getPromotionNotation(Move& move) {
    U8 flags = getFlags(move);

    switch (flags)
    {
    case QUEEN_PROM_FLAG:
        return "q";
        break;
    case ROOK_PROM_FLAG:
        return "r";
        break;
    case BISHOP_PROM_FLAG:
        return "b";
        break;
    case KNIGHT_PROM_FLAG:
        return "n";
        break;
    
    default:
        return "";
        break;
    }
}
std::string Chess::getFen() {
    std::string fen = "";

    int emptySquares = 0;
    int square = 56;
    while (true) {

        
        if (getPieceType(square) == NO_PIECE) {
            emptySquares++;
        } else {
            char current = notations[getPieceType(square)][!getPieceColor(square)];
            if (emptySquares > 0) {
                fen += std::to_string(emptySquares);
                emptySquares=0;
            }
            fen += std::string(1, current);
        }
        
        if (square == 7) break;
        square++;
        if (!(square%8)){
            if (emptySquares > 0) {
                fen += std::to_string(emptySquares);
                emptySquares=0;
            }
            fen += "/";
            square-=16;
        } 

    }
    fen += currentTurn ? " w " : " b ";
    if (castlingRights & 0b1000) {
        fen += "K";
    } 
    if (castlingRights & 0b0100) {
        fen += "Q";
    } 
    if (castlingRights & 0b0010) {
        fen += "k";
    } 
    if (castlingRights & 0b0001) {
        fen += "q";
    } 

    fen += " ";

    if (enPassantSquare == 64) {
        fen += "- ";
    } else {
        fen += std::string(1, getFile(enPassantSquare) + 'a') + std::string(1, getRank(enPassantSquare) + '1') + " ";
    }

    fen += std::to_string(moveRule50Count);



    return fen;
}

Move Chess::constructMove(U8 from, U8 to, U8 capture, U8 flags) {
    return (from | (to << TO_SHIFT) | (capture << CAPTURE_SHIFT) | (flags << FLAG_SHIFT));
}
void Chess::movePiece(U8 piece, U8 color, U8 from, U8 to) {
    unsetBit(bitboards[piece][color], from); setBit(bitboards[piece][color], to);
}

void Chess::displayBitMap(Bitboard map) {
        
    int square = 56;
    for (int i=0; i<64; i++) {
        if (i>0 && i%8==0) {
            square-=16;
            std::cout<<"\n";
        }
        std::cout << ((map >> square) & 1ULL) << " ";
        square++;
    }
    std::cout << std::endl;
    std::cout << "\n";

}
void Chess::displayBoard() {
    int square = 56;
    for (int i=0; i<64; i++) {
        if (i>0 && i%8==0) {
            square-=16;
            std::cout<<"\n";
        }
        U8 piece = getPieceType(square);
        if (piece == NO_PIECE) {
            std::cout << ". ";
        } else {
            int color = getPieceColor(square); // 0 = WHITE, 1 = BLACK
            std::cout << notations[piece][!color] << " ";
        }
        square++;
    }
    std::cout << std::endl;
    std::cout << "\n";
}
void Chess::displayMoves(std::vector<Move> moves) {
    for (Move move : moves) {
        std::cout << (char)(getFile(getFromSquare(move)) + 'a') << (char)(getRank(getFromSquare(move)) + '1') << (char)(getFile(getToSquare(move)) + 'a') <<  (char)(getRank(getToSquare(move)) + '1') << " = " << (char)(getPromotionNotation(move)[0]-32) << "\n";
    }
}

void Chess::init(std::string fen) {
    // Bottom left corner is LSB, Top right corner is MSB
    U8 square = 56; // starting at a8 moving right
    U8 index = 0;
    U8 inc = 0;
    
    
    for (char value : fen) {
        index++;
        if (value == ' ') {
            break;
        } else if (value == '/'){
            square -= 16; 
        } else if (isdigit(value)) {
            inc = value - '0';
            square += inc;
            
        } else {
            switch (value)
            {
            case 'P':
                setBit(bitboards[PAWN][WHITE], square);
                break;
            case 'p':
                setBit(bitboards[PAWN][BLACK], square);
                break;

            case 'N':
                setBit(bitboards[KNIGHT][WHITE], square);
                break;
            case 'n':
                setBit(bitboards[KNIGHT][BLACK], square);
                break;

            case 'B':
                setBit(bitboards[BISHOP][WHITE], square);
                break;
            case 'b':
                setBit(bitboards[BISHOP][BLACK], square);
                break;

            case 'R':
                setBit(bitboards[ROOK][WHITE], square);
                break;

            case 'r':
                setBit(bitboards[ROOK][BLACK], square);
                break;
            
            case 'Q':
                setBit(bitboards[QUEEN][WHITE], square);
                break;
            case 'q':
                setBit(bitboards[QUEEN][BLACK], square);
                break;
            
            case 'K':
                setBit(bitboards[KING][WHITE], square);
                kingSquare[1] = square;
                break;
            case 'k':
                setBit(bitboards[KING][BLACK], square);
                kingSquare[0] = square;
                break;
            
            default:
                break;
            }
            square++;

        }
    }

    currentTurn = (fen[index] == 'w' ? 1 : 0);

    index +=2;
    
    while (fen[index] != ' ') {
        // Set castling rights
        switch (fen[index])
        {
        case 'K':
            castlingRights |= 1ULL << 3;
            break;
        case 'Q':
            castlingRights |= 1ULL << 2;
            break;
        case 'k':
            castlingRights |= 1ULL << 1;
            break;
        case 'q':
            castlingRights |= 1ULL << 0;
            break;
    
        
        default:
            break;
        }

        index++;
    }

    index++;

    if (fen[index] == '-')  {
        enPassantSquare = 64;
    } else {
        enPassantSquare = (fen[index+1] - '1') * 8 + (fen[index] - 'a');
    }

    index+=2;

    moveRule50Count = (fen[index] - '0');
    
    std::cout << "Finished Parsing Fen" << "\n";
    std::cout << "En Passant Square = " << (int)enPassantSquare << "\n\n";

    saveChanges(); 
    initMoveTables();
    initAttackTables();
}
void Chess::saveChanges() {
    whitePieces = 0;
    blackPieces = 0;
    for (int piece = PAWN; piece <= KING; ++piece) {
        whitePieces |= bitboards[piece][WHITE];
        blackPieces |= bitboards[piece][BLACK];
    }

    allPieces = whitePieces | blackPieces;  
}

void Chess::setBit(Bitboard &map, U8 index) {
    map |= (1ULL << index);
}
void Chess::unsetBit(Bitboard &map, U8 index) {
    map &= ~(1ULL << index);
}
int Chess::countBits(Bitboard& map) {
    return __builtin_popcountll(map);
}
Bitboard Chess::getWithSetBit(int square) {
    return (1ULL << square);
}
