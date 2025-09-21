#pragma once

#include <iostream>
#include <vector>
#include <math.h>
#include <cstdint>
#include <chrono>
#include <cstring>


#define MAX_MOVES 256
#define MAX_DEPTH 256

typedef uint16_t Move;
typedef uint8_t U8;
typedef uint64_t Bitboard;
typedef double Eval;

enum Color {BLACK, WHITE};
enum Piece {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, NO_PIECE};

struct GameState
{
    U8 currentTurn;
    U8 castlingRights;
    U8 enPassantSquare;
    U8 capturedPiece;
    U8 promotedPiece;
    uint16_t fiftyMoveRuleCounter;

    U8 kingSquare[2];


    Bitboard bitboards[6][2]={};

    Bitboard whitePieces=0, blackPieces=0, allPieces=0;
};

struct MoveType {
    std::string from;
    std::string to;
};

struct MoveData {
    int status;
    std::string san;
    std::string color;
    std::string flags;
};


constexpr Bitboard notAFile = 0xfefefefefefefefeULL;
constexpr Bitboard notHFile = 0x7f7f7f7f7f7f7f7fULL;

constexpr U8 FROM_SHIFT = 0;
constexpr U8 TO_SHIFT = 6;
constexpr U8 CAPTURE_SHIFT = 12;
constexpr U8 FLAG_SHIFT = 13;

constexpr U8 NOTHING_FLAG = 0;
constexpr U8 CAPTURE_FLAG = 1;
constexpr U8 EN_PASSANT_FLAG = 2;
constexpr U8 CASTLE_FLAG = 3;
constexpr U8 QUEEN_PROM_FLAG = 4;
constexpr U8 ROOK_PROM_FLAG = 5;
constexpr U8 BISHOP_PROM_FLAG = 6;
constexpr U8 KNIGHT_PROM_FLAG = 7;

// 0000    0    000000 000000 -> LSB
// flgs   cptr    to    from

constexpr uint16_t SQUARE_MASK  = 0x3F; // 6 bits
constexpr uint16_t CAPTURE_MASK = 1; // single bit
constexpr uint16_t FLAG_MASK  = 7; // 3 bits

const U8 QUEEN_SIDE_CASTLE_FILE = 2;
const U8 KING_SIDE_CASTLE_FILE = 6;

const U8 WHITE_PAWN_STARTING_RANK = 1;
const U8 BLACK_PAWN_STARTING_RANK = 6;

const U8 WHITE_PROMOTION_RANK = 6;
const U8 BLACK_PROMOTION_RANK = 1;

const int INF = 1000000;



class Chess {
        
    public:

    Bitboard bitboards[6][2]={};

    Bitboard whitePieces = 0;
    Bitboard blackPieces = 0;
    Bitboard allPieces = 0;

    Bitboard pawnMoves[2][64];
    Bitboard knightMoves[64];
    Bitboard kingMoves[64];

    Bitboard pawnAttacks[2][64];
    Bitboard knightAttacks[64];
    Bitboard kingAttacks[64];
    Bitboard bishopMasks[64];
    Bitboard rookMasks[64];

    char notations[6][2] = {{'P', 'p'}, {'N', 'n'}, {'B', 'b'}, {'R', 'r'}, {'Q', 'q'}, {'K', 'k'}};
    char promNotations[4] = {'q', 'r', 'b', 'n'};

    U8 kingSquare[2];

    U8 currentTurn = 1; // 1 = White, 0 = Black
    U8 castlingRights = 0; // KQkq <=> 0b1111 
    U8 enPassantSquare = 64; // -1 = no EN_PASSANT, 0 <= square < 64 {Valid Square}
    int moveRule50Count = 0;
    U8 capturedPiece = 12;
    U8 promotedPiece = 12;

    GameState gameStateStack[MAX_DEPTH];
    Move movesStack[MAX_DEPTH];
    U8 pieceLocations[64];

    int stackPointer = 0;

    const U8 NOT_OVER = 0;
    const U8 CHECK_MATE = 1;
    const U8 STALE_MATE = 2;

    const int PAWN_VALUE = 1;
    const int KNIGHT_VALUE = 3;
    const int BISHOP_VALUE = 3;
    const int ROOK_VALUE = 5;
    const int QUEEN_VALUE = 9;
    const int KING_VALUE = 0;

    // Fetching Data
    // 1) move related
    U8 getFromSquare(Move& move);
    U8 getToSquare(Move& move);
    U8 getFlags(Move& move);
    U8 getPromotedPiece(Move& move);
    bool isCaptureMove(Move& move);    
    U8 squareFromNotation(const std::string& square);
    std::string notationFromSquare(Move& move);
    std::string getNotation(U8 square);


    // 2) square related 
    U8 getRank(U8 square);
    U8 getFile(U8 square);
    U8 getPieceColor(U8 square);
    U8 getPieceType(int square);
    bool inPromoPosition(int square);
    bool inStartingPos(int square);

    // 3) board related
    std::string getPromotionNotation(Move& move);
    std::string getFen();
    

    // Handling moves
    Move constructMove(U8 from, U8 to, U8 castleStatus, U8 flags);
    void movePiece(U8 piece, U8 color, U8 from, U8 to); 
    void makeMove(Move &move);
    void undoMove();
    bool move(std::string move);


    // Displaying data
    void displayBitMap(Bitboard map);
    void displayBoard();
    void displayMoves(std::vector<Move> moves);
    

    // Initialising and saving
    void init(std::string fen);
    void initAttackTables();
    void initMoveTables();
    void saveChanges();

    // Bitboard manipulation
    void setBit(Bitboard &map, U8 index);
    void unsetBit(Bitboard &map, U8 index);
    int countBits(Bitboard& map);
    Bitboard getWithSetBit(int square);
    


    // Move generation
    int getPawnMoves(Move* buffer, U8 square);
    int getBishopMoves(Move* buffer, U8 square);
    int getKnightMoves(Move* buffer, U8 square);
    int getRookMoves(Move* buffer, U8 square);
    int getQueenMoves(Move* buffer, U8 square);
    int getKingMoves(Move* buffer, U8 square);
    int GenerateMoves(Move* buffer);
    int GenerateLegalMoves(Move* buffer);


    // Attack detection
    Bitboard rookAttacks(int square, Bitboard occ);
    Bitboard bishopAttacks(int square, Bitboard occ);

    // Validation checks
    bool isInCheck(int kingsqr = -1, bool flip = false);
    bool isLegalMove(U8 from, U8 to, int promotionPiece);
    bool isCheckMate();
    bool isStaleMate();
    U8 isGameOver();

    // Evaluation
    Eval negaMax(int depth, int alpha, int beta); 
    Eval evaluate();
    Bitboard perft(int depth, int* mates, int originalDepth = -1);//, int& mates);


    // ------------------------------------------- JS MODULE -------------------------------------------
    MoveData makeMoveJS(Move move);
    std::vector<Move> GenerateLegalMovesJS();
    std::string getTurn();
    MoveData moveJS(std::string move);

};


