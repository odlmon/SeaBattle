#ifndef SEABATTLE_CONFIG_H
#define SEABATTLE_CONFIG_H

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>

#define MAX_STR_LENGTH 200

#define IDB_BACKGROUND 32

#define IDM_NEW_GAME 1
#define IDM_STAT 2
#define IDM_SOUND 3
#define IDM_HELP 4
#define IDM_EXIT 5

#define IDS_HIT 1
#define IDS_MISS 2
#define IDS_START 3
#define IDS_RANDOM 4
#define IDS_FONT 5
#define IDS_FILE_CORRUPTED 6
#define IDS_ERROR 7
#define IDS_WIN 8
#define IDS_WIN_DESCRIPTION 9
#define IDS_LOST 10
#define IDS_LOST_DESCRIPTION 11
#define IDS_ACCURACY 12
#define IDS_TIME 13
#define IDS_BEST_TIME 14
#define IDS_GAMES 15
#define IDS_WINS 16
#define IDS_PERCENT 17
#define IDS_ANOTHER 18
#define IDS_SEC 19
#define IDS_NO_WINS 20
#define IDS_PERCENT_SIGN 21
#define IDS_STAT_FILENAME 22
#define IDS_STAT 23
#define IDS_STAT_DESCRIPTION 24
#define IDS_HELP 25
#define IDS_WELCOME 26
#define IDS_GAME_HELP 27
#define IDS_PREGAME_HELP 28
#define IDS_STAT_HELP 29
#define IDS_MENU_HELP 30
#define IDS_END 31

const int RECT_SIDE = 35;
const int COUNT_CELLS_IN_SIDE = 10;
const int SHIP_TYPE_COUNT = 4;
const int BUTTON_WIDTH = 100;
const int BUTTON_HEIGHT = 75;
const int LENGTH_TIMER_ID = 8007;
const int SECOND_DELAY = 1000;
const int TURN_TIMER_ID = 8008;
const int COMPUTER_DELAY = 1300;

const TCHAR LETTERS[10][2] = {L"A", L"B", L"C", L"D", L"E", L"F", L"G", L"H", L"I", L"J"};
const TCHAR NUMBERS[10][3] = {L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9", L"10"};

const int MAP_LINE_WIDTH = 3;
const int RESULT_LINE_WIDTH = 2;

const COLORREF SHIP_COLOR = RGB(0, 0, 0);
const COLORREF RESULT_COLOR = RGB(255, 0, 0);
const COLORREF MAP_COLOR = RGB(255, 255, 255);
const COLORREF ALLOW_COLOR = RGB(0, 255, 0);
const COLORREF FORBID_COLOR = RGB(255, 0, 0);

#endif //SEABATTLE_CONFIG_H
