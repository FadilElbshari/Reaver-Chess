#include <emscripten/bind.h>
#include "chess.h"


using namespace emscripten;

EMSCRIPTEN_BINDINGS(chess_module) {
    class_<Chess>("Chess")
        .constructor<>()
        .function("init", &Chess::init)
        .function("make_move", &Chess::makeMoveJS)
        .function("get_piece", &Chess::getPieceType)
        .function("is_checkmate", &Chess::isCheckMate)
        .function("get_fen", &Chess::getFen);
}