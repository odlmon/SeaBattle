#include "../../main/config.h"
#include "../../common/state_changers/state_changers.h"
#include "handlers.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

void InitiateRedraw(HWND hwnd) {
    RECT rect;
    GetClientRect(hwnd, &rect);
    InvalidateRect(hwnd, &rect, TRUE);
}

void SetButtons(HWND hwnd, StateInfo *pState) {
    int x = pState->clientWidth / 2 - BUTTON_WIDTH / 2;
    int y = pState->self.map.coord.bottom + 4 * RECT_SIDE;
    pState->startButton = CreateWindow(
            L"BUTTON",  // Predefined class; Unicode assumed
            GetStringFromResource(IDS_START),      // Button text
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
            GetStringFromResource(IDS_RANDOM),      // Button text
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
        DrawText(hdc, NUMBERS[i], -1, &rect, DT_CENTER);
    }
    for (int i = 0; i < COUNT_CELLS_IN_SIDE; i++) {
        RECT rect = {
                pPlayer->map.coord.left + RECT_SIDE * i,
                pPlayer->map.coord.top - RECT_SIDE,
                pPlayer->map.coord.left + RECT_SIDE * (i + 1),
                pPlayer->map.coord.top
        };
        DrawText(hdc, LETTERS[i], -1, &rect, DT_CENTER);
    }
}

wstring ConvertNumberToWString(int number) {
    wstringstream wss;
    wss << number;
    return wss.str();
}

wstring ConvertNumberToWString(float number) {
    wstringstream wss;
    wss << number;
    return wss.str();
}

void OnNewGameMenuItemClick(HWND hwnd, StateInfo *pState) {
    if (pState->gameState == GAME) {
        KillTimer(hwnd, pState->gameLengthTimer);
        KillTimer(hwnd, pState->turnTimer);

        pState->gameState = PREGAME;
        pState->self.map.cells.clear();
        InitializeMap(pState, HUMAN);
        pState->self.ships.clear();
        pState->enemy.ships.clear();
        GenerateShipsPlace(pState, HUMAN);
        SetButtons(hwnd, pState);

        InitiateRedraw(hwnd);
    } else {
        pState->self.map.cells.clear();
        InitializeMap(pState, HUMAN);
        pState->self.ships.clear();
        pState->enemy.ships.clear();
        GenerateShipsPlace(pState, HUMAN);

        InitiateRedraw(hwnd);
    }
}

void OnStatMenuItemClick(HWND hwnd, StateInfo *pState) {
    wstring text;

    text += GetStringFromResource(IDS_STAT_DESCRIPTION);
    text += GetStringFromResource(IDS_BEST_TIME) +
            (pState->stat.bestVictoryTime > 0
            ? ConvertNumberToWString(pState->stat.bestVictoryTime) + GetStringFromResource(IDS_SEC)
            : GetStringFromResource(IDS_NO_WINS));
    text += GetStringFromResource(IDS_GAMES) + ConvertNumberToWString(pState->stat.games);
    text += GetStringFromResource(IDS_WINS) + ConvertNumberToWString(pState->stat.wins);
    float winPercent = 0;
    if (pState->stat.wins != 0 || pState->stat.games != 0) {
        winPercent = floorf(((float) pState->stat.wins / (float) pState->stat.games) * 100);
    }
    text += GetStringFromResource(IDS_PERCENT) + ConvertNumberToWString(winPercent) + GetStringFromResource(IDS_PERCENT_SIGN);

    MessageBox(hwnd, text.c_str(), GetStringFromResource(IDS_STAT), MB_OK);
}

void OnSoundMenuItemClick(HWND hwnd, StateInfo *pState) {
    if (pState->isSoundOn) {
        pState->isSoundOn = false;
        CheckMenuItem(GetMenu(hwnd), IDM_SOUND, MF_UNCHECKED);
    } else {
        pState->isSoundOn = true;
        CheckMenuItem(GetMenu(hwnd), IDM_SOUND, MF_CHECKED);
    }
}

void OnHelpMenuItemClick(HWND hwnd) {
    wstring helpText = GetStringFromResource(IDS_WELCOME);
    helpText += GetStringFromResource(IDS_GAME_HELP);
    helpText += GetStringFromResource(IDS_PREGAME_HELP);
    helpText += GetStringFromResource(IDS_STAT_HELP);
    helpText += GetStringFromResource(IDS_MENU_HELP);
    helpText += GetStringFromResource(IDS_END);
    MessageBox(hwnd, helpText.c_str(), GetStringFromResource(IDS_HELP), MB_OK);
}

void OnMainMenuButtonsClick(HWND hwnd, WPARAM wParam, StateInfo *pState) {
    switch (wParam) {
        case IDM_NEW_GAME:
            OnNewGameMenuItemClick(hwnd, pState);
            break;
        case IDM_STAT:
            OnStatMenuItemClick(hwnd, pState);
            break;
        case IDM_SOUND:
            OnSoundMenuItemClick(hwnd, pState);
            break;
        case IDM_HELP:
            OnHelpMenuItemClick(hwnd);
            break;
        case IDM_EXIT:
            PostMessage(hwnd, WM_CLOSE, 0, 0);
            break;
    }
}

void OnDestroy(StateInfo *pState) {
    string output = std::to_string(pState->stat.bestVictoryTime) + " " + std::to_string(pState->stat.wins) + " " +
                    std::to_string(pState->stat.games);
    ofstream file;
    file.open(GetStringFromResource(IDS_STAT_FILENAME));
    if (file.fail()) {
        SetFileAttributes(GetStringFromResource(IDS_STAT_FILENAME), FILE_ATTRIBUTE_NORMAL);
        file.open(GetStringFromResource(IDS_STAT_FILENAME));
    }
    file << output;
    file.close();
}
