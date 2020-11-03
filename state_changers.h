#ifndef SEABATTLE_STATE_CHANGERS_H
#define SEABATTLE_STATE_CHANGERS_H

#include "structs.h"

void InitializeMap(StateInfo *pState);

void GenerateShipsPlace(StateInfo *pState);

void UpdateShipRect(StateInfo *pState, int x, int y, int i);

void BacklightCells(StateInfo *pState, int i);

void RotateShip(HWND hwnd, StateInfo *pState, int i);

#endif //SEABATTLE_STATE_CHANGERS_H
