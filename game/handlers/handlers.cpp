#ifndef UNICODE
#define UNICODE
#endif

#include "../../main/config.h"
#include "handlers.h"
#include "../state_changers/state_changers.h"

#include <windowsx.h>
#include <ctime>
#include <Mmsystem.h>

namespace Game {
    const TCHAR *letters[] = {L"A", L"B", L"C", L"D", L"E", L"F", L"G", L"H", L"I", L"J"};
    const TCHAR *numbers[] = {L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9", L"10"};

    HBRUSH black = CreateSolidBrush(RGB(0, 0, 0));
    HBRUSH back = CreateSolidBrush(RGB(0, 213, 255));

    void InitiateRedraw(HWND hwnd) {
        RECT rect;
        GetClientRect(hwnd, &rect);
        InvalidateRect(hwnd, &rect, TRUE);
    }

    BOOL DrawLine(HDC hdc, int x1, int y1, int x2, int y2) {
        POINT pt;
        MoveToEx(hdc, x1, y1, &pt);
        return LineTo(hdc, x2, y2);
    }

    void DrawLines(HDC hdc, StateInfo *pState, PlayerType playerType) {
        Player* pPlayer = (playerType == HUMAN) ? &(pState->self) : &(pState->enemy);

        for (int i = 0; i <= COUNT_CELLS_IN_SIDE; i++) {
            int x = pPlayer->map.coord.left + RECT_SIDE * i;
            DrawLine(hdc, x, pPlayer->map.coord.top, x, pPlayer->map.coord.bottom);
        }
        for (int i = 0; i <= COUNT_CELLS_IN_SIDE; i++) {
            int y = pPlayer->map.coord.top + RECT_SIDE * i;
            DrawLine(hdc, pPlayer->map.coord.left, y, pPlayer->map.coord.right, y);
        }
    }

    void DrawLetters(HDC hdc, StateInfo *pState, PlayerType playerType) {
        Player* pPlayer = (playerType == HUMAN) ? &(pState->self) : &(pState->enemy);

        for (int i = 0; i < COUNT_CELLS_IN_SIDE; i++) {
            RECT rect = {
                    pPlayer->map.coord.left - RECT_SIDE,
                    pPlayer->map.coord.top + RECT_SIDE * i,
                    pPlayer->map.coord.left,
                    pPlayer->map.coord.top + RECT_SIDE * (i + 1)
            };
            DrawText(hdc, numbers[i], -1, &rect, DT_CENTER);
        }
        for (int i = 0; i < COUNT_CELLS_IN_SIDE; i++) {
            RECT rect = {
                    pPlayer->map.coord.left + RECT_SIDE * i,
                    pPlayer->map.coord.top - RECT_SIDE,
                    pPlayer->map.coord.left + RECT_SIDE * (i + 1),
                    pPlayer->map.coord.top
            };
            DrawText(hdc, letters[i], -1, &rect, DT_CENTER);
        }
    }

    void DrawShootedCells(HDC hdc, StateInfo *pState, PlayerType playerType) {
        Player* pPlayer = (playerType == HUMAN) ? &(pState->self) : &(pState->enemy);

        //possible leak
        HPEN hPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
        HPEN oldPen = (HPEN) SelectObject(hdc, hPen);

        for (int i = 0; i < COUNT_CELLS_IN_SIDE; i++) {
            for (int j = 0; j < COUNT_CELLS_IN_SIDE; j++) {
                if (!pPlayer->map.cells[i][j].isAvailable && pPlayer->map.cells[i][j].index == -1) {
                    //точка
                    DrawLine(hdc, pPlayer->map.cells[i][j].rect.left + 11, pPlayer->map.cells[i][j].rect.top + 11,
                             pPlayer->map.cells[i][j].rect.right - 11, pPlayer->map.cells[i][j].rect.bottom - 11);
                    DrawLine(hdc, pPlayer->map.cells[i][j].rect.right - 11, pPlayer->map.cells[i][j].rect.top + 11,
                             pPlayer->map.cells[i][j].rect.left + 11, pPlayer->map.cells[i][j].rect.bottom - 11);
                }
                if (!pPlayer->map.cells[i][j].isAvailable && pPlayer->map.cells[i][j].index != -1) {
                    //крест
                    DrawLine(hdc, pPlayer->map.cells[i][j].rect.left + 3, pPlayer->map.cells[i][j].rect.top + 3,
                             pPlayer->map.cells[i][j].rect.right - 3, pPlayer->map.cells[i][j].rect.bottom - 3);
                    DrawLine(hdc, pPlayer->map.cells[i][j].rect.right - 3, pPlayer->map.cells[i][j].rect.top + 3,
                             pPlayer->map.cells[i][j].rect.left + 3, pPlayer->map.cells[i][j].rect.bottom - 3);
                }
            }
        }

        SelectObject(hdc, oldPen);
        DeleteObject(hPen);
    }

    void DrawGameMap(HDC hdc, StateInfo *pState, PlayerType playerType) {
        HPEN hPen = CreatePen(PS_SOLID, 3, RGB(255, 255, 255));
        HPEN oldPen = (HPEN) SelectObject(hdc, hPen);

        HFONT hFont = CreateFont(RECT_SIDE, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE,
                                 ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                                 DEFAULT_PITCH | FF_SWISS, L"Arial");
        HFONT oldFont = (HFONT) SelectObject(hdc, hFont);
        SetTextColor(hdc, RGB(255, 255, 255));
        SetBkMode(hdc, TRANSPARENT);

        DrawLines(hdc, pState, playerType);
        DrawLetters(hdc, pState, playerType);

        SetBkMode(hdc, OPAQUE);
        SelectObject(hdc, oldFont);
        DeleteObject(hFont);

        SelectObject(hdc, oldPen);
        DeleteObject(hPen);
    }

    void DrawShips(HDC hdc, StateInfo *pState, PlayerType playerType) {
        Player* pPlayer = (playerType == HUMAN) ? &(pState->self) : &(pState->enemy);

        HBRUSH brush = black;
        auto iter = pPlayer->ships.begin();
        while (iter != pPlayer->ships.end()) {
            if (playerType == COMPUTER) {
                if (!iter->aliveCount) {
                    FillRect(hdc, &iter->rect, brush);
                }
            } else {
                FillRect(hdc, &iter->rect, brush);
            }
            ++iter;
        }
    }

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

    void SetButtons(HWND hwnd, StateInfo *pState) {
        int x = pState->clientWidth / 2 - BUTTON_WIDTH / 2;
        int y = pState->self.map.coord.bottom + 4 * RECT_SIDE;
        pState->startButton = CreateWindow(
                L"BUTTON",  // Predefined class; Unicode assumed
                L"START",      // Button text
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles
                x,         // x position
                y,         // y position
                BUTTON_WIDTH,        // Button width
                BUTTON_HEIGHT,        // Button height
                hwnd,     // Parent window
                NULL,       // No menu.
                (HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
                NULL);
        EnableWindow(pState->startButton, false);

        x = pState->self.map.coord.left + (pState->self.map.coord.right - pState->self.map.coord.left - BUTTON_WIDTH) / 2;
        pState->randomButton = CreateWindow(
                L"BUTTON",  // Predefined class; Unicode assumed
                L"RANDOM",      // Button text
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles
                x,         // x position
                y,         // y position
                BUTTON_WIDTH,        // Button width
                BUTTON_HEIGHT,        // Button height
                hwnd,     // Parent window
                NULL,       // No menu.
                (HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
                NULL);
    }

    void OnLButtonDown(HWND hwnd, LPARAM lParam, StateInfo *pState) {
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);

        bool isFound = false;
        int i = 0;
        while (i < COUNT_CELLS_IN_SIDE && !isFound) {
            int j = 0;
            while (j < COUNT_CELLS_IN_SIDE && !isFound) {
                if (x >= pState->enemy.map.cells[i][j].rect.left && x <= pState->enemy.map.cells[i][j].rect.right
                    && y >= pState->enemy.map.cells[i][j].rect.top && y <= pState->enemy.map.cells[i][j].rect.bottom) {
                    isFound = true;
                    if (pState->enemy.map.cells[i][j].isAvailable) {
                        pState->enemy.map.cells[i][j].isAvailable = false;
                        pState->enemy.map.cells[i][j].isAttempted = true;
                        if (pState->enemy.map.cells[i][j].index != -1) {
                            PlaySound(L"Hit", GetModuleHandle(NULL), SND_FILENAME | SND_ASYNC);
                            pState->enemy.ships[pState->enemy.map.cells[i][j].index].aliveCount--;
                            if (pState->enemy.ships[pState->enemy.map.cells[i][j].index].aliveCount == 0) {
                                UnableAdjacentCells(pState, pState->enemy.map.cells[i][j].index, COMPUTER);
                                CheckWinOf(hwnd, pState, HUMAN);
                            }
                        } else {
                            PlaySound(L"Miss", GetModuleHandle(NULL), SND_FILENAME | SND_ASYNC);
                            pState->turn = COMPUTER;
                            pState->timer = SetTimer(hwnd, TIMER_ID, COMPUTER_DELAY, NULL);
                        }
                    }
                }
                j++;
            }
            i++;
        }

        InitiateRedraw(hwnd);
    }

    void OnPaint(HWND hwnd, HDC hdcBack, StateInfo *pState) {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        HBRUSH brush = back;
        FillRect(hdcBack, &ps.rcPaint, brush);

        DrawGameMap(hdcBack, pState, HUMAN);
        DrawShips(hdcBack, pState, HUMAN);
        DrawShootedCells(hdcBack, pState, HUMAN);

        DrawGameMap(hdcBack, pState, COMPUTER);
        DrawShips(hdcBack, pState, COMPUTER);
        DrawShootedCells(hdcBack, pState, COMPUTER);

        BitBlt(hdc, 0, 0, pState->clientWidth, pState->clientHeight, hdcBack, 0, 0, SRCCOPY);

        EndPaint(hwnd, &ps);

        if (pState->isGameEnded) {
            int result;
            if (pState->winner == HUMAN) {
                result = MessageBox(hwnd, L"You defeat computer!", L"YOU ARE WIN", MB_OK);
            } else {
                result = MessageBox(hwnd, L"You were defeated by computer!", L"GAME OVER", MB_OK);
            }
            if (result == IDOK) {
                pState->gameState = PREGAME;

                pState->self.map.cells.clear();
                InitializeMap(pState, HUMAN);
                pState->self.ships.clear();
                pState->enemy.ships.clear();
                GenerateShipsPlace(pState, HUMAN);
                SetButtons(hwnd, pState);

                InitiateRedraw(hwnd);
            }
        }
    }

    void OnTimer(HWND hwnd, WPARAM wParam, StateInfo *pState) {
        static int i = 0, j = 0;
        if (wParam == TIMER_ID) {
            MakeComputerMove(hwnd, pState);

            InitiateRedraw(hwnd);
        }
    }
}