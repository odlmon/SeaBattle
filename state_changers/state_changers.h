#ifndef SEABATTLE_STATE_CHANGERS_H
#define SEABATTLE_STATE_CHANGERS_H

#include "../main/structs.h"

void InitializeMap(StateInfo *pState);

void GenerateShipsPlace(StateInfo *pState);

void UpdateShipRect(StateInfo *pState, int x, int y, int i);

void BacklightCells(StateInfo *pState, int i);

void RotateShip(HWND hwnd, StateInfo *pState, int i);

void PlaceShip(StateInfo *pState);

#endif //SEABATTLE_STATE_CHANGERS_H
