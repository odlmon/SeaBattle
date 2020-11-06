#ifndef UNICODE
#define UNICODE
#endif

#include "../main/config.h"
#include "handlers.h"
#include "../state_changers/state_changers.h"

#include <windowsx.h>

const TCHAR *letters[] = {L"A", L"B", L"C", L"D", L"E", L"F", L"G", L"H", L"I", L"J"};
const TCHAR *numbers[] = {L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9", L"10"};

HBRUSH black = CreateSolidBrush(RGB(0, 0, 0));
HBRUSH green = CreateSolidBrush(RGB(0, 255, 0));
HBRUSH red = CreateSolidBrush(RGB(255, 0, 0));
HBRUSH back = CreateSolidBrush(RGB(0, 213, 255));

void OnCreate(HWND hwnd, StateInfo *pState, HDC *hdcBack, HBITMAP *hbmBack) {
    RECT winRect;

    if (GetClientRect(hwnd, &winRect)) {
        int width = winRect.right - winRect.left;
        int height = winRect.bottom - winRect.top;
        pState->clientWidth = width;
        pState->clientHeight = height;

        InitializeMap(pState);

        GenerateShipsPlace(pState);

        //Double-buffering
        HDC hdc = GetDC(hwnd);
        *hdcBack = CreateCompatibleDC(hdc);
        *hbmBack = CreateCompatibleBitmap(hdc, pState->clientWidth, pState->clientHeight);
        ReleaseDC(hwnd, hdc);
        SaveDC(*hdcBack);
        SelectObject(*hdcBack, *hbmBack);
    }
}

void InitiateRedraw(HWND hwnd) {
    RECT rect;
    GetClientRect(hwnd, &rect);
    InvalidateRect(hwnd, &rect, TRUE);
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

BOOL DrawLine(HDC hdc, int x1, int y1, int x2, int y2) {
    POINT pt;
    MoveToEx(hdc, x1, y1, &pt);
    return LineTo(hdc, x2, y2);
}

void DrawLines(HDC hdc, StateInfo *pState) {
    for (int i = 0; i <= COUNT_CELLS_IN_SIDE; i++) {
        int x = pState->self.map.coord.left + RECT_SIDE * i;
        DrawLine(hdc, x, pState->self.map.coord.top, x, pState->self.map.coord.bottom);
    }
    for (int i = 0; i <= COUNT_CELLS_IN_SIDE; i++) {
        int y = pState->self.map.coord.top + RECT_SIDE * i;
        DrawLine(hdc, pState->self.map.coord.left, y, pState->self.map.coord.right, y);
    }
}

void DrawLetters(HDC hdc, StateInfo *pState) {
    for (int i = 0; i < COUNT_CELLS_IN_SIDE; i++) {
        RECT rect = {
                pState->self.map.coord.left - RECT_SIDE,
                pState->self.map.coord.top + RECT_SIDE * i,
                pState->self.map.coord.left,
                pState->self.map.coord.top + RECT_SIDE * (i + 1)
        };
        DrawText(hdc, numbers[i], -1, &rect, DT_CENTER);
    }
    for (int i = 0; i < COUNT_CELLS_IN_SIDE; i++) {
        RECT rect = {
                pState->self.map.coord.left + RECT_SIDE * i,
                pState->self.map.coord.top - RECT_SIDE,
                pState->self.map.coord.left + RECT_SIDE * (i + 1),
                pState->self.map.coord.top
        };
        DrawText(hdc, letters[i], -1, &rect, DT_CENTER);
    }
}

void DrawBacklightedCells(HDC hdc, StateInfo *pState) {
    //maybe add global flag if ship in map coords for optimization
    for (int i = 0; i < COUNT_CELLS_IN_SIDE; i++) {
        for (int j = 0; j < COUNT_CELLS_IN_SIDE; j++) {
            if (pState->self.map.cells[i][j].isVisualized) {
                HBRUSH brush;
                if (pState->self.map.cells[i][j].isAvailable && !pState->self.map.cells[i][j].isPartial) {
                    brush = green;
                } else {
                    brush = red;
                }
                FillRect(hdc, &pState->self.map.cells[i][j].rect, brush);
            }
        }
    }
}

void DrawGameMap(HDC hdc, StateInfo *pState) {
    HPEN hPen = CreatePen(PS_SOLID, 3, RGB(255, 255, 255));
    HPEN oldPen = (HPEN) SelectObject(hdc, hPen);

    HFONT hFont = CreateFont(RECT_SIDE, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE,
                             ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                             DEFAULT_PITCH | FF_SWISS, L"Arial");
    HFONT oldFont = (HFONT) SelectObject(hdc, hFont);
    SetTextColor(hdc, RGB(255, 255, 255));
    SetBkMode(hdc, TRANSPARENT);

    DrawBacklightedCells(hdc, pState);

    DrawLines(hdc, pState);

    DrawLetters(hdc, pState);

    SetBkMode(hdc, OPAQUE);
    SelectObject(hdc, oldFont);
    DeleteObject(hFont);

    SelectObject(hdc, oldPen);
    DeleteObject(hPen);
}

void DrawShips(HDC hdc, StateInfo *pState) {
    HBRUSH brush = black;
    auto iter = pState->self.ships.begin();
    while (iter != pState->self.ships.end()) {
        FillRect(hdc, &iter->rect, brush);
        ++iter;
    }
}

void OnPaint(HWND hwnd, HDC hdcBack, StateInfo *pState) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    HBRUSH brush = back;
    FillRect(hdcBack, &ps.rcPaint, brush);

    DrawGameMap(hdcBack, pState);
    DrawShips(hdcBack, pState);

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
        PlaceShip(pState);

        BacklightCells(pState, pState->draggedShip.index);

        InitiateRedraw(hwnd);
    }
}