#ifndef SEABATTLE_STRUCTS_H
#define SEABATTLE_STRUCTS_H

#include <vector>
#include <set>
#include <Windows.h>

using namespace std;

enum Position {
    HORIZONTAL, VERTICAL
};

struct Ship { //???only pregame
    int width, height; //in pixels for away from window logic
    int type; //1 to 4
    RECT rect; //global coords of ship
    RECT defaultRect; //rect which we get in Generate... method
    BOOL isOnMap; //pregame status + use for oldRect + can use when change place on map
    Position position;
    set<POINT> bannedCells; // means backlighted forbidBrush
    int aliveCount; // game: starts with type, then dec, show all ship if zero
};

inline bool operator<(const POINT& lhs, const POINT& rhs) {
    if (lhs.x == rhs.x) {
        return lhs.y < rhs.y;
    } else {
        return lhs.x < rhs.x;
    }
}

struct DraggedShip {
    int index;
    RECT oldRect;
    set<POINT> oldBannedCells;
    Position position;
    BOOL oldIsOnMap;
    int deltaLeft, deltaTop, deltaRight, deltaBottom; // rect - point: can be negative
};

struct MapCell { //???for in-game mechanics
    RECT rect; //global coords of cell
    int index; // -1 if no ship
    BOOL isAvailable; // -1 + false -> adjacent cell  | game: can shoot
    BOOL isAttempted; // true if was shooted | game: if shooted
    BOOL isVisualized; //pregame prop for red/green squares
    BOOL isPartial; //pregame prop for red square if not full
};

struct CellSquare {
    int square;
    int i, j;
};

//can have counter with accurate shoots for win determination
struct Map {
    RECT coord; //coord of edges
    int side; //side in pixels
    vector<vector<MapCell>> cells; //matrix of cells
};

struct Player {
    Map map;
    vector<Ship> ships;
};

enum GameState {
    PREGAME, GAME
};

enum PlayerType {
    HUMAN, COMPUTER
};

enum Direction {
    LEFT, TOP, RIGHT, BOTTOM
};

struct DecisionBasis {
    bool isFinishOff;
    bool isDirectionFound;
    Direction direction;
    int i, j, shift;
};

struct Stat {
    int bestVictoryTime;
    int wins, games;
};

struct StateInfo {
    int clientWidth, clientHeight;
    GameState gameState;
    Player self, enemy;
    BOOL isDragged;
    DraggedShip draggedShip;
    HWND startButton;
    HWND randomButton;
    vector<vector<RECT>> possiblePlacesForEachShip;
    PlayerType turn;
    UINT_PTR turnTimer;
    vector<POINT> availableForComputerCells;
    bool isGameEnded;
    PlayerType winner;
    HBITMAP background;
    UINT_PTR gameLengthTimer;
    int gameLength; // in secs
    bool isSoundOn;
    DecisionBasis decisionBasis;
    Stat stat;
};

#endif //SEABATTLE_STRUCTS_H
