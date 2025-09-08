#include <emscripten/bind.h>
#include "chess.h"


using namespace emscripten;

EMSCRIPTEN_BINDINGS(chess_module) {
;

    value_object<MoveData>("MoveData")
        .field("status", &MoveData::status)
        .field("san", &MoveData::san)
        .field("color", &MoveData::color)
        .field("flags", &MoveData::flags);
    register_vector<Move>("VectorUint16");

    class_<Chess>("Chess")
        .constructor<>()
        .function("init", &Chess::init)
        .function("move", &Chess::moveJS)
        .function("get_piece", &Chess::getPieceType)
        .function("is_checkmate", &Chess::isCheckMate) 
        .function("is_stalemate", &Chess::isStaleMate)
        .function("get_fen", &Chess::getFen)
        .function("get_legal_moves", &Chess::GenerateLegalMovesJS)
        .function("undo_move", &Chess::undoMove)
        .function("get_turn", &Chess::getTurn);
}