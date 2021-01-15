#include "../../main/config.h"
#include "handlers.h"
#include "../state_changers/state_changers.h"
#include "../../common/state_changers/state_changers.h"
#include "../../common/handlers/handlers.h"

#include <windowsx.h>
#include <objidl.h>
#include <ctime>
#include <fstream>
#include <sstream>
#include <iterator>

namespace Pregame {
    HBRUSH shipBrush = CreateSolidBrush(SHIP_COLOR);
    HBRUSH allowBrush = CreateSolidBrush(ALLOW_COLOR);
    HBRUSH forbidBrush = CreateSolidBrush(FORBID_COLOR);
    HFONT hFont = CreateFont(RECT_SIDE, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE,
                             ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                             DEFAULT_PITCH | FF_SWISS, GetStringFromResource(IDS_FONT));

    void DrawBacklightedCells(HDC hdc, StateInfo *pState) {
        bool isHasNotAvailable = false;
        for (int i = 0; i < COUNT_CELLS_IN_SIDE; i++) {
            for (int j = 0; j < COUNT_CELLS_IN_SIDE; j++) {
                if (pState->self.map.cells[i][j].isVisualized && !pState->self.map.cells[i][j].isAvailable) {
                    isHasNotAvailable = true;
                }
            }
        }
        for (int i = 0; i < COUNT_CELLS_IN_SIDE; i++) {
            for (int j = 0; j < COUNT_CELLS_IN_SIDE; j++) {
                if (pState->self.map.cells[i][j].isVisualized) {
                    HBRUSH brush;
                    if (pState->self.map.cells[i][j].isAvailable && !pState->self.map.cells[i][j].isPartial
                        && !isHasNotAvailable) {
                        brush = allowBrush;
                    } else {
                        brush = forbidBrush;
                    }
                    FillRect(hdc, &pState->self.map.cells[i][j].rect, brush);
                }
            }
        }
    }

    void DrawGameMap(HDC hdc, StateInfo *pState, PlayerType playerType) {
        HPEN hPen = CreatePen(PS_SOLID, MAP_LINE_WIDTH, MAP_COLOR);
        HPEN oldPen = (HPEN) SelectObject(hdc, hPen);

        HFONT oldFont = (HFONT) SelectObject(hdc, hFont);
        SetTextColor(hdc, MAP_COLOR);
        SetBkMode(hdc, TRANSPARENT);

        DrawBacklightedCells(hdc, pState);
        DrawLines(hdc, pState, playerType);
        DrawLetters(hdc, pState, playerType);

        SetBkMode(hdc, OPAQUE);
        SelectObject(hdc, oldFont);

        SelectObject(hdc, oldPen);
        DeleteObject(hPen);
    }

    void DrawShips(HDC hdc, StateInfo *pState, PlayerType playerType) {
        Player *pPlayer = (playerType == HUMAN) ? &(pState->self) : &(pState->enemy);

        HBRUSH brush = shipBrush;
        auto iter = pPlayer->ships.begin();
        while (iter != pPlayer->ships.end()) {
            FillRect(hdc, &iter->rect, brush);
            ++iter;
        }
    }

    void PrepareStructs(StateInfo *pState, PlayerType playerType) {
        Player *pPlayer = (playerType == HUMAN) ? &(pState->self) : &(pState->enemy);

        for (int i = 0; i < COUNT_CELLS_IN_SIDE; i++) {
            for (int j = 0; j < COUNT_CELLS_IN_SIDE; j++) {
                pPlayer->map.cells[i][j].isAvailable = true;
                pPlayer->map.cells[i][j].isAttempted = false;
                pPlayer->map.cells[i][j].index = -1;
            }
        }

        for (auto &ship : pPlayer->ships) {
            ship.aliveCount = ship.type;
        }

        for (int i = 0; i < pPlayer->ships.size(); i++) {
            bool isFound = false;
            int m = 0;
            while (m < COUNT_CELLS_IN_SIDE && !isFound) {
                int n = 0;
                while (n < COUNT_CELLS_IN_SIDE && !isFound) {
                    if ((pPlayer->ships[i].rect.left == pPlayer->map.cells[m][n].rect.left) &&
                        (pPlayer->ships[i].rect.top == pPlayer->map.cells[m][n].rect.top)) {
                        isFound = true;
                        for (int j = 0; j < pPlayer->ships[i].type; j++) {
                            if (pPlayer->ships[i].position == HORIZONTAL) {
                                pPlayer->map.cells[m][n + j].index = i;
                            } else {
                                pPlayer->map.cells[m + j][n].index = i;
                            }
                        }
                    }
                    n++;
                }
                m++;
            }
        }
    }

    void InitializeAvailableForTurnCells(StateInfo *pState) {
        for (int i = 0; i < COUNT_CELLS_IN_SIDE; i++) {
            for (int j = 0; j < COUNT_CELLS_IN_SIDE; j++) {
                pState->availableForComputerCells.push_back({i, j});
            }
        }
    }

    void GetContentFromFile(HWND hwnd, StateInfo *pState) {
        string line;
        ifstream file(GetStringFromResource(IDS_STAT_FILENAME));
        if (file.is_open()) {
            getline(file, line);
            file.close();
            vector<string> vector;
            istringstream iss(line);
            copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter(vector));
            bool isFileCorrupted = false;
            if (vector.size() != 3) {
                isFileCorrupted = true;
            }
            if (!isFileCorrupted) {
                for (auto &value : vector) {
                    try {
                        if (stoi(value) < 0) {
                            isFileCorrupted = true;
                        }
                    } catch (...) {
                        isFileCorrupted = true;
                    }
                }
            }
            if (!isFileCorrupted) {
                if (stoi(vector[1]) > stoi(vector[2])) {
                    isFileCorrupted = true;
                }
            }
            if (!isFileCorrupted) {
                pState->stat = {stoi(vector[0]), stoi(vector[1]), stoi(vector[2])};
            } else {
                MessageBox(hwnd, GetStringFromResource(IDS_FILE_CORRUPTED), GetStringFromResource(IDS_ERROR), MB_OK);
                pState->stat = {0, 0, 0};
            }
        } else {
            pState->stat = {0, 0, 0};
        }
    }

    void OnCreate(HWND hwnd, StateInfo *pState, HDC *hdcBack, HBITMAP *hbmBack) {
        RECT winRect;

        if (GetClientRect(hwnd, &winRect)) {
            int width = winRect.right - winRect.left;
            int height = winRect.bottom - winRect.top;
            pState->clientWidth = width;
            pState->clientHeight = height;
            pState->background = (HBITMAP) LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BACKGROUND),
                                                     IMAGE_BITMAP, 0, 0, NULL);
            pState->isSoundOn = true;
            GetContentFromFile(hwnd, pState);

            InitializeMap(pState, HUMAN);
            GenerateShipsPlace(pState, HUMAN);
            SetButtons(hwnd, pState);

            //Double-buffering
            HDC hdc = GetDC(hwnd);
            *hdcBack = CreateCompatibleDC(hdc);
            *hbmBack = CreateCompatibleBitmap(hdc, pState->clientWidth, pState->clientHeight);
            ReleaseDC(hwnd, hdc);
            SaveDC(*hdcBack);
            SelectObject(*hdcBack, *hbmBack);
        }
    }

    void OnMouseMove(HWND hwnd, LPARAM lParam, StateInfo *pState) {
        if (pState->isDragged) {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            int i = pState->draggedShip.index;

            UpdateShipRect(pState, x, y, i);
            BacklightCells(pState, i);

            InitiateRedraw(hwnd);
        }
    }

    void OnPaint(HWND hwnd, HDC hdcBack, StateInfo *pState) {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        DrawBackground(hdcBack, pState);
        DrawGameMap(hdcBack, pState, HUMAN);
        DrawShips(hdcBack, pState, HUMAN);

        BitBlt(hdc, 0, 0, pState->clientWidth, pState->clientHeight, hdcBack, 0, 0, SRCCOPY);

        EndPaint(hwnd, &ps);
    }

    void OnLButtonDown(HWND hwnd, LPARAM lParam, StateInfo *pState) {
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);
        POINT point = {x, y};

        if (DragDetect(hwnd, point)) {
            BOOL isNotFound = true;
            int index = 0;
            auto iter = pState->self.ships.begin();
            while (iter != pState->self.ships.end() && isNotFound) {
                if (x >= iter->rect.left && x <= iter->rect.right && y >= iter->rect.top &&
                    y <= iter->rect.bottom) {
                    isNotFound = false;

                    InitializeDraggedShip(pState, *iter, index, x, y);
                    UnbanCells(pState, index);
                }
                index++;
                ++iter;
            }
        }
    }

    void OnKeyDown(HWND hwnd, WPARAM wParam, StateInfo *pState) {
        if ((wParam == VK_SHIFT) && pState->isDragged) {
            RotateShip(hwnd, pState, pState->draggedShip.index);
            BacklightCells(pState, pState->draggedShip.index);

            InitiateRedraw(hwnd);
        }
    }

    void OnLButtonUp(HWND hwnd, StateInfo *pState) {
        if (pState->isDragged) {
            pState->isDragged = FALSE;

            PlaceShip(pState, pState->draggedShip.index);
            BacklightCells(pState, pState->draggedShip.index);
            CheckStartGame(pState);

            InitiateRedraw(hwnd);
        }
    }

    void OnRandomButtonClick(HWND hwnd, StateInfo *pState) {
        ResetMapAndShipsState(pState);

        InitializePossiblePlaces(pState, HUMAN);
        RandomizeShipPlace(pState, HUMAN);

        EnableWindow(pState->startButton, true);
        InitiateRedraw(hwnd);
        SetFocus(hwnd);
    }

    void OnStartButtonClick(HWND hwnd, StateInfo *pState) {
        srand(time(NULL));

        pState->gameState = GAME;
        pState->turn = HUMAN;
        pState->isGameEnded = false;
        pState->gameLength = 0;
        pState->gameLengthTimer = SetTimer(hwnd, LENGTH_TIMER_ID, SECOND_DELAY, NULL);
        pState->decisionBasis = {false, false, static_cast<Direction>(rand() % 4),
                                 0, 0, 1};

        InitializeMap(pState, COMPUTER);
        GenerateShipsPlace(pState, COMPUTER);

        InitializePossiblePlaces(pState, COMPUTER);
        RandomizeShipPlace(pState, COMPUTER);
        InitializeAvailableForTurnCells(pState);

        PrepareStructs(pState, HUMAN);
        PrepareStructs(pState, COMPUTER);

        DestroyWindow(pState->startButton);
        DestroyWindow(pState->randomButton);
        InitiateRedraw(hwnd);
        SetFocus(hwnd);
    }

    void OnCommand(HWND hwnd, LPARAM lParam, WPARAM wParam, StateInfo *pState) {
        if ((HWND) lParam == pState->randomButton) {
            OnRandomButtonClick(hwnd, pState);
        } else if ((HWND) lParam == pState->startButton) {
            OnStartButtonClick(hwnd, pState);
        } else {
            OnMainMenuButtonsClick(hwnd, wParam, pState);
        }

    }
}