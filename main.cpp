#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <windowsx.h>
#include <new>
#include <vector>

using namespace std;

struct Ship { //???only pregame
    int width, height; //in pixels for away from window logic
    int type; //1 to 4
    RECT rect; //global coords of ship
    BOOL isOnMap; //pregame status + use for oldRect + can use when change place on map
};

struct DraggedShip {
    int index;
    RECT oldRect;
    int deltaLeft, deltaTop, deltaRight, deltaBottom;
};

struct MapCell { //???for in-game mechanics
    RECT rect; //global coords of cell
    int type; // 0 if no ship
    BOOL isAvailable; // 0 + false -> adjacent cell
    BOOL isAttempted; // true if was shooted
};
//can have counter with accurate shoots for win determination
struct Map {
    RECT coord; //coord of edges
    int side; //side in pixels
    vector<vector<MapCell>> cells; //matrix of cells
};

struct Player {
    Map map;
    vector<Ship> ships;
};

struct StateInfo {
    int clientWidth, clientHeight;
    Player self, enemy;
    BOOL isDragged;
    DraggedShip draggedShip;
};

const int RECT_SIDE = 25;
const int COUNT_CELLS_IN_SIDE = 10;
const TCHAR *letters[] = {L"A", L"B", L"C", L"D", L"E", L"F", L"G", L"H", L"I", L"J"};
const TCHAR *numbers[] = {L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9", L"10"};

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

    RegisterClass(&wc);

    // Create state variable

    StateInfo *pState = new(std::nothrow) StateInfo;

//    pState->width = RECT_WIDTH;
//    pState->height = RECT_HEIGHT;
    pState->isDragged = FALSE;

    if (pState == NULL) {
        return 0;
    }

    // Create the window.

    HWND hwnd = CreateWindowEx(
            0,                              // Optional window styles.
            CLASS_NAME,                     // Window class
            L"SeaBattle",    // Window text
            WS_OVERLAPPEDWINDOW,            // Window style

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

    // Run the message loop.

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

void InitializeMap(StateInfo *pState) {
    pState->self.map = Map{{}, RECT_SIDE * COUNT_CELLS_IN_SIDE};
    pState->self.map.coord = {
            pState->clientWidth / 4 - pState->self.map.side / 2,
            pState->clientHeight / 2 - pState->self.map.side / 2,
            pState->clientWidth / 4 + pState->self.map.side / 2,
            pState->clientHeight / 2 + pState->self.map.side / 2,
    };

    vector<MapCell> row;
    row.reserve(COUNT_CELLS_IN_SIDE);
    for (int i = 0; i < COUNT_CELLS_IN_SIDE; i++) {
        for (int j = 0; j < COUNT_CELLS_IN_SIDE; j++) {
            row.push_back({{
                pState->self.map.coord.left + RECT_SIDE * j,
                pState->self.map.coord.top + RECT_SIDE * i,
                pState->self.map.coord.left + RECT_SIDE * (j + 1),
                pState->self.map.coord.top + RECT_SIDE * (i + 1)
                }, 0, true, false});
        }
        pState->self.map.cells.push_back(row);
        row.clear();
    }
}

void GenerateShipsPlace(StateInfo *pState) {
    int x = pState->clientWidth * 3 / 4;
    int y = pState->clientHeight / 2;

    Ship ship = {};
    ship.height = RECT_SIDE;

    ship.type = 3;
    ship.width = RECT_SIDE * ship.type;
    ship.rect = {x - RECT_SIDE * 7 / 2, y - RECT_SIDE * 3 / 2, x - RECT_SIDE / 2, y - RECT_SIDE / 2};
    pState->self.ships.push_back(ship);
    ship.rect = {x + RECT_SIDE / 2, y - RECT_SIDE * 3 / 2, x + RECT_SIDE * 7 / 2, y - RECT_SIDE / 2};
    pState->self.ships.push_back(ship);

    ship.type = 2;
    ship.width = RECT_SIDE * ship.type;
    ship.rect = {x - RECT_SIDE, y + RECT_SIDE * 1 / 2, x + RECT_SIDE, y + RECT_SIDE * 3 / 2};
    pState->self.ships.push_back(ship);
    ship.rect = {x - RECT_SIDE * 4, y + RECT_SIDE * 1 / 2, x - RECT_SIDE * 2, y + RECT_SIDE * 3 / 2};
    pState->self.ships.push_back(ship);
    ship.rect = {x + RECT_SIDE * 2, y + RECT_SIDE * 1 / 2, x + RECT_SIDE * 4, y + RECT_SIDE * 3 / 2};
    pState->self.ships.push_back(ship);

    ship.type = 4;
    ship.width = RECT_SIDE * ship.type;
    ship.rect = {x - RECT_SIDE * 2, y - RECT_SIDE * 7 / 2, x + RECT_SIDE * 2, y - RECT_SIDE * 5 / 2};
    pState->self.ships.push_back(ship);

    ship.type = 1;
    ship.width = RECT_SIDE * ship.type;
    ship.rect = {x - RECT_SIDE * 3 / 2, y + RECT_SIDE * 5 / 2, x - RECT_SIDE / 2, y + RECT_SIDE * 7 / 2};
    pState->self.ships.push_back(ship);
    ship.rect = {x - RECT_SIDE * 7 / 2, y + RECT_SIDE * 5 / 2, x - RECT_SIDE * 5 / 2, y + RECT_SIDE * 7 / 2};
    pState->self.ships.push_back(ship);
    ship.rect = {x + RECT_SIDE / 2, y + RECT_SIDE * 5 / 2, x + RECT_SIDE * 3 / 2, y + RECT_SIDE * 7 / 2};
    pState->self.ships.push_back(ship);
    ship.rect = {x + RECT_SIDE * 5 / 2, y + RECT_SIDE * 5 / 2, x + RECT_SIDE * 7 / 2, y + RECT_SIDE * 7 / 2};
    pState->self.ships.push_back(ship);

}

inline StateInfo *GetAppState(HWND hwnd) {
    LONG_PTR ptr = GetWindowLongPtr(hwnd, GWLP_USERDATA);
    StateInfo *pState = reinterpret_cast<StateInfo *>(ptr);
    return pState;
}

void InitiateRedraw(HWND hwnd) {
    RECT rect;
    GetClientRect(hwnd, &rect);
    InvalidateRect(hwnd, &rect, TRUE);
}

BOOL DrawLine(HDC hdc, int x1, int y1, int x2, int y2) {
    POINT pt;
    MoveToEx(hdc, x1, y1, &pt);
    return LineTo(hdc, x2, y2);
}

void DrawTable(HDC hdc, StateInfo *pState) {
    HPEN hPen = CreatePen(PS_SOLID, 3, RGB(255, 255, 255));
    HPEN oldPen = (HPEN) SelectObject(hdc, hPen);

    HFONT hFont = CreateFont(RECT_SIDE, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE,
                             ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                             DEFAULT_PITCH | FF_SWISS, L"Arial");
    HFONT oldFont = (HFONT) SelectObject(hdc, hFont);
    SetTextColor(hdc, RGB(255, 255, 255));
    SetBkMode(hdc, TRANSPARENT);

    for (int i = 0; i <= COUNT_CELLS_IN_SIDE; i++) {
        int x = pState->self.map.coord.left + RECT_SIDE * i;
        DrawLine(hdc, x, pState->self.map.coord.top, x, pState->self.map.coord.bottom);
    }
    for (int i = 0; i <= COUNT_CELLS_IN_SIDE; i++) {
        int y = pState->self.map.coord.top + RECT_SIDE * i;
        DrawLine(hdc, pState->self.map.coord.left, y, pState->self.map.coord.right, y);
    }

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

    SetBkMode(hdc, OPAQUE);
    SelectObject(hdc, oldFont);
    DeleteObject(hFont);

    SelectObject(hdc, oldPen);
    DeleteObject(hPen);
}

void DrawShips(HDC hdc, StateInfo *pState) {
    HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
    auto iter = pState->self.ships.begin();
    while (iter != pState->self.ships.end()) {
        FillRect(hdc, &iter->rect, brush);
        ++iter;
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    StateInfo *pState;
    if (uMsg == WM_CREATE) {
        CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT *>(lParam);
        pState = reinterpret_cast<StateInfo *>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR) pState);

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
            hdcBack = CreateCompatibleDC(hdc);
            hbmBack = CreateCompatibleBitmap(hdc, pState->clientWidth, pState->clientHeight);
            ReleaseDC(hwnd, hdc);
            SaveDC(hdcBack);
            SelectObject(hdcBack, hbmBack);
        }
    } else {
        pState = GetAppState(hwnd);
    }

    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_ERASEBKGND:
            return TRUE;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            HBRUSH brush = CreateSolidBrush(RGB(0, 213, 255));
            FillRect(hdcBack, &ps.rcPaint, brush);

            DrawTable(hdcBack, pState);
            DrawShips(hdcBack, pState);

            BitBlt(hdc, 0, 0, pState->clientWidth, pState->clientHeight, hdcBack, 0, 0, SRCCOPY);

            EndPaint(hwnd, &ps);
        }
            return 0;

        case WM_LBUTTONDOWN: {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            POINT point = {x, y};

            if (DragDetect(hwnd, point)) {
                BOOL isNotFound = true;
                int index = 0;
                auto iter = pState->self.ships.begin();
                while (iter != pState->self.ships.end() && isNotFound) {
                    if (x >= iter->rect.left && x <= iter->rect.right && y >= iter->rect.top && y <= iter->rect.bottom) {
                        isNotFound = false;
                        pState->isDragged = true;
                        pState->draggedShip = {
                                index,
                                iter->rect,
                                iter->rect.left - x,
                                iter->rect.top - y,
                                iter->rect.right - x,
                                iter->rect.bottom - y
                        };
                    }
                    index++;
                    ++iter;
                }
            }
        }
            return 0;

        case WM_MOUSEMOVE: {
            if (pState->isDragged) {
                int x = GET_X_LPARAM(lParam);
                int y = GET_Y_LPARAM(lParam);
                int i = pState->draggedShip.index;

                pState->self.ships[i].rect = {
                        x + pState->draggedShip.deltaLeft,
                        y + pState->draggedShip.deltaTop,
                        x + pState->draggedShip.deltaRight,
                        y + pState->draggedShip.deltaBottom
                };

                if (pState->self.ships[i].rect.right > pState->clientWidth) {
                    pState->self.ships[i].rect.right = pState->clientWidth;
                    pState->self.ships[i].rect.left = pState->self.ships[i].rect.right - pState->self.ships[i].width;
                }
                if (pState->self.ships[i].rect.left < 0) {
                    pState->self.ships[i].rect.left = 0;
                    pState->self.ships[i].rect.right = pState->self.ships[i].width;
                }
                if (pState->self.ships[i].rect.top < 0) {
                    pState->self.ships[i].rect.top = 0;
                    pState->self.ships[i].rect.bottom = pState->self.ships[i].height;
                }
                if (pState->self.ships[i].rect.bottom > pState->clientHeight) {
                    pState->self.ships[i].rect.bottom = pState->clientHeight;
                    pState->self.ships[i].rect.top = pState->clientHeight - pState->self.ships[i].height;
                }

                for (int m = 0; m < COUNT_CELLS_IN_SIDE; m++) {
                    for (int n = 0; n < COUNT_CELLS_IN_SIDE; n++) { // find 3 max squares
                        int a = pState->self.ships[i].rect.bottom - pState->self.map.cells[m][n].rect.top;
                        int b = pState->self.ships[i].rect.right - pState->self.map.cells[m][n].rect.left;
                        int c = pState->self.map.cells[m][n].rect.bottom - pState->self.ships[i].rect.top;
                        int d = pState->self.map.cells[m][n].rect.right - pState->self.ships[i].rect.left;

                        if (a >= 0 && a <= RECT_SIDE && b >= 0 && b <= RECT_SIDE) {

                        }

                        if (c >= 0 && c <= RECT_SIDE && b >= 0 && b <= RECT_SIDE) {

                        }

                        if (a >= 0 && a <= RECT_SIDE && d >= 0 && d <= RECT_SIDE) {

                        }

                        if (c >= 0 && c <= RECT_SIDE && d >= 0 && d <= RECT_SIDE) {

                        }
                    }
                }

                InitiateRedraw(hwnd);
            }
        }
            return 0;

        case WM_LBUTTONUP: {
            pState->isDragged = FALSE;
        }
            return 0;

            //case WM_CLOSE:
            //  if (MessageBox(hwnd, L"Really quit?", L"My application", MB_OKCANCEL) == IDOK)
            //{
            //DestroyWindow(hwnd);
            //}
            // Else: User canceled. Do nothing.
            //return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}