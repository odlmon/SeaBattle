#ifndef UNICODE
#define UNICODE
#endif

#include "../../main/config.h"
#include "state_changers.h"

#include "ctime"

bool CheckWinOf(HWND hwnd, StateInfo *pState, PlayerType playerType) {
    Player* pCheckingPlayer = (playerType == HUMAN) ? &(pState->enemy) : &(pState->self);

    bool isAllDestroyed = true;
    for (auto & ship : pCheckingPlayer->ships) {
        if (ship.aliveCount != 0) {
            isAllDestroyed = false;
        }
    }

    if (isAllDestroyed) {
        pState->isGameEnded = true;
        pState->winner = playerType;
    }

    return pState->isGameEnded;
}

void UnableAdjacentCells(StateInfo *pState, int shipIndex, PlayerType playerType) {
    Player* pPlayer = (playerType == HUMAN) ? &(pState->self) : &(pState->enemy);

    vector<POINT> placementIndexes;
    for (int i = 0; i < COUNT_CELLS_IN_SIDE; i++) {
        for (int j = 0; j < COUNT_CELLS_IN_SIDE; j++) {
            if (pPlayer->map.cells[i][j].index == shipIndex) {
                placementIndexes.push_back({i, j});
            }
        }
    }

    for (auto &p : placementIndexes) {
        if (p.x - 1 > -1 && p.x - 1 < 10) {
            pPlayer->map.cells[p.x - 1][p.y].isAvailable = false;
        }
        if (p.x + 1 > -1 && p.x + 1 < 10) {
            pPlayer->map.cells[p.x + 1][p.y].isAvailable = false;
        }
        if (p.y - 1 > -1 && p.y - 1 < 10) {
            pPlayer->map.cells[p.x][p.y - 1].isAvailable = false;
        }
        if (p.y + 1 > -1 && p.y + 1 < 10) {
            pPlayer->map.cells[p.x][p.y + 1].isAvailable = false;
        }
        if (p.x - 1 > -1 && p.x - 1 < 10 && p.y - 1 > -1 && p.y - 1 < 10) {
            pPlayer->map.cells[p.x - 1][p.y - 1].isAvailable = false;
        }
        if (p.x - 1 > -1 && p.x - 1 < 10 && p.y + 1 > -1 && p.y + 1 < 10) {
            pPlayer->map.cells[p.x - 1][p.y + 1].isAvailable = false;
        }
        if (p.x + 1 > -1 && p.x + 1 < 10 && p.y - 1 > -1 && p.y - 1 < 10) {
            pPlayer->map.cells[p.x + 1][p.y - 1].isAvailable = false;
        }
        if (p.x + 1 > -1 && p.x + 1 < 10 && p.y + 1 > -1 && p.y + 1 < 10) {
            pPlayer->map.cells[p.x + 1][p.y + 1].isAvailable = false;
        }
    }
}

void RenewAvailableCellsForComputerTurn(StateInfo *pState) {
    pState->availableForComputerCells.clear();
    for (int m = 0; m < COUNT_CELLS_IN_SIDE; m++) {
        for (int n = 0; n < COUNT_CELLS_IN_SIDE; n++) {
            if (pState->self.map.cells[m][n].isAvailable) {
                pState->availableForComputerCells.push_back({m, n});
            }
        }
    }
}

void GetNextCellPropertiesByDirectionAndStartCell(Direction direction, int i, int j, int shift, int &m, int &n,
                                                  bool &indexCondition) {
    if (direction == LEFT) {
        m = i;
        n = j - shift;
        indexCondition = n >= 0;
    } else if (direction == TOP) {
        m = i - shift;
        n = j;
        indexCondition = m >= 0;
    } else if (direction == RIGHT) {
        m = i;
        n = j + shift;
        indexCondition = n <= 9;
    } else if (direction == BOTTOM) {
        m = i + shift;
        n = j;
        indexCondition = m <= 9;
    }
}

void SetNewDirection(bool isDirectionFound, Direction &direction, int &shift) {  // if missed
    if (isDirectionFound) {
        direction = static_cast<Direction>((direction + 2) % 4);
    } else {
        direction = static_cast<Direction>((direction + 1) % 4);
    }
    shift = 1;
}

void EndTurn(HWND hwnd, StateInfo *pState) {
    pState->turn = HUMAN;
    KillTimer(hwnd, pState->timer);
}

void RandomizeCell(StateInfo *pState, int &i, int &j) {
    int cellIndex = rand() % (pState->availableForComputerCells.size());
    i = pState->availableForComputerCells[cellIndex].x;
    j = pState->availableForComputerCells[cellIndex].y;
}

void AttemptCell(StateInfo *pState, int i, int j) {
    pState->self.map.cells[i][j].isAvailable = false;
    pState->self.map.cells[i][j].isAttempted = true;
}

void MakeComputerMove(HWND hwnd, StateInfo *pState) {
    srand(time(NULL));

    static bool isFinishOff = false;
    static bool isDirectionFound = false;
    static auto direction = static_cast<Direction>(rand() % 4);
    static int i = 0, j = 0, shift = 1;

    if (!isFinishOff) {
        RenewAvailableCellsForComputerTurn(pState);
        RandomizeCell(pState, i, j);
        AttemptCell(pState, i, j);

        if (pState->self.map.cells[i][j].index != -1) {
            PlaySound(L"Hit", GetModuleHandle(NULL), SND_FILENAME | SND_ASYNC);
            isFinishOff = true;
            pState->self.ships[pState->self.map.cells[i][j].index].aliveCount--;
            if (pState->self.ships[pState->self.map.cells[i][j].index].aliveCount == 0) {
                UnableAdjacentCells(pState, pState->self.map.cells[i][j].index, HUMAN);
                isFinishOff = false;
                if (CheckWinOf(hwnd, pState, COMPUTER)) {
                    EndTurn(hwnd, pState);
                };
            }
        } else {
            PlaySound(L"Miss", GetModuleHandle(NULL), SND_FILENAME | SND_ASYNC);
            EndTurn(hwnd, pState);
        }
    } else {
        bool isMoved = false;
        while (!isMoved) {
            int m, n;
            bool indexCondition = false;
            GetNextCellPropertiesByDirectionAndStartCell(direction, i, j, shift, m, n, indexCondition);

            if (indexCondition && pState->self.map.cells[m][n].isAvailable) {
                isMoved = true;

                AttemptCell(pState, m, n);

                if (pState->self.map.cells[m][n].index != -1) {
                    PlaySound(L"Hit", GetModuleHandle(NULL), SND_FILENAME | SND_ASYNC);
                    isDirectionFound = true;
                    pState->self.ships[pState->self.map.cells[m][n].index].aliveCount--;
                    if (pState->self.ships[pState->self.map.cells[m][n].index].aliveCount == 0) {
                        UnableAdjacentCells(pState, pState->self.map.cells[m][n].index, HUMAN);
                        isFinishOff = false;
                        shift = 1;
                        direction = static_cast<Direction>(rand() % 4);
                        isDirectionFound = false;
                        if (CheckWinOf(hwnd, pState, COMPUTER)) {
                            EndTurn(hwnd, pState);
                        }
                    } else {
                        shift++;
                    }
                } else {
                    PlaySound(L"Miss", GetModuleHandle(NULL), SND_FILENAME | SND_ASYNC);
                    SetNewDirection(isDirectionFound, direction, shift);
                    EndTurn(hwnd, pState);
                }
            } else {
                SetNewDirection(isDirectionFound, direction, shift);
            }
        }
    }
}