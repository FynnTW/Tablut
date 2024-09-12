#pragma once
#include <string>

constexpr int C_BOARD_SIZE = 9;

using Piece = uint8_t;
using Coord = uint8_t;
using Position = uint8_t;
using Move = uint16_t;

#define GET_X(position) ((position) & 0x0F)
#define GET_Y(position) ((position) >> 4)
#define MAKE_POSITION(x, y) ((x) | ((y) << 4))
#define MAKE_MOVE(from, to) ((from) | ((to) << 8))
#define GET_FROM(move) ((move) & 0xFF)
#define GET_TO(move) ((move) >> 8)
#define IS_CENTER(x, y) ((x) == 4 && (y) == 4)
#define GET_COLOR(piece) ((piece) & 0x03)

enum PieceType
{
    Empty = 0,
    Black = 1 << 0,
    White = 1 << 1,
    King =  1 << 2
};

enum TurnTypes
{
    Black_Turn = 0,
    White_Turn = 1
};

struct CaptureHistory
{
    Piece capturedPiece[4];
    Position captureLocation[4];
};

class Board
{
public:
    Piece gameBoard[9][9];
    Move legalMoves[256];
    CaptureHistory* captureHistory = new CaptureHistory[256];
    uint16_t turn = 0;
    uint8_t turnColor = Black_Turn;
    uint16_t moveCount = 0;
    bool blackWin = false;
    bool whiteWin = false;
    Move bestMove = 0;
    
    void initBoard();
    void play();
    
    void generateLegalMoves(uint8_t turnCol);
    Move* getMoves(Position position, Move* moves) const;
    void movePiece(Move move);
    bool isCapture(Position position);
    void checkCapture(Position captureSquare, Position helpSquare, uint8_t color);
    
    void makeMove(Move move);
    void unmakeMove(Move move);
    
    Move think() const;
    Move thinkSmart();
    int search(int alpha, int beta, uint8_t depth, uint8_t ply);
    int evaluate();
    int countPieces(int color) const;
    
    static std::string moveToString(Move move);
    std::string toString() const;
};
