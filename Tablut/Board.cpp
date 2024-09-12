#include "Board.h"
#include <iostream>
#include <random>

void Board::initBoard()
{
    gameBoard[3][0] = Black;
    gameBoard[4][0] = Black;
    gameBoard[5][0] = Black;
    gameBoard[4][1] = Black;
    
    gameBoard[3][8] = Black;
    gameBoard[4][8] = Black;
    gameBoard[5][8] = Black;
    gameBoard[4][7] = Black;
    
    gameBoard[0][3] = Black;
    gameBoard[0][4] = Black;
    gameBoard[0][5] = Black;
    gameBoard[1][4] = Black;
    
    gameBoard[8][3] = Black;
    gameBoard[8][4] = Black;
    gameBoard[8][5] = Black;
    gameBoard[7][4] = Black;
    
    gameBoard[4][2] = White;
    gameBoard[4][3] = White;
    
    gameBoard[4][4] = White | King;
    
    gameBoard[4][5] = White;
    gameBoard[4][6] = White;
    
    gameBoard[2][4] = White;
    gameBoard[3][4] = White;
    
    gameBoard[5][4] = White;
    gameBoard[6][4] = White;
    generateLegalMoves(turnColor);
}

Move* Board::getMoves(const Position position, Move* moves) const
{
    bool continueNorth = true;
    bool continueSouth = true;
    bool continueEast = true;
    bool continueWest = true;
    const Coord x = GET_X(position);
    const Coord y = GET_Y(position);
    for (int i = 1; i < C_BOARD_SIZE; i++)
    {
        if (continueEast && x + i < 9 && !IS_CENTER(x + i, y) && !gameBoard[x + i][y])
            *moves++ = MAKE_MOVE(position, MAKE_POSITION(x + i, y)); 
        else
            continueEast = false;
        
        if (continueWest && x - i >= 0 && !IS_CENTER(x - i, y) && !gameBoard[x - i][y])
            *moves++ = MAKE_MOVE(position, MAKE_POSITION(x - i, y)); 
        else
            continueWest = false;
        
        if (continueSouth && y + i < 9 && !IS_CENTER(x, y + i) && !gameBoard[x][y + i])
            *moves++ = MAKE_MOVE(position, MAKE_POSITION(x, y + i)); 
        else
            continueSouth = false;
        
        if (continueNorth && y - i >= 0 && !IS_CENTER(x, y - i) && !gameBoard[x][y - i])
            *moves++ = MAKE_MOVE(position, MAKE_POSITION(x, y - i)); 
        else
            continueNorth = false;
    }
    return moves;
}

void Board::movePiece(const Move move)
{
    const Position from = GET_FROM(move);
    const Position to = GET_TO(move);
    gameBoard[GET_X(to)][GET_Y(to)] = gameBoard[GET_X(from)][GET_Y(from)];
    gameBoard[GET_X(from)][GET_Y(from)] = 0;
}

void Board::checkCapture(const Position captureSquare, const Position helpSquare, const uint8_t color)
{
    const Coord capX = GET_X(captureSquare);
    const Coord capY = GET_Y(captureSquare);
    const Coord helpX = GET_X(helpSquare);
    const Coord helpY = GET_Y(helpSquare);
    const bool isKing = gameBoard[capX][capY] & King;
    if (GET_COLOR(gameBoard[capX][capY]) == color || gameBoard[capX][capY] == 0)
        return;
    if ((GET_COLOR(gameBoard[helpX][helpY]) == color) || (color == Black && isKing && IS_CENTER(helpX, helpY)))
    {
        if (isKing)
            blackWin = true;
        for (int i = 0; i < 4; i++)
        {
            if (captureHistory[turn].capturedPiece[i])
                continue;
            captureHistory[turn].capturedPiece[i] = gameBoard[capX][capY];
            captureHistory[turn].captureLocation[i] = captureSquare;
            break;
        }
        gameBoard[capX][capY] = 0;
    }
}

bool Board::isCapture(const Position position)
{
    const Coord x = GET_X(position);
    const Coord y = GET_Y(position);
    const uint8_t color = GET_COLOR(gameBoard[x][y]);
    std::memset(captureHistory[turn].capturedPiece, 0, sizeof(captureHistory[turn].capturedPiece));
    std::memset(captureHistory[turn].captureLocation, 0, sizeof(captureHistory[turn].captureLocation));
    if (x + 2 < C_BOARD_SIZE)
        checkCapture(MAKE_POSITION(x + 1, y), MAKE_POSITION(x + 2, y), color);
    if (x - 2 >= 0)
        checkCapture(MAKE_POSITION(x - 1, y), MAKE_POSITION(x - 2, y), color);
    if (y + 2 < C_BOARD_SIZE)
        checkCapture(MAKE_POSITION(x, y + 1), MAKE_POSITION(x, y + 2), color);
    if (y - 2 >= 0)
        checkCapture(MAKE_POSITION(x, y - 1), MAKE_POSITION(x, y - 2), color);
    return false;
}


void Board::generateLegalMoves(const uint8_t turnCol)
{
    std::memset(legalMoves, 0, sizeof(legalMoves));
    if (whiteWin || blackWin)
        return;
    Move* moveEnd = legalMoves;
    const uint8_t convertedColor = turnCol + 1;
    for (int j = 0; j < C_BOARD_SIZE; j++)
        for (int i = 0; i < C_BOARD_SIZE; i++)
            if (GET_COLOR(gameBoard[i][j]) == convertedColor)
                moveEnd = getMoves(MAKE_POSITION(i, j), moveEnd);
    
    moveCount = moveEnd - legalMoves;  // NOLINT(clang-diagnostic-implicit-int-conversion)
    if (moveCount == 0)
    {
        if (turnCol == Black_Turn)
            whiteWin = true;
        else
            blackWin = true;
    }
}
void Board::makeMove(const Move move)
{
    const Position to = GET_TO(move);
    movePiece(move);
    isCapture(to);
    if (turnColor == White_Turn)
    {
        const Coord toX = GET_X(to);
        const Coord toY = GET_Y(to);
        if (gameBoard[toX][toY] & King && (toX == 0 || toY == 0 || toX == 8 || toY == 8))
            whiteWin = true;
    }
    turnColor = turnColor ^ 1;
    generateLegalMoves(turnColor);
    turn++;
}

void Board::unmakeMove(const Move move)
{
    turn--;
    const Position from = GET_FROM(move);
    const Position to = GET_TO(move);
    gameBoard[GET_X(from)][GET_Y(from)] = gameBoard[GET_X(to)][GET_Y(to)];
    gameBoard[GET_X(to)][GET_Y(to)] = 0;
    for (int i = 0; i < 4; i++)
        if (captureHistory[turn].capturedPiece[i])
            gameBoard[GET_X(captureHistory[turn].captureLocation[i])][GET_Y(captureHistory[turn].captureLocation[i])] = captureHistory[turn].capturedPiece[i];
    
    turnColor = turnColor ^ 1;
    generateLegalMoves(turnColor);
    blackWin = false;
    whiteWin = false;
}

Move Board::think() const
{
    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_int_distribution<> distro(0, moveCount-1);
    return legalMoves[distro(eng)];
}

Move Board::thinkSmart()
{
    search(-1000000, 1000000, 4, 0);
    return bestMove;
}

int Board::search(int alpha, const int beta, const uint8_t depth, const uint8_t ply)
{
    int bestScore = -1000000;

    if (blackWin || whiteWin)
        return -1000000 + ply;
    
    if (depth < 1)
        return evaluate();

    Move moves[256];
    std::memcpy(moves, legalMoves, sizeof(legalMoves));
    for (int i = 0; i < moveCount; i++)
    {
        if (moves[i] == 0)
            continue;
        makeMove(moves[i]);
        const int score = -search(-beta, -alpha, depth - 1, ply + 1);
        unmakeMove(moves[i]);
        if (score > bestScore)
        {
            bestScore = score;
            if (ply == 0)
                bestMove = moves[i];
            if (score > alpha)
            {
                if (score >= beta)
                    break;
                alpha = score;
            }
        }
    }
    return bestScore;
}

int Board::evaluate()
{
    int scoreTurn = 0;
    int scoreOpponent = 0;
    scoreTurn += moveCount + (countPieces(turnColor == Black_Turn ? Black : White) * 8);
    Move* oldMoves[256];
    std::memcpy(oldMoves, legalMoves, sizeof(legalMoves));
    generateLegalMoves(turnColor ^ 1);
    scoreOpponent += moveCount + (countPieces(turnColor == Black_Turn ? White : Black) * 8);
    std::memcpy(legalMoves, oldMoves, sizeof(legalMoves));
    return scoreTurn - scoreOpponent;
}

int Board::countPieces(const int color) const
{
    int count = 0;
    for (int j = 0; j < C_BOARD_SIZE; j++)
        for (int i = 0; i < C_BOARD_SIZE; i++)  // NOLINT(modernize-loop-convert)
            if (GET_COLOR(gameBoard[i][j]) == color)
                count++;
    return count;
}

void Board::play()
{
    initBoard();
    Move lastMove = 0;
    std::cout << toString() << '\n';
    while (!blackWin && !whiteWin)
    {
        bestMove = 0;
        std::string userInputX;
        std::cout << "Enter your move X: ";
        std::getline(std::cin, userInputX);
        if (userInputX == "ai")
        {
            const Move move = thinkSmart();
            std::cout << moveToString(move);
            makeMove(move);
            std::cout << toString() << '\n';
            std::cout << "==============================" << '\n';
            lastMove = move;
            continue;
        }
        if (userInputX == "undo")
        {
            unmakeMove(lastMove);
            continue;
        }
        if (userInputX == "end")
            return;
        const int x = std::stoi(userInputX);
        
        std::string userInputY;
        std::cout << "Enter your move Y: ";
        std::getline(std::cin, userInputY);
        const int y = std::stoi(userInputY);
        
        std::string userInputDestX;
        std::cout << "Enter your move Dest X: ";
        std::getline(std::cin, userInputDestX);
        const int x2 = std::stoi(userInputDestX);
        
        std::string userInputDestY;
        std::cout << "Enter your move Dest Y: ";
        std::getline(std::cin, userInputDestY);
        const int y2 = std::stoi(userInputDestY);
        
        const Move move = MAKE_MOVE(MAKE_POSITION(x, y), MAKE_POSITION(x2, y2));
        makeMove(move);
        lastMove = move;
        std::cout << toString() << '\n';
        std::cout << "==============================" << '\n';
    }
    if (blackWin)
        std::cout << "Game Over blackWin" << '\n';
    if (whiteWin)
        std::cout << "Game Over whiteWin" << '\n';
}


std::string Board::toString() const
{
    std::string result;
    for (int j = 0; j < C_BOARD_SIZE; j++)
    {
        for (int i = 0; i < C_BOARD_SIZE; i++)  // NOLINT(modernize-loop-convert)
        {
            if (gameBoard[i][j] & King)
                result += "K";
            else if (gameBoard[i][j] & Black)
                result += "B";
            else if (gameBoard[i][j] & White)
                result += "W";
            else
                result += ".";
        }
        result += "\n";
    }
    return result;
}

std::string Board::moveToString(const Move move)
{
    const Position from = GET_FROM(move);
    const Position to = GET_TO(move);
    std::string result;
    result += std::to_string(GET_X(from));
    result += " ";
    result += std::to_string(GET_Y(from));
    result += " - ";
    result += std::to_string(GET_X(to));
    result += " ";
    result += std::to_string(GET_Y(to));
    result += "\n";
    return result;
}



