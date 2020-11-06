#include "../main/config.h"
#include "state_changers.h"

#include <algorithm>

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
    ship.isOnMap = false;
    ship.position = HORIZONTAL;
    ship.height = RECT_SIDE;

    ship.type = 3;
    ship.width = RECT_SIDE * ship.type;
    ship.rect = {x - RECT_SIDE * 7 / 2, y - RECT_SIDE * 3 / 2, x - RECT_SIDE / 2, y - RECT_SIDE / 2};
    ship.defaultRect = ship.rect;
    pState->self.ships.push_back(ship);
    ship.rect = {x + RECT_SIDE / 2, y - RECT_SIDE * 3 / 2, x + RECT_SIDE * 7 / 2, y - RECT_SIDE / 2};
    ship.defaultRect = ship.rect;
    pState->self.ships.push_back(ship);

    ship.type = 2;
    ship.width = RECT_SIDE * ship.type;
    ship.rect = {x - RECT_SIDE, y + RECT_SIDE * 1 / 2, x + RECT_SIDE, y + RECT_SIDE * 3 / 2};
    ship.defaultRect = ship.rect;
    pState->self.ships.push_back(ship);
    ship.rect = {x - RECT_SIDE * 4, y + RECT_SIDE * 1 / 2, x - RECT_SIDE * 2, y + RECT_SIDE * 3 / 2};
    ship.defaultRect = ship.rect;
    pState->self.ships.push_back(ship);
    ship.rect = {x + RECT_SIDE * 2, y + RECT_SIDE * 1 / 2, x + RECT_SIDE * 4, y + RECT_SIDE * 3 / 2};
    ship.defaultRect = ship.rect;
    pState->self.ships.push_back(ship);

    ship.type = 4;
    ship.width = RECT_SIDE * ship.type;
    ship.rect = {x - RECT_SIDE * 2, y - RECT_SIDE * 7 / 2, x + RECT_SIDE * 2, y - RECT_SIDE * 5 / 2};
    ship.defaultRect = ship.rect;
    pState->self.ships.push_back(ship);

    ship.type = 1;
    ship.width = RECT_SIDE * ship.type;
    ship.rect = {x - RECT_SIDE * 3 / 2, y + RECT_SIDE * 5 / 2, x - RECT_SIDE / 2, y + RECT_SIDE * 7 / 2};
    ship.defaultRect = ship.rect;
    pState->self.ships.push_back(ship);
    ship.rect = {x - RECT_SIDE * 7 / 2, y + RECT_SIDE * 5 / 2, x - RECT_SIDE * 5 / 2, y + RECT_SIDE * 7 / 2};
    ship.defaultRect = ship.rect;
    pState->self.ships.push_back(ship);
    ship.rect = {x + RECT_SIDE / 2, y + RECT_SIDE * 5 / 2, x + RECT_SIDE * 3 / 2, y + RECT_SIDE * 7 / 2};
    ship.defaultRect = ship.rect;
    pState->self.ships.push_back(ship);
    ship.rect = {x + RECT_SIDE * 5 / 2, y + RECT_SIDE * 5 / 2, x + RECT_SIDE * 7 / 2, y + RECT_SIDE * 7 / 2};
    ship.defaultRect = ship.rect;
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

vector<RECT> ShipToRects(const Ship& ship) {
    vector<RECT> result;
    if (ship.position == HORIZONTAL) {
        for (int i = 0; i < ship.type; i++) {
            result.push_back({ship.rect.left + RECT_SIDE * i,
                              ship.rect.top,
                              ship.rect.left + RECT_SIDE * (i + 1),
                              ship.rect.bottom
                             });
        }
    } else {
        for (int i = 0; i < ship.type; i++) {
            result.push_back({ship.rect.left,
                              ship.rect.top + RECT_SIDE * i,
                              ship.rect.right,
                              ship.rect.top + RECT_SIDE * (i + 1)
                             });
        }
    }
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
        for (int n = 0; n < COUNT_CELLS_IN_SIDE; n++) {
            pState->self.map.cells[m][n].isVisualized = false;
            pState->self.map.cells[m][n].isPartial = false;

            RECT cellRect = pState->self.map.cells[m][n].rect;

            int index = 0;
            for (auto &shipRect : shipRects) {
                int a = shipRect.bottom - cellRect.top;
                int b = shipRect.right - cellRect.left;
                int c = cellRect.bottom - shipRect.top;
                int d = cellRect.right - shipRect.left;

                if (a >= 0 && a <= RECT_SIDE && b >= 0 && b <= RECT_SIDE) {
                    if (squares[index].square < a * b) {
                        squares[index] = {a * b, m, n};
                    }
                } else if (c >= 0 && c <= RECT_SIDE && b >= 0 && b <= RECT_SIDE) {
                    if (squares[index].square < c * b) {
                        squares[index] = {c * b, m, n};
                    }
                } else if (a >= 0 && a <= RECT_SIDE && d >= 0 && d <= RECT_SIDE) {
                    if (squares[index].square < a * d) {
                        squares[index] = {a * d, m, n};
                    }
                } else if (c >= 0 && c <= RECT_SIDE && d >= 0 && d <= RECT_SIDE) {
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

void MarkCells(StateInfo *pState, const vector<CellSquare> &squares, int i) { // and
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

void BacklightCells(StateInfo *pState, int i) { // i is ship index
    vector<CellSquare> squares = FindCellSquares(pState, i);

    ResolveCollisions(&squares);

    MarkCells(pState, squares, i);
}

void RotateShip(HWND hwnd, StateInfo *pState, int i) { // i is ship index
    POINT point;
    if (GetCursorPos(&point)) {
        ScreenToClient(hwnd, &point);

        swap(pState->self.ships[i].width, pState->self.ships[i].height);

        int tempLeft = pState->draggedShip.deltaLeft;
        pState->draggedShip.deltaLeft = -abs(pState->draggedShip.deltaBottom);
        pState->draggedShip.deltaBottom = abs(pState->draggedShip.deltaRight);
        pState->draggedShip.deltaRight = abs(pState->draggedShip.deltaTop);
        pState->draggedShip.deltaTop = -abs(tempLeft);

        pState->self.ships[i].rect = {
                point.x - abs(pState->draggedShip.deltaLeft),
                point.y - abs(pState->draggedShip.deltaTop),
                point.x + abs(pState->draggedShip.deltaRight),
                point.y + abs(pState->draggedShip.deltaBottom)
        };

        if (pState->self.ships[i].position == HORIZONTAL) {
            pState->self.ships[i].position = VERTICAL;
        } else {
            pState->self.ships[i].position = HORIZONTAL;
        }
    }
}

void GetCellsInfo(StateInfo *pState, vector<POINT> *placementIndexes, vector<RECT> *placementRects) {
    for (int i = 0; i < COUNT_CELLS_IN_SIDE; i++) {
        for (int j = 0; j < COUNT_CELLS_IN_SIDE; j++) {
            if (pState->self.map.cells[i][j].isVisualized) {
                if (pState->self.map.cells[i][j].isAvailable && !pState->self.map.cells[i][j].isPartial) {
                    placementIndexes->push_back({i, j});
                    placementRects->push_back(pState->self.map.cells[i][j].rect);
                }
            }
        }
    }
}

bool CompareLeft(RECT r1, RECT r2) { return r1.left < r2.left; }

bool CompareTop(RECT r1, RECT r2) { return r1.top < r2.top; }

bool CompareRight(RECT r1, RECT r2) { return r1.right > r2.right; }

bool CompareBottom(RECT r1, RECT r2) { return r1.bottom > r2.bottom; }

void SetNewRect(vector<RECT> placementRects, Ship *pDraggedShip) {
    sort(placementRects.begin(), placementRects.end(), CompareLeft);
    pDraggedShip->rect.left = placementRects[0].left;
    sort(placementRects.begin(), placementRects.end(), CompareTop);
    pDraggedShip->rect.top = placementRects[0].top;
    sort(placementRects.begin(), placementRects.end(), CompareRight);
    pDraggedShip->rect.right = placementRects[0].right;
    sort(placementRects.begin(), placementRects.end(), CompareBottom);
    pDraggedShip->rect.bottom = placementRects[0].bottom;
}

void BanCells(StateInfo *pState, const vector<POINT> &placementIndexes, Ship *pDraggedShip) {
    //ship cells
    for (auto &pairedIndex : placementIndexes) {
        pState->self.map.cells[pairedIndex.x][pairedIndex.y].isAvailable = false;
    }

    // adjacent to ship cells
    for (auto &p : placementIndexes) {
        if (p.x - 1 > -1 && p.x - 1 < 10) {
            pState->self.map.cells[p.x - 1][p.y].isAvailable = false;
            pDraggedShip->bannedCells.insert({p.x - 1, p.y});
        }
        if (p.x + 1 > -1 && p.x + 1 < 10) {
            pState->self.map.cells[p.x + 1][p.y].isAvailable = false;
            pDraggedShip->bannedCells.insert({p.x + 1, p.y});
        }
        if (p.y - 1 > -1 && p.y - 1 < 10) {
            pState->self.map.cells[p.x][p.y - 1].isAvailable = false;
            pDraggedShip->bannedCells.insert({p.x, p.y - 1});
        }
        if (p.y + 1 > -1 && p.y + 1 < 10) {
            pState->self.map.cells[p.x][p.y + 1].isAvailable = false;
            pDraggedShip->bannedCells.insert({p.x, p.y + 1});
        }
        if (p.x - 1 > -1 && p.x - 1 < 10 && p.y - 1 > -1 && p.y - 1 < 10) {
            pState->self.map.cells[p.x - 1][p.y - 1].isAvailable = false;
            pDraggedShip->bannedCells.insert({p.x - 1, p.y - 1});
        }
        if (p.x - 1 > -1 && p.x - 1 < 10 && p.y + 1 > -1 && p.y + 1 < 10) {
            pState->self.map.cells[p.x - 1][p.y + 1].isAvailable = false;
            pDraggedShip->bannedCells.insert({p.x - 1, p.y + 1});
        }
        if (p.x + 1 > -1 && p.x + 1 < 10 && p.y - 1 > -1 && p.y - 1 < 10) {
            pState->self.map.cells[p.x + 1][p.y - 1].isAvailable = false;
            pDraggedShip->bannedCells.insert({p.x + 1, p.y - 1});
        }
        if (p.x + 1 > -1 && p.x + 1 < 10 && p.y + 1 > -1 && p.y + 1 < 10) {
            pState->self.map.cells[p.x + 1][p.y + 1].isAvailable = false;
            pDraggedShip->bannedCells.insert({p.x + 1, p.y + 1});
        }
    }
}

void RecoverPreviousState(StateInfo *pState, Ship *pDraggedShip) {
    pDraggedShip->rect = pState->draggedShip.oldRect;
    pDraggedShip->bannedCells = pState->draggedShip.oldBannedCells;
    for (auto &p : pDraggedShip->bannedCells) {
        pState->self.map.cells[p.x][p.y].isAvailable = false;
    }
    pDraggedShip->position = pState->draggedShip.position;
    if (pState->draggedShip.position == HORIZONTAL) {
        pDraggedShip->height = RECT_SIDE;
        pDraggedShip->width = RECT_SIDE * pDraggedShip->type;
    } else {
        pDraggedShip->height = RECT_SIDE * pDraggedShip->type;
        pDraggedShip->width = RECT_SIDE;
    }
}

void PlaceShip(StateInfo *pState) {
    pState->isDragged = FALSE;

    Ship *pDraggedShip = &(pState->self.ships[pState->draggedShip.index]);

    vector<POINT> placementIndexes; // i, j of marked cells
    vector<RECT> placementRects; // rects of marked cells
    GetCellsInfo(pState, &placementIndexes, &placementRects);

    if (!(pDraggedShip->rect.left > pState->self.map.coord.right ||
        pDraggedShip->rect.right < pState->self.map.coord.left ||
        pDraggedShip->rect.top > pState->self.map.coord.bottom ||
        pDraggedShip->rect.bottom < pState->self.map.coord.top)) {
        if (placementRects.size() == pDraggedShip->type) { // when green backlight
            pDraggedShip->isOnMap = true;

            SetNewRect(placementRects, pDraggedShip);

            BanCells(pState, placementIndexes, pDraggedShip);
        } else { // when red backlight
            RecoverPreviousState(pState, pDraggedShip);
        }
    } else {
        pDraggedShip->rect = pDraggedShip->defaultRect;
    }
}