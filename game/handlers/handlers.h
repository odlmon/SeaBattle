#ifndef SEABATTLE_HANDLERS_GAME_H
#define SEABATTLE_HANDLERS_GAME_H

#include "../../main/structs.h"

namespace Game {
    void OnPaint(HWND hwnd, HDC hdcBack, StateInfo *pState);
    void OnLButtonDown(HWND hwnd, LPARAM lParam, StateInfo *pState);
    void OnTimer(HWND hwnd, WPARAM wParam, StateInfo *pState);
}

#endif //SEABATTLE_HANDLERS_H
