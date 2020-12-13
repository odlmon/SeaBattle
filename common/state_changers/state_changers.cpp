#include "../../main/config.h"
#include "state_changers.h"

void InitializeMap(StateInfo *pState, PlayerType playerType) {
    Player* pPlayer = (playerType == HUMAN) ? &(pState->self) : &(pState->enemy);

    if (playerType == HUMAN) {
        pPlayer->map = Map{{}, RECT_SIDE * COUNT_CELLS_IN_SIDE};
        pPlayer->map.coord = {
                pState->clientWidth / 4 - pPlayer->map.side / 2,
                pState->clientHeight / 2 - pPlayer->map.side / 2,
                pState->clientWidth / 4 + pPlayer->map.side / 2,
                pState->clientHeight / 2 + pPlayer->map.side / 2,
        };

        vector<MapCell> row;
        row.reserve(COUNT_CELLS_IN_SIDE);
        for (int i = 0; i < COUNT_CELLS_IN_SIDE; i++) {
            for (int j = 0; j < COUNT_CELLS_IN_SIDE; j++) {
                row.push_back({{
                                       pPlayer->map.coord.left + RECT_SIDE * j,
                                       pPlayer->map.coord.top + RECT_SIDE * i,
                                       pPlayer->map.coord.left + RECT_SIDE * (j + 1),
                                       pPlayer->map.coord.top + RECT_SIDE * (i + 1)
                               }, 0, true, false, false});
            }
            pPlayer->map.cells.push_back(row);
            row.clear();
        }
    } else {
        pPlayer->map = Map{{}, RECT_SIDE * COUNT_CELLS_IN_SIDE};
        pPlayer->map.coord = {
                pState->clientWidth * 3 / 4 - pPlayer->map.side / 2,
                pState->clientHeight / 2 - pPlayer->map.side / 2,
                pState->clientWidth * 3 / 4 + pPlayer->map.side / 2,
                pState->clientHeight / 2 + pPlayer->map.side / 2,
        };

        vector<MapCell> row;
        row.reserve(COUNT_CELLS_IN_SIDE);
        for (int i = 0; i < COUNT_CELLS_IN_SIDE; i++) {
            for (int j = 0; j < COUNT_CELLS_IN_SIDE; j++) {
                row.push_back({{
                                       pPlayer->map.coord.left + RECT_SIDE * j,
                                       pPlayer->map.coord.top + RECT_SIDE * i,
                                       pPlayer->map.coord.left + RECT_SIDE * (j + 1),
                                       pPlayer->map.coord.top + RECT_SIDE * (i + 1)
                               }, 0, true, false, false});
            }
            pPlayer->map.cells.push_back(row);
            row.clear();
        }
    }
}

void GenerateShipsPlace(StateInfo *pState, PlayerType playerType) {
    Player* pPlayer = (playerType == HUMAN) ? &(pState->self) : &(pState->enemy);

    int x = pState->clientWidth * 3 / 4;
    int y = pState->clientHeight / 2;

    Ship ship = {};
    ship.isOnMap = false;
    ship.position = HORIZONTAL;
    ship.height = RECT_SIDE;

    ship.type = 4;
    ship.width = RECT_SIDE * ship.type;
    ship.rect = {x - RECT_SIDE * 2, y - RECT_SIDE * 7 / 2, x + RECT_SIDE * 2, y - RECT_SIDE * 5 / 2};
    ship.defaultRect = ship.rect;
    pPlayer->ships.push_back(ship);

    ship.type = 3;
    ship.width = RECT_SIDE * ship.type;
    ship.rect = {x - RECT_SIDE * 7 / 2, y - RECT_SIDE * 3 / 2, x - RECT_SIDE / 2, y - RECT_SIDE / 2};
    ship.defaultRect = ship.rect;
    pPlayer->ships.push_back(ship);
    ship.rect = {x + RECT_SIDE / 2, y - RECT_SIDE * 3 / 2, x + RECT_SIDE * 7 / 2, y - RECT_SIDE / 2};
    ship.defaultRect = ship.rect;
    pPlayer->ships.push_back(ship);

    ship.type = 2;
    ship.width = RECT_SIDE * ship.type;
    ship.rect = {x - RECT_SIDE, y + RECT_SIDE * 1 / 2, x + RECT_SIDE, y + RECT_SIDE * 3 / 2};
    ship.defaultRect = ship.rect;
    pPlayer->ships.push_back(ship);
    ship.rect = {x - RECT_SIDE * 4, y + RECT_SIDE * 1 / 2, x - RECT_SIDE * 2, y + RECT_SIDE * 3 / 2};
    ship.defaultRect = ship.rect;
    pPlayer->ships.push_back(ship);
    ship.rect = {x + RECT_SIDE * 2, y + RECT_SIDE * 1 / 2, x + RECT_SIDE * 4, y + RECT_SIDE * 3 / 2};
    ship.defaultRect = ship.rect;
    pPlayer->ships.push_back(ship);

    ship.type = 1;
    ship.width = RECT_SIDE * ship.type;
    ship.rect = {x - RECT_SIDE * 3 / 2, y + RECT_SIDE * 5 / 2, x - RECT_SIDE / 2, y + RECT_SIDE * 7 / 2};
    ship.defaultRect = ship.rect;
    pPlayer->ships.push_back(ship);
    ship.rect = {x - RECT_SIDE * 7 / 2, y + RECT_SIDE * 5 / 2, x - RECT_SIDE * 5 / 2, y + RECT_SIDE * 7 / 2};
    ship.defaultRect = ship.rect;
    pPlayer->ships.push_back(ship);
    ship.rect = {x + RECT_SIDE / 2, y + RECT_SIDE * 5 / 2, x + RECT_SIDE * 3 / 2, y + RECT_SIDE * 7 / 2};
    ship.defaultRect = ship.rect;
    pPlayer->ships.push_back(ship);
    ship.rect = {x + RECT_SIDE * 5 / 2, y + RECT_SIDE * 5 / 2, x + RECT_SIDE * 7 / 2, y + RECT_SIDE * 7 / 2};
    ship.defaultRect = ship.rect;
    pPlayer->ships.push_back(ship);
}