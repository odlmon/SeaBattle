#ifndef UNICODE
#define UNICODE
#endif

#include <Windows.h>
#include <new>

#include "structs.h"
#include "../pregame/handlers/handlers.h"
#include "../game/handlers/handlers.h"

using namespace std;

HDC hdcBack;
HBITMAP hbmBack;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void GenerateShipsPlace(StateInfo *pState);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow) {
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASS wc = {};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW); //в тестирование + в ИИ с убирание доступных из финишофф

    RegisterClass(&wc);

    // Create state variable

    StateInfo *pState = new(std::nothrow) StateInfo;

//    pState->width = RECT_WIDTH;
//    pState->height = RECT_HEIGHT;
    pState->gameState = PREGAME;
    pState->isDragged = FALSE;

    if (pState == NULL) {
        return 0;
    }

    // Create the window.

    HWND hwnd = CreateWindowEx(
            0,                              // Optional window styles.
            CLASS_NAME,                     // Window class
            L"SeaBattle",    // Window text
            WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,            // Window style

            // Size and position
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

            NULL,       // Parent window
            NULL,       // Menu
            hInstance,  // Instance handle
            pState        // Additional application data
    );

    if (hwnd == NULL) {
        return 1;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Run the message loop.

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

inline StateInfo *GetAppState(HWND hwnd) {
    LONG_PTR ptr = GetWindowLongPtr(hwnd, GWLP_USERDATA);
    StateInfo *pState = reinterpret_cast<StateInfo *>(ptr);
    return pState;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    StateInfo *pState;
    if (uMsg == WM_CREATE) {
        CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT *>(lParam);
        pState = reinterpret_cast<StateInfo *>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) pState);
    } else {
        pState = GetAppState(hwnd);
    }

    switch (uMsg) {
        case WM_CREATE:
            if (pState->gameState == PREGAME) {
                Pregame::OnCreate(hwnd, pState, &hdcBack, &hbmBack);
            } else {
            }
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_ERASEBKGND:
            return TRUE;

        case WM_PAINT:
            if (pState->gameState == PREGAME) {
                Pregame::OnPaint(hwnd, hdcBack, pState);
            } else {
                Game::OnPaint(hwnd, hdcBack, pState);
            }
            return 0;

        case WM_LBUTTONDOWN:
            if (pState->gameState == PREGAME) {
                Pregame::OnLButtonDown(hwnd, lParam, pState);
            } else if (pState->turn == HUMAN) {
                Game::OnLButtonDown(hwnd, lParam, pState);
            }
            return 0;

        case WM_MOUSEMOVE:
            if (pState->gameState == PREGAME) {
                Pregame::OnMouseMove(hwnd, lParam, pState);
            } else {

            }
            return 0;

        case WM_KEYDOWN:
            if (pState->gameState == PREGAME) {
                Pregame::OnKeyDown(hwnd, wParam, pState);
            } else {

            }
            return 0;

        case WM_LBUTTONUP:
            if (pState->gameState == PREGAME) {
                Pregame::OnLButtonUp(hwnd, pState);
            } else {

            }
            return 0;

        case WM_COMMAND:
            if (pState->gameState == PREGAME) {
                Pregame::OnCommand(hwnd, lParam, pState);
            } else {

            }
            return 0;

        case WM_TIMER:
            if (pState->gameState == PREGAME) {

            } else {
                Game::OnTimer(hwnd, wParam, pState);
            }
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}