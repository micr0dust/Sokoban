#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include "protocols.h"
#include "mysocket.h"

using std::cout;
using std::istringstream;
using std::ostringstream;
using std::string;
using std::vector;

#define EMPTY ' '
#define PLAYER 'W'
#define BOX 'B'
#define WALL 'H'

#define INIT_level -1

int debugCount = 0;
string debug = "";

Endpoint* ep;

enum Direction
{
    LEFT,
    UP,
    DOWN,
    RIGHT
};

class CBlock
{
public:
    char value;
    bool dest;
    CBlock(char c = ' ', bool d = false) : value(c), dest(d) {};
};

class CMap
{
    friend int showMap();

public:
    int level;

    CMap(int init_level = INIT_level) : level(init_level),
        x(-1), y(-1),
        destAmount(0),
        completed(0),
        boxAmount(0),
        step(0),
        err(false) {};

    void newLineAdd();
    void lineBlockAdd(int line, char block);
    void show();
    char get(int dy, int dx);
    void moveFn(Direction d, bool push);
    int completedCount();
    int nextLevel();
    bool MapVarify();
    void statusBar();
    bool isEnd();
    bool err;

private:
    vector<
        vector<CBlock>

    >
        map;
    int x, y;
    int destAmount, completed, boxAmount, step;
};

CMap g_map;

bool fileError(string err = "error")
{
    //g_map.nextLevel();
    g_map.err = true;
    return false;
}

void CMap::newLineAdd()
{
    map.push_back(vector<CBlock>());
}

void CMap::lineBlockAdd(int line, char block)
{
    bool isdest = (block == 'C' || block == 'D') ? true : false;
    if (isdest)
        destAmount++;
    if (block == '0' || block == 'D')
        block = EMPTY;
    else if (block == 'C')
        block = BOX;
    if (block == 'B')
        boxAmount++;
    if (block == '\t')
    {
        block = EMPTY;
        for (int i = 0; i < 6; i++)
            map[line].push_back(CBlock(block, isdest));
    }
    map[line].push_back(CBlock(block, isdest));
}

char encoder(CBlock block) {
    if (!block.dest)
        return block.value;
    if (block.value == 'W')
        return 'w';
    if (block.value == 'B')
        return 'C';
    if (block.value == ' ')
        return 'D';
}

void CMap::show()
{
    string res;
    for (int i = 0; i < map.size(); i++) {
        for (int j = 0; j < map[i].size(); j++)
        {
            if (map[i][j].value == PLAYER)
            {
                x = j;
                y = i;
            }
            res += encoder(map[i][j]);
        }
        res += 'N';
    }

    //debug=res;
    (*ep).Write(res);
    res = "";
}

int CMap::completedCount()
{
    completed = 0;
    for (int i = 0; i < map.size(); i++)
        for (int j = 0; j < map[i].size(); j++)
            if (map[i][j].dest && map[i][j].value == BOX)
                completed++;
    return completed;
}

bool CMap::MapVarify()
{
    if (boxAmount < destAmount || boxAmount == 0)
        return fileError("This map don't have enough box.");
    if (x == -1 || y == -1)
        return fileError("The worker seem disappear in map.");
    return true;
}

int ReadMap(int n)
{
    if (n < 0 || n > 150)
        return fileError("Only accept map000 to map150");
    char filename[15];
    sprintf_s(filename, "map/map%03d.txt", n);
    FILE* fp;
    errno_t err = fopen_s(&fp, filename, "r");

    char word;
    if (fp == NULL)
        return fileError("file not found. Press any key to quit.");
    else
    {
        g_map.newLineAdd();
        for (int i = 0; (word = fgetc(fp)) != EOF;)
        {
            if (word == '\n')
            {
                g_map.newLineAdd();
                i++;
            }
            else if (
                word == 'H' ||
                word == '0' ||
                word == ' ' ||
                word == '\t' ||
                word == 'B' ||
                word == 'C' ||
                word == 'D' ||
                word == 'W')
                g_map.lineBlockAdd(i, word);
            else if (word != '\r')
                return fileError("Invalid syntax in map.");
        }

        fclose(fp);
        return 1;
    }
}

void CMap::statusBar()
{
    this->completedCount();
    char res[50];
    sprintf_s(res, "%d %d %d %d", level, destAmount, completed, step);
    (*ep).Write(res);
}

int showMap()
{
    g_map.show();
    g_map.statusBar();
    return 0;
}

char CMap::get(int dy, int dx)
{
    if (y + dy >= map.size() || y + dy < 0)
        return WALL;
    if (x + dx >= map[y + dy].size() || x + dx < 0)
        return WALL;
    return map[y + dy][x + dx].value;
}

char NextToWorker(Direction d, int n)
{
    if (d == LEFT)
        return g_map.get(0, -n);
    if (d == UP)
        return g_map.get(-n, 0);
    if (d == DOWN)
        return g_map.get(n, 0);
    if (d == RIGHT)
        return g_map.get(0, n);
}

void CMap::moveFn(Direction d, bool push)
{
    step++;
    if (push)
    {
        if (d == LEFT)
            map[y][x - 2].value = map[y][x - 1].value;
        if (d == UP)
            map[y - 2][x].value = map[y - 1][x].value;
        if (d == DOWN)
            map[y + 2][x].value = map[y + 1][x].value;
        if (d == RIGHT)
            map[y][x + 2].value = map[y][x + 1].value;
    }
    if (d == LEFT)
        map[y][x - 1].value = map[y][x].value;
    if (d == UP)
        map[y - 1][x].value = map[y][x].value;
    if (d == DOWN)
        map[y + 1][x].value = map[y][x].value;
    if (d == RIGHT)
        map[y][x + 1].value = map[y][x].value;
    map[y][x].value = EMPTY;
}

int MoveWorker(Direction d)
{
    char next = NextToWorker(d, 1);
    char nextNext = NextToWorker(d, 2);
    if (next == BOX && nextNext == EMPTY)
        g_map.moveFn(d, true);
    if (next == EMPTY)
        g_map.moveFn(d, false);
    return showMap();
}

int CMap::nextLevel()
{
    level++;
    // if (level > 150)
    //     congratulation();
    destAmount = 0;
    boxAmount = 0;
    completed = 0;
    step = 0;
    map.clear();
    ReadMap(level);
    MapVarify();
    if (!err && level < 150) return nextLevel();
    showMap();
    return 0;
}

bool CMap::isEnd()
{
    if (completed >= destAmount)
        return true;
    return false;
}

int controlFn(char req)
{
    if (g_map.isEnd())
        return g_map.nextLevel();
    if (req == '0')
        return MoveWorker(LEFT);
    if (req == '1')
        return MoveWorker(UP);
    if (req == '2')
        return MoveWorker(DOWN);
    if (req == '3')
        return MoveWorker(RIGHT);
    /* x:goto last level, x:goto next level */
    if (g_map.level < 150 && req == 'x')
        return g_map.nextLevel();
    if (req == 'z')
    {
        if ((g_map.level -= 2) <= 0)
            g_map.level = -1;
        return g_map.nextLevel();
    }
    return showMap();
}

int main()
{
    string ip, port;
    cout << "IPV4位址：";
    std::cin >> ip;
    cout << "PORT埠號：";
    std::cin >> port;
    cout << "\n\n";
    cout << "  /$$$$$$            /$$                 /$$\n";
    cout << " /$$__  $$          | $$                | $$\n";
    cout << "| $$  \\__/  /$$$$$$ | $$   /$$  /$$$$$$ | $$$$$$$   /$$$$$$  /$$$$$$$\n";
    cout << "|  $$$$$$  /$$__  $$| $$  /$$/ /$$__  $$| $$__  $$ |____  $$| $$__  $$\n";
    cout << " \\____  $$| $$  \\ $$| $$$$$$/ | $$  \\ $$| $$  \\ $$  /$$$$$$$| $$  \\ $$\n";
    cout << " /$$  \\ $$| $$  | $$| $$_  $$ | $$  | $$| $$  | $$ /$$__  $$| $$  | $$\n";
    cout << "|  $$$$$$/|  $$$$$$/| $$ \\  $$|  $$$$$$/| $$$$$$$/|  $$$$$$$| $$  | $$\n";
    cout << " \\______/  \\______/ |__/  \\__/ \\______/ |_______/  \\_______/|__/  |__/\n";
    cout << "                                                 Server is running ...\n";
    static Endpoint epc(TCP | SERVER, ip, port);
    ep = &epc;
    string req;
    (*ep).Read(-1, req);
    g_map.nextLevel();

    while (debug == "")
    {
        (*ep).Read(-1, req);
        controlFn(req[0]);
        req = "";
    }
    cout << debug;
    return 0;
}