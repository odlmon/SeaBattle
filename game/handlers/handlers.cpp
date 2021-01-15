#include "../../main/config.h"
#include "handlers.h"
#include "../state_changers/state_changers.h"
#include "../../common/state_changers/state_changers.h"
#include "../../common/handlers/handlers.h"

#include <windowsx.h>

namespace Game {
    HBRUSH shipBrush = CreateSolidBrush(SHIP_COLOR);
    HFONT hFont = CreateFont(RECT_SIDE, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE,
                             ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                             DEFAULT_PITCH | FF_SWISS, GetStringFromResource(IDS_FONT));

    void DrawDot(HDC hdc, StateInfo *pState, PlayerType playerType, int i, int j) {
        Player* pPlayer = (playerType == HUMAN) ? &(pState->self) : &(pState->enemy);

        int delta = RECT_SIDE / 2 - 1;
        DrawLine(hdc, pPlayer->map.cells[i][j].rect.left + delta, pPlayer->map.cells[i][j].rect.top + delta,
                 pPlayer->map.cells[i][j].rect.right - delta, pPlayer->map.cells[i][j].rect.bottom - delta);
        DrawLine(hdc, pPlayer->map.cells[i][j].rect.right - delta, pPlayer->map.cells[i][j].rect.top + delta,
                 pPlayer->map.cells[i][j].rect.left + delta, pPlayer->map.cells[i][j].rect.bottom - delta);
    }

    void DrawCross(HDC hdc, StateInfo *pState, PlayerType playerType, int i, int j) {
        Player* pPlayer = (playerType == HUMAN) ? &(pState->self) : &(pState->enemy);

        DrawLine(hdc, pPlayer->map.cells[i][j].rect.left + 3, pPlayer->map.cells[i][j].rect.top + 3,
                 pPlayer->map.cells[i][j].rect.right - 3, pPlayer->map.cells[i][j].rect.bottom - 3);
        DrawLine(hdc, pPlayer->map.cells[i][j].rect.right - 3, pPlayer->map.cells[i][j].rect.top + 3,
                 pPlayer->map.cells[i][j].rect.left + 3, pPlayer->map.cells[i][j].rect.bottom - 3);
    }

    void DrawShootedCells(HDC hdc, StateInfo *pState, PlayerType playerType) {
        Player* pPlayer = (playerType == HUMAN) ? &(pState->self) : &(pState->enemy);

        HPEN hPen = CreatePen(PS_SOLID, RESULT_LINE_WIDTH, RESULT_COLOR);
        HPEN oldPen = (HPEN) SelectObject(hdc, hPen);

        for (int i = 0; i < COUNT_CELLS_IN_SIDE; i++) {
            for (int j = 0; j < COUNT_CELLS_IN_SIDE; j++) {
                if (!pPlayer->map.cells[i][j].isAvailable && pPlayer->map.cells[i][j].index == -1) {
                    DrawDot(hdc, pState, playerType, i, j);
                }
                if (!pPlayer->map.cells[i][j].isAvailable && pPlayer->map.cells[i][j].index != -1) {
                    DrawCross(hdc, pState, playerType, i, j);
                }
            }
        }

        SelectObject(hdc, oldPen);
        DeleteObject(hPen);
    }

    void DrawGameMap(HDC hdc, StateInfo *pState, PlayerType playerType) {
        HPEN hPen = CreatePen(PS_SOLID, MAP_LINE_WIDTH, MAP_COLOR);
        HPEN oldPen = (HPEN) SelectObject(hdc, hPen);

        HFONT oldFont = (HFONT) SelectObject(hdc, hFont);
        SetTextColor(hdc, MAP_COLOR);
        SetBkMode(hdc, TRANSPARENT);

        DrawLines(hdc, pState, playerType);
        DrawLetters(hdc, pState, playerType);

        SetBkMode(hdc, OPAQUE);
        SelectObject(hdc, oldFont);

        SelectObject(hdc, oldPen);
        DeleteObject(hPen);
    }

    void DrawShips(HDC hdc, StateInfo *pState, PlayerType playerType) {
        Player* pPlayer = (playerType == HUMAN) ? &(pState->self) : &(pState->enemy);

        HBRUSH brush = shipBrush;
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

                    MakeHumanMove(hwnd, pState, i, j);
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

        DrawBackground(hdcBack, pState);
        DrawGameMap(hdcBack, pState, HUMAN);
        DrawShips(hdcBack, pState, HUMAN);
        DrawShootedCells(hdcBack, pState, HUMAN);

        DrawGameMap(hdcBack, pState, COMPUTER);
        DrawShips(hdcBack, pState, COMPUTER);
        DrawShootedCells(hdcBack, pState, COMPUTER);

        BitBlt(hdc, 0, 0, pState->clientWidth, pState->clientHeight, hdcBack, 0, 0, SRCCOPY);

        EndPaint(hwnd, &ps);

        if (pState->isGameEnded) {
            FinishGame(hwnd, pState);
        }
    }

    void OnTimer(HWND hwnd, WPARAM wParam, StateInfo *pState) {
        static int i = 0, j = 0;
        if (wParam == TURN_TIMER_ID) {
            MakeComputerMove(hwnd, pState);

            InitiateRedraw(hwnd);
        } else if (wParam == LENGTH_TIMER_ID) {
            pState->gameLength++;
        }
    }

    void OnCommand(HWND hwnd, WPARAM wParam, StateInfo *pState) {
        OnMainMenuButtonsClick(hwnd, wParam, pState);
    }
}