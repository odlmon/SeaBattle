#ifndef SEABATTLE_STATE_CHANGERS_PREGAME_H
#define SEABATTLE_STATE_CHANGERS_PREGAME_H

#include "../../main/structs.h"

void InitializeDraggedShip(StateInfo *pState, const Ship& ship, int index, int x, int y);
void UnbanCells(StateInfo *pState, int index);
void UpdateShipRect(StateInfo *pState, int x, int y, int i);
void BacklightCells(StateInfo *pState, int shipIndex);
void RotateShip(HWND hwnd, StateInfo *pState, int shipIndex);
void PlaceShip(StateInfo *pState, int shipIndex);
void CheckStartGame(StateInfo *pState);
void ResetMapAndShipsState(StateInfo *pState);
void InitializePossiblePlaces(StateInfo *pState, PlayerType playerType);
void RandomizeShipPlace(StateInfo *pState, PlayerType playerType);

#endif //SEABATTLE_STATE_CHANGERS_H
