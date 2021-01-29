#include <iostream>
#include <random>
#include <emscripten.h>
#include <iostream>
#include <ctime>
#include <ratio>
#include <chrono>

using namespace std;

typedef enum
{
    plain,
    slipery,
    electric,
    water,
    orange,
    hole,
    undecided,
    goal,
} tCell;

char cellToChar[8] = {'#', '+', 'x', '~', '%', ' ', '?', 'G'};

typedef enum
{
    orangeFlavour,
    none
} tFavour;

//  n
//w   e
//  s
typedef enum
{
    north,
    south,
    east,
    west
} tKeypress;

char dirToString[4] = {'^', 'v', '>', '<'};

const int MAX_SIZE = 40;

typedef tCell tRow[MAX_SIZE];

typedef tRow tBoardRaw[MAX_SIZE];

struct tBoard
{
    tBoardRaw cells;
    int height;
    int width;
};

struct tPlayer
{
    int x;
    int y;
    tFavour flavor = none;
};

struct tGameSituation
{
    tPlayer player;
    tBoard board;
};

const int MAX_KEYPRESS = 40;

typedef tKeypress tMovement[MAX_KEYPRESS];

struct tInstructions
{
    tMovement moves;
    tBoard steped;
    int lenght = 0;
    bool valid = false;
};

struct tSolution
{
    tPlayer player;
    tInstructions instructions;
};

const int MAX_SOLUTIONS = 700;

typedef tSolution tSolutionArr[MAX_SOLUTIONS];

struct tSolutionList
{
    tSolutionArr data;
    int beguining = 0;
    int end = 0;
};

// functions:

void swap(int &a, int &b)
{
    int c = a;
    a = b;
    b = c;
}

void printGame(tBoardRaw out, int h, int w, int pX, int pY)
{
    cout << "[" << h << "x" << w << "]" << endl;
    cout << "+";
    for (int i = 0; i < w; i++)
    {
        cout << "-";
    }
    cout << "+" << endl;

    for (int y = 0; y < h; y++)
    {
        cout << "|";
        for (int x = 0; x < w; x++)
        {
            if (pX == x && pY == y)
            {
                cout << "p";
            }
            else
                cout << cellToChar[out[y][x]];
        }
        cout << "|" << endl;
    }

    cout << "+";
    for (int i = 0; i < w; i++)
    {
        cout << "-";
    }
    cout << "+" << endl;
}

void printGame(tGameSituation gs)

{
    if (gs.board.height == 0)
    {
        cout << "jehfñizsdjmnbvwelzaicbweadplñfjbdhjbgf" << endl;
    }
    printGame(gs.board.cells, gs.board.height, gs.board.width, gs.player.x, gs.player.y);
}

void printInstructions(tInstructions instr)
{
    cout << instr.lenght << endl;
    if (true)
    {
        for (int i = 0; i < instr.lenght; i++)
        {
            cout << dirToString[instr.moves[i]] << "-";
        }
        cout << endl;
    }
}

bool playerComparison(tPlayer a, tPlayer b)
{
    return a.flavor == b.flavor && a.x == b.x && a.y == b.y;
}

void insert(tSolutionList &dst, tSolution data, bool &overflow)
{
    //cout << "gona instert" << endl;
    //printGame(data.state);

    for (int i = 0; i < dst.end; i++)
    {
        if (playerComparison(data.player, dst.data[i].player))
        {
            return;
        }
    }

    //cout << "inserted on " << dst.end << " [" << dst.beguining << " - " << dst.end + 1 << "]" << endl;
    dst.data[dst.end] = data;
    dst.end++;
    overflow = dst.end == MAX_SOLUTIONS;

    if (overflow && dst.beguining != 0)
    {
        cout << "reestructuring" << endl;
        overflow = false;

        dst.end -= dst.beguining;
        for (int i = 0; i < dst.end - dst.beguining; i++)
        {
            dst.data[i] = dst.data[i + dst.beguining];
        }
        dst.beguining = 0;
    }
    //cout << "in[" << dst.beguining << " - " << dst.end << "]" << endl;
}

double d(int x, int y, int X, int Y)
{
    return abs(x - X) + abs(y - Y);
}

double distanceToGoal(tPlayer player, tBoard board)
{
    for (int i = 1; i < board.width; i++)
    {
        if (board.cells[board.height - 1][i] == goal)
        {
            return d(player.x, player.y, board.height - 1, i);
        }
    }
    return 1000;
}

bool pop(tSolutionList &src, tSolution &ret, tBoard currentBoard)
{
    if (src.beguining == src.end)
        return false;
    //cout << "pop called, [" << src.beguining << " - " << src.end << "]" << endl;
    if (src.beguining < src.end)
    {
        double retDistance = distanceToGoal(src.data[src.beguining].player, currentBoard);
        ret = src.data[src.beguining];
        int finalSelection = src.beguining;
        for (int i = src.beguining + 1; i < src.end; i++)
        {
            if (retDistance > distanceToGoal(src.data[i].player, currentBoard))
            {
                ret = src.data[i];
                finalSelection = i;
            }
        }
        //cout << retDistance << endl;
        src.data[finalSelection] = src.data[src.beguining];
        src.data[src.beguining] = ret;
        //cout << "poping from " << finalSelection << " [" << src.beguining << " - " << src.end << "]" << endl;
        src.beguining++;
        //cout << "po[" << src.beguining << " - " << src.end << "]" << endl;
    }
    return true;
}

bool electrified(const tPlayer &player, tBoard board)
{
    if (board.cells[player.y + 1][player.x] == electric || board.cells[player.y - 1][player.x] == electric || board.cells[player.y][player.x + 1] == electric || board.cells[player.y][player.x - 1] == electric)
    {
        //cout << " electrified" << endl;
        return true;
    }
    return false;
    //return board.cells[player.y + 1][player.x] == electric || board.cells[player.y - 1][player.x] == electric || board.cells[player.y][player.x + 1] == electric || board.cells[player.y][player.x - 1] == electric;
}

tCell standing(tPlayer player, tBoard board)
{
    return board.cells[player.y][player.x];
}

//  n
//w   e
//  s

bool getAutomaticNext(tSolution &instr, tKeypress dir, bool sliding, tBoard currentBoard)
{
    tSolution back = instr;

    //cout << "calledGetNext with dir: " << dirToString[dir] << endl;
    //printGame(instr.state);
    //printInstructions(instr.instructions);
    if (!sliding)
    {
        instr.instructions.moves[instr.instructions.lenght] = dir;
        instr.instructions.lenght++;
    }
    //cout << "new instructions" << endl;
    //printInstructions(instr.instructions);

    instr.player.x += (dir == east) - (dir == west);
    instr.player.y += (dir == south) - (dir == north);

    //cout << "new board" << endl;
    //printGame(instr.state);

    instr.instructions.steped.cells[instr.player.y][instr.player.x] = undecided;

    tCell standingCell = standing(instr.player, currentBoard);

    if (standingCell == hole || standingCell == electric)
    {
        instr = back;
        //cout << "fell or electrified or something" << endl;
        return false;
    }
    if (standingCell == slipery)
    {
        instr.player.flavor = none;
        //cout << "sliiiiiiide: ";
        return getAutomaticNext(instr, dir, true, currentBoard);
    }
    if (standingCell == orange)
    {
        instr.player.flavor = orangeFlavour;
        ///cout << "orange flavour achieved" << endl;

        return true;
    }
    if (standingCell == water)
    {
        if (instr.player.flavor == orangeFlavour)
        {
            instr = back;
            //    cout << "orange on water" << endl;

            return false;
        }
        if (electrified(instr.player, currentBoard))
        {
            instr = back;
            //      cout << "electrified" << endl;
            return false;
        }
    }
    //cout << "nothing wrong with the move" << endl;
    return true;
}

tInstructions solution(const tGameSituation &gs)
{
    //cout << "empieza la busueda de soluciones" << endl;
    //printGame(gs);
    tSolutionList cola;
    tSolution initial;

    initial.player = gs.player;
    initial.instructions.lenght = 0;
    initial.instructions.valid = false;

    initial.instructions.steped.height = gs.board.height;
    initial.instructions.steped.width = gs.board.width;

    for (int y = 0; y < gs.board.height; y++)
    {
        for (int x = 0; x < gs.board.width; x++)
        {
            initial.instructions.steped.cells[y][x] = hole;
        }
    }

    bool AreThereMisingPaths = true;
    bool overflowDetector = false;
    tSolution current = initial;

    do
    {
        for (int d = 0; d < 4; d++)
        {
            tSolution nextStep = current;
            if (nextStep.instructions.lenght + 1 < MAX_KEYPRESS)
            {
                //cout << "previous intructions:" << endl;
                //printInstructions(nextStep.instructions);
                //cout << "plus " << dirToString[d] << endl;
                if (getAutomaticNext(nextStep, (tKeypress)d, false, gs.board))
                {
                    //cout << "aproved move" << endl;
                    if (standing(nextStep.player, gs.board) == goal)
                    {
                        nextStep.instructions.valid = true;
                        return nextStep.instructions;
                    }
                    insert(cola, nextStep, overflowDetector);
                }
                else
                {
                    //cout << "disaproved move" << endl;
                }
            }
        }

        AreThereMisingPaths = pop(cola, current, gs.board);

        //cout << "missing paths: " << AreThereMisingPaths << endl;
        //cout << "overflowDect : " << overflowDetector << endl;

    } while (AreThereMisingPaths && !overflowDetector);

    if (overflowDetector)
    {
        //cout << "To small of a list" << endl;
    }
    if (!AreThereMisingPaths)
    {
        //cout << "No solution" << endl;
    }
    initial.instructions.valid = false;
    return initial.instructions;
}

/*
    plain,-
    slipery,-
    electric,-
    water,-
    orange,-
    hole,
    undecided,
    goal,

*/

void removeHorizontal(tBoardRaw in, int i)
{
    for (int j = i; j < MAX_SIZE - 1; j++)
    {
        for (int progress = 0; progress < MAX_SIZE; progress++)
        {
            in[j][progress] = in[j + 1][progress];
        }
    }
}

bool emptyHorizontal(tRow in, int w)
{
    for (int i = 0; i < w; i++)
    {
        if (in[i] != hole)
            return false;
    }
    return true;
}

void crop(tGameSituation &gs)
{

    //printGame(gs);

    for (int y = 1; y < gs.board.height - 1; y++)
    {
        if (emptyHorizontal(gs.board.cells[y], gs.board.width))
        {
            removeHorizontal(gs.board.cells, y);
            y--;
            if (y <= gs.player.y)
                gs.player.y--;
            gs.board.height--;
        }
    }
}

void flipBoard(tGameSituation &gs)
{

    tGameSituation ret;
    ret.board.width = gs.board.height;
    ret.board.height = gs.board.width;
    ret.player.x = gs.player.y;
    ret.player.y = gs.player.x;

    for (int y = 0; y < gs.board.height; y++)
    {
        for (int x = 0; x < gs.board.width; x++)
        {
            ret.board.cells[x][y] = gs.board.cells[y][x];
        }
    }

    gs = ret;
}

tGameSituation getRandomBoard(int h, int w)
{
    tGameSituation ret;
    ret.board.height = h;
    ret.board.width = w;

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            if (x == 0 || x == w - 1 || y == 0 || y == h - 1)
            {
                ret.board.cells[y][x] = hole;
            }
            else
            {
                ret.board.cells[y][x] = tCell(rand() % 5);
            }
        }
    }
    // place player

    ret.player.x = 1;
    ret.player.y = 1;
    ret.board.cells[ret.player.y][ret.player.x] = plain;

    // place goal
    int goalY = ret.board.height - 2;
    int goalX = ret.board.width - 2;

    //cout << "goal pos " << ret.board.height-2 << "," << ret.board.width-2 << endl;
    ret.board.cells[goalY][goalX] = goal;

    //cout << "returning " << endl;
    //printGame(ret);

    return ret;
}

int fun(tInstructions instr, tBoard board)
{
    int ret = 0;
    ret += instr.lenght * 2;

    int variety[4] = {0, 0, 0, 0};
    int i = 0;
    int lastInput = -1;
    while (i < instr.lenght)
    {
        if (lastInput != instr.moves[i])
        {
            variety[instr.moves[i]]++;
            lastInput = instr.moves[i];
        }
        i++;
    }
    for (int i = 0; i < 4; i++)
    {
        ret += variety[i] * 10;
    }

    for (int y = 0; y < instr.steped.height; y++)
    {
        for (int x = 0; x < instr.steped.width; x++)
        {
            ret += (instr.steped.cells[y][x] == undecided);
        }
    }

    return ret;
}

int TimeElapsed(chrono::high_resolution_clock::time_point start)
{
    chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
    chrono::duration<double, std::milli> time_span = t2 - start;
    return time_span.count();
}

int diagonalSorund(tBoard in, int x, int y, tCell id)
{
    int ret = 0;
    int deltaX[8] = {1, 1, -1, -1};
    int deltaY[8] = {1, -1, 1, -1};

    for (int i = 0; i < 8; i++)
    {
        if (in.cells[(y + deltaX[i]) % in.height][(x + deltaY[i]) % in.width] == id)
            ret++;
    }
    return ret;
}

int crossSorund(tBoard in, int x, int y, tCell id)
{
    int ret = 0;
    int deltaX[8] = {0, 0, 1, -1};
    int deltaY[8] = {1, -1, 0, 0};

    for (int i = 0; i < 8; i++)
    {
        if (in.cells[(y + deltaX[i]) % in.height][(x + deltaY[i]) % in.width] == id)
            ret++;
    }
    return ret;
}

bool sorund(tBoard in, int x, int y, tCell id)
{
    return diagonalSorund(in, x, y, id) > 0 || crossSorund(in, x, y, id) > 0;
}

void start(tBoard &in, int h, int w, tCell cell)
{
    in.height = h;
    in.width = w;
    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            in.cells[y][x] = cell;
        }
    }
}

void posprocess(tGameSituation &gs, tInstructions &sol)
{

    tBoard antiErodeBuff = sol.steped;

    cout << "initial antierode" << endl;
    printGame(antiErodeBuff.cells, sol.steped.height, sol.steped.width, -1, -1);

    // full antierode

    for (int growCycle = 0; growCycle < 1; growCycle++)
    {
        tBoard nextAntiErodeBuff;
        start(nextAntiErodeBuff, sol.steped.height, sol.steped.width, hole);
        for (int y = 0; y < gs.board.height; y++)
        {
            for (int x = 0; x < gs.board.width; x++)
            {
                if (antiErodeBuff.cells[y][x] == undecided || sorund(antiErodeBuff, x, y, undecided))
                {
                    nextAntiErodeBuff.cells[y][x] = undecided;
                }
            }
        }
        antiErodeBuff = nextAntiErodeBuff;
    }

    cout << "diagonal+adjacent antierode" << endl;
    printGame(antiErodeBuff.cells, sol.steped.height, sol.steped.width, -1, -1);

    // cross antierode

    for (int growCycle = 0; growCycle < 1; growCycle++)
    {
        tBoard nextAntiErodeBuff;
        start(nextAntiErodeBuff, sol.steped.height, sol.steped.width, hole);

        for (int y = 0; y < gs.board.height; y++)
        {
            for (int x = 0; x < gs.board.width; x++)
            {
                if (antiErodeBuff.cells[y][x] == undecided || crossSorund(antiErodeBuff, x, y, undecided) > 1)
                {
                    nextAntiErodeBuff.cells[y][x] = undecided;
                }
            }
        }
        antiErodeBuff = nextAntiErodeBuff;
    }

    cout << "total antierode" << endl;
    printGame(antiErodeBuff.cells, sol.steped.height, sol.steped.width, -1, -1);

    for (int y = 0; y < gs.board.height; y++)
    {
        for (int x = 0; x < gs.board.width; x++)
        {
            if (antiErodeBuff.cells[y][x] != undecided)
            {
                gs.board.cells[y][x] = hole;
            }
        }
    }
    crop(gs);
    flipBoard(gs);
    crop(gs);
    flipBoard(gs);
}

int minH = 10;
int minW = 10;
int TimeFlexibility = 1000;

void EMSCRIPTEN_KEEPALIVE JSconfig(int h, int w, int t)
{
    //cout << "old config: " << minH << " | " << minW << " | " << TimeFlexibility << endl;
    minH = h;
    minW = w;
    TimeFlexibility = t;
    //cout << "new config: " << minH << " | " << minW << " | " << TimeFlexibility << endl;
}

tInstructions chossenInstr;
tGameSituation chosenGs;
int chosenPuntuation;
int transmisionState = 0;
int transmitionX = 0;
int transmitionY = 0;
int solPos = 0;
int EMSCRIPTEN_KEEPALIVE JSgetBoard()
{
    if (transmisionState == 0)
    {
        transmisionState = 1;
        return chosenGs.board.height;
    }
    if (transmisionState == 1)
    {
        transmisionState = 2;
        return chosenGs.board.width;
    }
    if (transmisionState == 2)
    {
        int ret = chosenGs.board.cells[transmitionY][transmitionX];
        transmitionX++;
        if (transmitionX == chosenGs.board.width)
        {
            transmitionX = 0;
            transmitionY++;
            if (transmitionY == chosenGs.board.height)
            {
                transmisionState = 3;
            }
        }
        return ret;
    }
    if (transmisionState == 3)
    {
        transmisionState = 4;
        return chossenInstr.lenght;
    }
    if (transmisionState == 4)
    {
        int ret = chossenInstr.moves[solPos];
        solPos++;
        if (solPos == chossenInstr.lenght)
        {
            transmisionState = 5;
        }
        return ret;
    }
    if (transmisionState == 5)
    {
        transmisionState = -1;
        return chosenPuntuation;
    }
    return -1;
}

void EMSCRIPTEN_KEEPALIVE JSsync()
{
    transmisionState = 0;
    transmitionX = 0;
    transmitionY = 0;
    solPos = 0;
}

void EMSCRIPTEN_KEEPALIVE JSnewBoard()
{
    int choosenScore = -1;

    chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();
    do
    {
        tInstructions instr;
        tGameSituation gs;
        do
        {
            gs = getRandomBoard(minH, minW);
            instr = solution(gs);
            //cout << ".";
        } while (!instr.valid);
        //cout << "found new solvable" << endl;
        int newScore = fun(instr, gs.board);
        if (newScore > choosenScore)
        {
            chosenGs = gs;
            chossenInstr = instr;
            choosenScore = newScore;
            //cout << "best so far got a " << choosenScore << endl;
        }

    } while (TimeElapsed(t1) < TimeFlexibility);
    chosenPuntuation = choosenScore;
}

int main()
{
    //cout << "End of compilation" << endl;
    srand(time(0));

    return 0;
}