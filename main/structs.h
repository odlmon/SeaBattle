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
    set<POINT> bannedCells; // means backlighted red
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
    int deltaLeft, deltaTop, deltaRight, deltaBottom; // rect - point: can be negative
};

struct MapCell { //???for in-game mechanics
    RECT rect; //global coords of cell
    int type; // 0 if no ship
    BOOL isAvailable; // 0 + false -> adjacent cell
    BOOL isAttempted; // true if was shooted
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

struct StateInfo {
    int clientWidth, clientHeight;
    Player self, enemy;
    BOOL isDragged;
    DraggedShip draggedShip;
};

#endif //SEABATTLE_STRUCTS_H
