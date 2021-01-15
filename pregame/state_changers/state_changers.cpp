#include "../../main/config.h"
#include "state_changers.h"
#include "../../common/state_changers/state_changers.h"

#include <algorithm>
#include <ctime>

void InitializeDraggedShip(StateInfo *pState, const Ship& ship, int index, int x, int y) { // means dragged ship index
    pState->isDragged = true;
    pState->draggedShip = {
            index,
            ship.rect,
            pState->self.ships[index].bannedCells,
            pState->self.ships[index].position,
            pState->self.ships[index].isOnMap,
            ship.rect.left - x,
            ship.rect.top - y,
            ship.rect.right - x,
            ship.rect.bottom - y
    };
}

void UnbanCells(StateInfo *pState, int index) { // means dragged ship index
    for (auto & p : pState->self.ships[index].bannedCells) {
        bool isFound = false;
        for (int i = 0; i < pState->self.ships.size(); i++) {
            if (i != index) {
                if (pState->self.ships[i].bannedCells.find(p) != pState->self.ships[i].bannedCells.end()) {
                    isFound = true;
                }
            }
        }
        if (!isFound) {
            pState->self.map.cells[p.x][p.y].isAvailable = true;
        }
    }
    pState->self.ships[index].bannedCells.clear();
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
    shipRects.clear();

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

void BacklightCells(StateInfo *pState, int shipIndex) {
    vector<CellSquare> squares = FindCellSquares(pState, shipIndex);
    ResolveCollisions(&squares);
    MarkCells(pState, squares, shipIndex);
}

void RotateShip(HWND hwnd, StateInfo *pState, int shipIndex) {
    POINT point;
    if (GetCursorPos(&point)) {
        ScreenToClient(hwnd, &point);

        swap(pState->self.ships[shipIndex].width, pState->self.ships[shipIndex].height);

        int tempLeft = pState->draggedShip.deltaLeft;
        pState->draggedShip.deltaLeft = -abs(pState->draggedShip.deltaBottom);
        pState->draggedShip.deltaBottom = abs(pState->draggedShip.deltaRight);
        pState->draggedShip.deltaRight = abs(pState->draggedShip.deltaTop);
        pState->draggedShip.deltaTop = -abs(tempLeft);

        pState->self.ships[shipIndex].rect = {
                point.x - abs(pState->draggedShip.deltaLeft),
                point.y - abs(pState->draggedShip.deltaTop),
                point.x + abs(pState->draggedShip.deltaRight),
                point.y + abs(pState->draggedShip.deltaBottom)
        };

        if (pState->self.ships[shipIndex].position == HORIZONTAL) {
            pState->self.ships[shipIndex].position = VERTICAL;
        } else {
            pState->self.ships[shipIndex].position = HORIZONTAL;
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
    for (auto &p : placementIndexes) {
        pState->self.map.cells[p.x][p.y].isAvailable = false;
        pDraggedShip->bannedCells.insert({p.x, p.y});
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
    pDraggedShip->isOnMap = pState->draggedShip.oldIsOnMap;
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

void RecoverDefaultState(Ship *pDraggedShip) {
    pDraggedShip->isOnMap = false;
    pDraggedShip->rect = pDraggedShip->defaultRect;
    pDraggedShip->position = HORIZONTAL;
    pDraggedShip->height = RECT_SIDE;
    pDraggedShip->width = RECT_SIDE * pDraggedShip->type;
}

bool IsShipInsideMap(StateInfo *pState, Ship *pDraggedShip) {
    return !(pDraggedShip->rect.left > pState->self.map.coord.right ||
             pDraggedShip->rect.right < pState->self.map.coord.left ||
             pDraggedShip->rect.top > pState->self.map.coord.bottom ||
             pDraggedShip->rect.bottom < pState->self.map.coord.top);
}

void PlaceShip(StateInfo *pState, int shipIndex) {
    // means ship from game state, not DraggedShip struct
    Ship *pDraggedShip = &(pState->self.ships[shipIndex]);

    vector<POINT> placementIndexes; // i, j of marked cells
    vector<RECT> placementRects; // rects of marked cells
    GetCellsInfo(pState, &placementIndexes, &placementRects);

    if (IsShipInsideMap(pState, pDraggedShip)) {
        if (placementRects.size() == pDraggedShip->type) { // when green backlight
            pDraggedShip->isOnMap = true;

            SetNewRect(placementRects, pDraggedShip);
            BanCells(pState, placementIndexes, pDraggedShip);
        } else { // when forbidBrush backlight
            RecoverPreviousState(pState, pDraggedShip);
        }
    } else {
        RecoverDefaultState(pDraggedShip);
    }
}

void CheckStartGame(StateInfo *pState) {
    bool isAllShipOnMap = true;
    for (auto &ship : pState->self.ships) {
        if (!ship.isOnMap) {
            isAllShipOnMap = false;
        }
    }
    EnableWindow(pState->startButton, isAllShipOnMap);
}

void ResetMapAndShipsState(StateInfo *pState) {
    pState->self.map.cells.clear();
    pState->self.ships.clear();
    InitializeMap(pState, HUMAN);
    GenerateShipsPlace(pState, HUMAN);
}

vector<vector<RECT>> GetPossiblePlacesForEachType(StateInfo *pState, Position position, PlayerType playerType) {
    Player* pPlayer = (playerType == HUMAN) ? &(pState->self) : &(pState->enemy);

    vector<vector<RECT>> possiblePlacesForEach;
    RECT rect;

    for (int type = SHIP_TYPE_COUNT; type >= 1; type--) {
        vector<RECT> possiblePlaces;
        if (position == HORIZONTAL) {
            rect = {pPlayer->map.coord.left,
                    pPlayer->map.coord.top,
                    pPlayer->map.coord.left + type * RECT_SIDE,
                    pPlayer->map.coord.top + RECT_SIDE};
        } else {
            rect = {pPlayer->map.coord.left,
                    pPlayer->map.coord.top,
                    pPlayer->map.coord.left + RECT_SIDE,
                    pPlayer->map.coord.top + type * RECT_SIDE};
        }
        while (rect.bottom <= pPlayer->map.coord.bottom) {
            while (rect.right <= pPlayer->map.coord.right) {
                possiblePlaces.push_back(rect);
                rect.left += RECT_SIDE;
                rect.right += RECT_SIDE;
            }
            if (position == HORIZONTAL) {
                rect = {pPlayer->map.coord.left,
                        rect.top + RECT_SIDE,
                        pPlayer->map.coord.left + type * RECT_SIDE,
                        rect.bottom + RECT_SIDE};
            } else {
                rect = {pPlayer->map.coord.left,
                        rect.top + RECT_SIDE,
                        pPlayer->map.coord.left + RECT_SIDE,
                        rect.top + type * RECT_SIDE + RECT_SIDE};
            }
        }
        possiblePlacesForEach.push_back(possiblePlaces);
    }

    return possiblePlacesForEach;
}

vector<vector<RECT>> MergePositionsForEachType(vector<vector<RECT>> v1, vector<vector<RECT>> v2) {
    for (int i = 0; i < SHIP_TYPE_COUNT; i++) {
        v1[i].insert(v1[i].end(), v2[i].begin(), v2[i].end());
    }
    return v1;
}

void InitializePossiblePlaces(StateInfo *pState, PlayerType playerType) {
    vector<vector<RECT>> horizontalPositions = GetPossiblePlacesForEachType(pState, HORIZONTAL, playerType);
    vector<vector<RECT>> verticalPositions = GetPossiblePlacesForEachType(pState, VERTICAL, playerType);
    pState->possiblePlacesForEachShip = MergePositionsForEachType(horizontalPositions, verticalPositions);
}

void DeleteOverlappedPlaces(StateInfo *pState, RECT placedRect, PlayerType playerType) {
    Player* pPlayer = (playerType == HUMAN) ? &(pState->self) : &(pState->enemy);

    if (placedRect.left > pPlayer->map.coord.left) {
        placedRect.left -= RECT_SIDE;
    }
    if (placedRect.top > pPlayer->map.coord.top) {
        placedRect.top -= RECT_SIDE;
    }
    if (placedRect.right < pPlayer->map.coord.right) {
        placedRect.right += RECT_SIDE;
    }
    if (placedRect.bottom < pPlayer->map.coord.bottom) {
        placedRect.bottom += RECT_SIDE;
    }

    vector<vector<RECT>> placesForEach;
    for (auto &possiblePlaces : pState->possiblePlacesForEachShip) {
        vector<RECT> places;
        for (auto &possiblePlace : possiblePlaces) {
            if (!(placedRect.right > possiblePlace.left && placedRect.left < possiblePlace.right &&
                  placedRect.top < possiblePlace.bottom && placedRect.bottom > possiblePlace.top)) {
                places.push_back(possiblePlace);
            }
        }
        placesForEach.push_back(places);
    }
    pState->possiblePlacesForEachShip = placesForEach;
}

void RandomizeShipPlace(StateInfo *pState, PlayerType playerType) {
    Player* pPlayer = (playerType == HUMAN) ? &(pState->self) : &(pState->enemy);

    srand(time(NULL));
    int shipIndex = 0; // means that GenerateShips initialize ships in order 4->3->2->1
    for (int type = SHIP_TYPE_COUNT; type >= 1; type--) {
        int index = SHIP_TYPE_COUNT - type; // index of type in possiblePlaces
        for (int count = 0; count <= SHIP_TYPE_COUNT - type; count++) {
            int randomShip = rand() % (pState->possiblePlacesForEachShip[index].size());
            pPlayer->ships[shipIndex].rect = pState->possiblePlacesForEachShip[index][randomShip];

            int width = pPlayer->ships[shipIndex].rect.right - pPlayer->ships[shipIndex].rect.left;
            int height = pPlayer->ships[shipIndex].rect.bottom - pPlayer->ships[shipIndex].rect.top;
            pPlayer->ships[shipIndex].width = width;
            pPlayer->ships[shipIndex].height = height;
            if (height == RECT_SIDE) {
                pPlayer->ships[shipIndex].position = HORIZONTAL;
            } else {
                pPlayer->ships[shipIndex].position = VERTICAL;
            }

            DeleteOverlappedPlaces(pState, pPlayer->ships[shipIndex].rect, playerType);
            if (playerType == HUMAN) {
                BacklightCells(pState, shipIndex);
                PlaceShip(pState, shipIndex);
                BacklightCells(pState, shipIndex);
            }

            shipIndex++;
        }
    }
}