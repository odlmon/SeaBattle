#ifndef SEABATTLE_STATE_CHANGERS_COMMON_H
#define SEABATTLE_STATE_CHANGERS_COMMON_H

#include "../../main/structs.h"

void InitializeMap(StateInfo *pState, PlayerType playerType);
void GenerateShipsPlace(StateInfo *pState, PlayerType playerType);
LPWSTR GetStringFromResource(UINT resourceId);
void PlayShotSound(StateInfo *pState, UINT resourceId);

#endif //SEABATTLE_STATE_CHANGERS_H
