#ifndef SEABATTLE_HANDLERS_COMMON_H
#define SEABATTLE_HANDLERS_COMMON_H

#include <string>
#include "../../main/structs.h"

void InitiateRedraw(HWND hwnd);
void SetButtons(HWND hwnd, StateInfo *pState);
BOOL DrawLine(HDC hdc, int x1, int y1, int x2, int y2);
void DrawBackground(HDC hdcBack, StateInfo *pState);
void DrawLines(HDC hdc, StateInfo *pState, PlayerType playerType);
void DrawLetters(HDC hdc, StateInfo *pState, PlayerType playerType);
std::wstring ConvertNumberToWString(int number);
std::wstring ConvertNumberToWString(float number);
void OnMainMenuButtonsClick(HWND hwnd, WPARAM wParam, StateInfo *pState);
void OnDestroy(StateInfo *pState);

#endif //SEABATTLE_HANDLERS_H
