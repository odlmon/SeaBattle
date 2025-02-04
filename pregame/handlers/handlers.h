#ifndef SEABATTLE_HANDLERS_PREGAME_H
#define SEABATTLE_HANDLERS_PREGAME_H

#include "../../main/structs.h"

namespace Pregame {
    void OnCreate(HWND hwnd, StateInfo *pState, HDC *hdcBack, HBITMAP *hbmBack);
    void OnMouseMove(HWND hwnd, LPARAM lParam, StateInfo *pState);
    void OnPaint(HWND hwnd, HDC hdcBack, StateInfo *pState);
    void OnLButtonDown(HWND hwnd, LPARAM lParam, StateInfo *pState);
    void OnKeyDown(HWND hwnd, WPARAM wParam, StateInfo *pState);
    void OnLButtonUp(HWND hwnd, StateInfo *pState);
    void OnCommand(HWND hwnd, LPARAM lParam, WPARAM wParam, StateInfo *pState);
}

#endif //SEABATTLE_HANDLERS_H
