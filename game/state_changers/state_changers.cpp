#include "../../main/config.h"
#include "state_changers.h"
#include "../../common/handlers/handlers.h"
#include "../../common/state_changers/state_changers.h"

#include "ctime"

bool CheckWinOf(StateInfo *pState, PlayerType playerType) {
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

void GetNextCellProperties(DecisionBasis *pDecisionBasis, int &m, int &n, bool &indexCondition) {
    if (pDecisionBasis->direction == LEFT) {
        m = pDecisionBasis->i;
        n = pDecisionBasis->j - pDecisionBasis->shift;
        indexCondition = n >= 0;
    } else if (pDecisionBasis->direction == TOP) {
        m = pDecisionBasis->i - pDecisionBasis->shift;
        n = pDecisionBasis->j;
        indexCondition = m >= 0;
    } else if (pDecisionBasis->direction == RIGHT) {
        m = pDecisionBasis->i;
        n = pDecisionBasis->j + pDecisionBasis->shift;
        indexCondition = n <= 9;
    } else if (pDecisionBasis->direction == BOTTOM) {
        m = pDecisionBasis->i + pDecisionBasis->shift;
        n = pDecisionBasis->j;
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
    KillTimer(hwnd, pState->turnTimer);
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

void RandomShot(HWND hwnd, StateInfo *pState, DecisionBasis *pDecisionBasis) {
    RenewAvailableCellsForComputerTurn(pState);
    RandomizeCell(pState, pDecisionBasis->i, pDecisionBasis->j);
    AttemptCell(pState, pDecisionBasis->i, pDecisionBasis->j);

    if (pState->self.map.cells[pDecisionBasis->i][pDecisionBasis->j].index != -1) {
        PlayShotSound(pState, IDS_HIT);

        pDecisionBasis->isFinishOff = true;
        int shipIndex = pState->self.map.cells[pDecisionBasis->i][pDecisionBasis->j].index;
        pState->self.ships[shipIndex].aliveCount--;
        if (pState->self.ships[shipIndex].aliveCount == 0) {
            UnableAdjacentCells(pState, shipIndex,HUMAN);
            pDecisionBasis->isFinishOff = false;
            if (CheckWinOf(pState, COMPUTER)) {
                EndTurn(hwnd, pState);
            };
        }
    } else {
        PlayShotSound(pState, IDS_MISS);

        EndTurn(hwnd, pState);
    }
}

void FinishingShot(HWND hwnd, StateInfo *pState, DecisionBasis *pDecisionBasis) {
    bool isMoved = false;
    while (!isMoved) {
        int m, n;
        bool indexCondition = false;
        GetNextCellProperties(pDecisionBasis, m, n, indexCondition);

        if (indexCondition && pState->self.map.cells[m][n].isAvailable) {
            isMoved = true;

            AttemptCell(pState, m, n);
            if (pState->self.map.cells[m][n].index != -1) {
                PlayShotSound(pState, IDS_HIT);

                pDecisionBasis->isDirectionFound = true;
                int shipIndex = pState->self.map.cells[m][n].index;
                pState->self.ships[shipIndex].aliveCount--;
                if (pState->self.ships[shipIndex].aliveCount == 0) {
                    UnableAdjacentCells(pState, shipIndex, HUMAN);
                    pDecisionBasis->isFinishOff = false;
                    pDecisionBasis->shift = 1;
                    pDecisionBasis->direction = static_cast<Direction>(rand() % 4);
                    pDecisionBasis->isDirectionFound = false;

                    if (CheckWinOf(pState, COMPUTER)) {
                        EndTurn(hwnd, pState);
                    }
                } else {
                    (pDecisionBasis->shift)++;
                }
            } else {
                PlayShotSound(pState, IDS_MISS);

                SetNewDirection(pDecisionBasis->isDirectionFound, pDecisionBasis->direction, pDecisionBasis->shift);
                EndTurn(hwnd, pState);
            }
        } else {
            SetNewDirection(pDecisionBasis->isDirectionFound, pDecisionBasis->direction, pDecisionBasis->shift);
        }
    }
}

void MakeComputerMove(HWND hwnd, StateInfo *pState) {
    srand(time(NULL));

    DecisionBasis *pDecisionBasis = &pState->decisionBasis;

    if (!pDecisionBasis->isFinishOff) {
        RandomShot(hwnd, pState, pDecisionBasis);
    } else {
        FinishingShot(hwnd, pState, pDecisionBasis);
    }
}

void MakeHumanMove(HWND hwnd, StateInfo *pState, int i, int j) {
    if (pState->enemy.map.cells[i][j].isAvailable) {
        pState->enemy.map.cells[i][j].isAvailable = false;
        pState->enemy.map.cells[i][j].isAttempted = true;

        if (pState->enemy.map.cells[i][j].index != -1) {
            PlayShotSound(pState, IDS_HIT);

            pState->enemy.ships[pState->enemy.map.cells[i][j].index].aliveCount--;
            if (pState->enemy.ships[pState->enemy.map.cells[i][j].index].aliveCount == 0) {
                UnableAdjacentCells(pState, pState->enemy.map.cells[i][j].index, COMPUTER);
                CheckWinOf(pState, HUMAN);
            }
        } else {
            PlayShotSound(pState, IDS_MISS);

            pState->turn = COMPUTER;
            pState->turnTimer = SetTimer(hwnd, TURN_TIMER_ID, COMPUTER_DELAY, NULL);
        }
    }
}

void PrepareStatInfo(StateInfo *pState, wstring& text) {
    float attempts = 0, hits = 0;
    for (int i = 0; i < COUNT_CELLS_IN_SIDE; i++) {
        for (int j = 0; j < COUNT_CELLS_IN_SIDE; j++) {
            if (pState->enemy.map.cells[i][j].isAttempted) {
                attempts++;
                if (pState->enemy.map.cells[i][j].index != -1) {
                    hits++;
                }
            }
        }
    }

    float accuracy = hits / attempts;
    text += GetStringFromResource(IDS_ACCURACY) + ConvertNumberToWString(floorf(accuracy * 100) / 100);
    text += GetStringFromResource(IDS_TIME) + ConvertNumberToWString(pState->gameLength) + GetStringFromResource(IDS_SEC);
    text += GetStringFromResource(IDS_BEST_TIME) + (pState->stat.bestVictoryTime > 0
                                          ? ConvertNumberToWString(pState->stat.bestVictoryTime) + GetStringFromResource(IDS_SEC)
                                          : GetStringFromResource(IDS_NO_WINS));
    text += GetStringFromResource(IDS_GAMES) + ConvertNumberToWString(pState->stat.games);
    text += GetStringFromResource(IDS_WINS) + ConvertNumberToWString(pState->stat.wins);
    float winPercent = floorf(((float) pState->stat.wins / (float) pState->stat.games) * 100);
    text += GetStringFromResource(IDS_PERCENT) + ConvertNumberToWString(winPercent) + GetStringFromResource(IDS_PERCENT_SIGN);
    text += GetStringFromResource(IDS_ANOTHER);
}

void FinishGame(HWND hwnd, StateInfo *pState) {
    KillTimer(hwnd, pState->gameLengthTimer);

    std::wstring text, caption;
    pState->stat.games++;
    if (pState->winner == HUMAN) {
        pState->stat.wins++;
        if (pState->gameLength < pState->stat.bestVictoryTime || pState->stat.bestVictoryTime == 0) {
            pState->stat.bestVictoryTime = pState->gameLength;
        }
        caption = GetStringFromResource(IDS_WIN);
        text = GetStringFromResource(IDS_WIN_DESCRIPTION);
    } else {
        caption = GetStringFromResource(IDS_LOST);
        text = GetStringFromResource(IDS_LOST_DESCRIPTION);
    }

    PrepareStatInfo(pState, text);

    switch (MessageBox(hwnd, text.c_str(), caption.c_str(), MB_YESNO)) {
        case IDYES:
            pState->gameState = PREGAME;

            pState->self.map.cells.clear();
            InitializeMap(pState, HUMAN);
            pState->self.ships.clear();
            pState->enemy.ships.clear();
            GenerateShipsPlace(pState, HUMAN);
            SetButtons(hwnd, pState);

            InitiateRedraw(hwnd);
            break;

        case IDNO:
            PostMessage(hwnd, WM_CLOSE, 0, 0);
            break;
    }
}