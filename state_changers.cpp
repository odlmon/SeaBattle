#include "config.h"
#include "state_changers.h"

void InitializeMap(StateInfo *pState) {
    pState->self.map = Map{{}, RECT_SIDE * COUNT_CELLS_IN_SIDE};
    pState->self.map.coord = {
            pState->clientWidth / 4 - pState->self.map.side / 2,
            pState->clientHeight / 2 - pState->self.map.side / 2,
            pState->clientWidth / 4 + pState->self.map.side / 2,
            pState->clientHeight / 2 + pState->self.map.side / 2,
    };

    vector<MapCell> row;
    row.reserve(COUNT_CELLS_IN_SIDE);
    for (int i = 0; i < COUNT_CELLS_IN_SIDE; i++) {
        for (int j = 0; j < COUNT_CELLS_IN_SIDE; j++) {
            row.push_back({{
                                   pState->self.map.coord.left + RECT_SIDE * j,
                                   pState->self.map.coord.top + RECT_SIDE * i,
                                   pState->self.map.coord.left + RECT_SIDE * (j + 1),
                                   pState->self.map.coord.top + RECT_SIDE * (i + 1)
                           }, 0, true, false, false});
        }
        pState->self.map.cells.push_back(row);
        row.clear();
    }
}

void GenerateShipsPlace(StateInfo *pState) {
    int x = pState->clientWidth * 3 / 4;
    int y = pState->clientHeight / 2;

    Ship ship = {};
    ship.height = RECT_SIDE;

    ship.type = 3;
    ship.width = RECT_SIDE * ship.type;
    ship.rect = {x - RECT_SIDE * 7 / 2, y - RECT_SIDE * 3 / 2, x - RECT_SIDE / 2, y - RECT_SIDE / 2};
    pState->self.ships.push_back(ship);
    ship.rect = {x + RECT_SIDE / 2, y - RECT_SIDE * 3 / 2, x + RECT_SIDE * 7 / 2, y - RECT_SIDE / 2};
    pState->self.ships.push_back(ship);

    ship.type = 2;
    ship.width = RECT_SIDE * ship.type;
    ship.rect = {x - RECT_SIDE, y + RECT_SIDE * 1 / 2, x + RECT_SIDE, y + RECT_SIDE * 3 / 2};
    pState->self.ships.push_back(ship);
    ship.rect = {x - RECT_SIDE * 4, y + RECT_SIDE * 1 / 2, x - RECT_SIDE * 2, y + RECT_SIDE * 3 / 2};
    pState->self.ships.push_back(ship);
    ship.rect = {x + RECT_SIDE * 2, y + RECT_SIDE * 1 / 2, x + RECT_SIDE * 4, y + RECT_SIDE * 3 / 2};
    pState->self.ships.push_back(ship);

    ship.type = 4;
    ship.width = RECT_SIDE * ship.type;
    ship.rect = {x - RECT_SIDE * 2, y - RECT_SIDE * 7 / 2, x + RECT_SIDE * 2, y - RECT_SIDE * 5 / 2};
    pState->self.ships.push_back(ship);

    ship.type = 1;
    ship.width = RECT_SIDE * ship.type;
    ship.rect = {x - RECT_SIDE * 3 / 2, y + RECT_SIDE * 5 / 2, x - RECT_SIDE / 2, y + RECT_SIDE * 7 / 2};
    pState->self.ships.push_back(ship);
    ship.rect = {x - RECT_SIDE * 7 / 2, y + RECT_SIDE * 5 / 2, x - RECT_SIDE * 5 / 2, y + RECT_SIDE * 7 / 2};
    pState->self.ships.push_back(ship);
    ship.rect = {x + RECT_SIDE / 2, y + RECT_SIDE * 5 / 2, x + RECT_SIDE * 3 / 2, y + RECT_SIDE * 7 / 2};
    pState->self.ships.push_back(ship);
    ship.rect = {x + RECT_SIDE * 5 / 2, y + RECT_SIDE * 5 / 2, x + RECT_SIDE * 7 / 2, y + RECT_SIDE * 7 / 2};
    pState->self.ships.push_back(ship);
}

void UpdateShipRect(StateInfo *pState, int x, int y, int i) { // i is ship index
    pState->self.ships[i].rect = {
            x + pState->draggedShip.deltaLeft,
            y + pState->draggedShip.deltaTop,
            x + pState->draggedShip.deltaRight,
            y + pState->draggedShip.deltaBottom
    };

    if (pState->self.ships[i].rect.right > pState->clientWidth) {
        pState->self.ships[i].rect.right = pState->clientWidth;
        pState->self.ships[i].rect.left = pState->self.ships[i].rect.right - pState->self.ships[i].width;
    }
    if (pState->self.ships[i].rect.left < 0) {
        pState->self.ships[i].rect.left = 0;
        pState->self.ships[i].rect.right = pState->self.ships[i].width;
    }
    if (pState->self.ships[i].rect.top < 0) {
        pState->self.ships[i].rect.top = 0;
        pState->self.ships[i].rect.bottom = pState->self.ships[i].height;
    }
    if (pState->self.ships[i].rect.bottom > pState->clientHeight) {
        pState->self.ships[i].rect.bottom = pState->clientHeight;
        pState->self.ships[i].rect.top = pState->clientHeight - pState->self.ships[i].height;
    }
}

vector<RECT> ShipToRects(Ship ship) {
    //add logic for vertical
    vector<RECT> result;
    //if horizontal
    for (int i = 0; i < ship.type; i++) {
        result.push_back({
                                 ship.rect.left + RECT_SIDE * i,
                                 ship.rect.top,
                                 ship.rect.left + RECT_SIDE * (i + 1),
                                 ship.rect.bottom
                         });
    }
    //if vertical
//    for (int i = 0; i < ship.type; i++) {
//        result.push_back({
//            ship.rect.left,
//            ship.rect.top + RECT_SIDE * i,
//            ship.rect.right,
//            ship.rect.top + RECT_SIDE * (i + 1)
//        });
//    }
    return result;
}

vector<CellSquare> FindCellSquares(StateInfo *pState, int i) {
    vector<CellSquare> squares;
    squares.reserve(pState->self.ships[i].type);
    for (int j = 0; j < pState->self.ships[i].type; j++) {
        squares.push_back({0, 0, 0});
    }

    vector<RECT> shipRects = ShipToRects(pState->self.ships[i]);
    for (int m = 0; m < COUNT_CELLS_IN_SIDE; m++) {
        for (int n = 0; n < COUNT_CELLS_IN_SIDE; n++) { // find 3 max squares
            pState->self.map.cells[m][n].isVisualized = false;
            pState->self.map.cells[m][n].isPartial = false;

            RECT cellRect = pState->self.map.cells[m][n].rect;

            int index = 0;
            for (auto &shipRect : shipRects) {
                int a = shipRect.bottom - cellRect.top;
                int b = shipRect.right - cellRect.left;
                int c = cellRect.bottom - shipRect.top;
                int d = cellRect.right - shipRect.left;

                if (a > 0 && a < RECT_SIDE && b > 0 && b < RECT_SIDE) {
                    if (squares[index].square < a * b) {
                        squares[index] = {a * b, m, n};
                    }
                } else if (c > 0 && c < RECT_SIDE && b > 0 && b < RECT_SIDE) {
                    if (squares[index].square < c * b) {
                        squares[index] = {c * b, m, n};
                    }
                } else if (a > 0 && a < RECT_SIDE && d > 0 && d < RECT_SIDE) {
                    if (squares[index].square < a * d) {
                        squares[index] = {a * d, m, n};
                    }
                } else if (c > 0 && c < RECT_SIDE && d > 0 && d < RECT_SIDE) {
                    if (squares[index].square < c * d) {
                        squares[index] = {c * d, m, n};
                    }
                }
                index++;
            }
        }
    }

    return squares;
}

void ResolveCollisions(vector<CellSquare> *cellSquares) {
    for (int i = 0; i < cellSquares->size(); i++) {
        if ((*cellSquares)[i].square != 0) {
            for (int j = i + 1; j < cellSquares->size(); j++) {
                if ((*cellSquares)[i].i == (*cellSquares)[j].i && (*cellSquares)[i].j == (*cellSquares)[j].j) {
                    if ((*cellSquares)[i].square < (*cellSquares)[j].square) {
                        (*cellSquares)[i] = {0, 0, 0};
                    } else {
                        (*cellSquares)[j] = {0, 0, 0};
                    }
                }
            }
        }
    }
}

bool NotZeros(vector<CellSquare> cellSquares, int *count) {
    int i = 0;
    *count = 0;
    while (i < cellSquares.size()) {
        if (cellSquares[i++].square != 0) {
            (*count)++;
        }
    }
    return (*count) != 0;
}

void MarkCells(StateInfo *pState, const vector<CellSquare>& squares, int i) {
    int count;
    if (NotZeros(squares, &count)) {
        if (count == pState->self.ships[i].type) {
            for (auto &square : squares) {
                pState->self.map.cells[square.i][square.j].isVisualized = true;
            }
        } else {
            for (auto &square : squares) {
                if (square.square != 0) {
                    pState->self.map.cells[square.i][square.j].isVisualized = true;
                    pState->self.map.cells[square.i][square.j].isPartial = true;
                }
            }
        }
    }
}

void PrepareCellsForBacklighting(StateInfo *pState, int i) { // i is ship index
    vector<CellSquare> squares = FindCellSquares(pState, i);

    ResolveCollisions(&squares);

    MarkCells(pState, squares, i);
}