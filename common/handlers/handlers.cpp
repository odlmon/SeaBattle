#ifndef UNICODE
#define UNICODE
#endif

#include "../../main/config.h"
#include "handlers.h"

const TCHAR *letters[] = {L"A", L"B", L"C", L"D", L"E", L"F", L"G", L"H", L"I", L"J"};
const TCHAR *numbers[] = {L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9", L"10"};

HBRUSH black = CreateSolidBrush(RGB(0, 0, 0));
HBRUSH back = CreateSolidBrush(RGB(0, 213, 255));

void InitiateRedraw(HWND hwnd) {
    RECT rect;
    GetClientRect(hwnd, &rect);
    InvalidateRect(hwnd, &rect, TRUE);
}

void DrawBackground(HDC hdcBack, StateInfo *pState) {
    HDC hdcMem = CreateCompatibleDC(hdcBack);
    HGDIOBJ oldBitmap = SelectObject(hdcMem, pState->background);

    BITMAP background;
    GetObject(pState->background, sizeof(BITMAP), &background);
    StretchBlt(hdcBack, 0, 0, pState->clientWidth, pState->clientHeight, hdcMem, 0, 0,
               background.bmWidth, background.bmHeight, SRCCOPY);

    SelectObject(hdcMem, oldBitmap);
    DeleteDC(hdcMem);
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
