#ifndef SEABATTLE_HANDLERS_H
#define SEABATTLE_HANDLERS_H

#include "structs.h"

void OnCreate(HWND hwnd, StateInfo *pState, HDC *hdcBack, HBITMAP *hbmBack);

void OnMouseMove(HWND hwnd, LPARAM lParam, StateInfo *pState);

void OnPaint(HWND hwnd, HDC hdcBack, StateInfo *pState);

void OnLButtonDown(HWND hwnd, LPARAM lParam, StateInfo *pState);

#endif //SEABATTLE_HANDLERS_H
