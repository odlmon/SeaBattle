#ifndef SEABATTLE_HANDLERS_COMMON_H
#define SEABATTLE_HANDLERS_COMMON_H

#include "../../main/structs.h"

void InitiateRedraw(HWND hwnd);
BOOL DrawLine(HDC hdc, int x1, int y1, int x2, int y2);
void DrawBackground(HDC hdcBack, StateInfo *pState);
void DrawLines(HDC hdc, StateInfo *pState, PlayerType playerType);
void DrawLetters(HDC hdc, StateInfo *pState, PlayerType playerType);

#endif //SEABATTLE_HANDLERS_H
