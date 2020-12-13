#ifndef SEABATTLE_STATE_CHANGERS_H
#define SEABATTLE_STATE_CHANGERS_H

#include "../../main/structs.h"

//void InitializeMap(StateInfo *pState, PlayerType playerType);
//void GenerateShipsPlace(StateInfo *pState, PlayerType playerType);
bool CheckWinOf(HWND hwnd, StateInfo *pState, PlayerType playerType);
void UnableAdjacentCells(StateInfo *pState, int shipIndex, PlayerType playerType);
void MakeComputerMove(HWND hwnd, StateInfo *pState);

#endif //SEABATTLE_STATE_CHANGERS_H
