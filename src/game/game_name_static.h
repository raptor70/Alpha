#ifndef __GAME_NAME_STATIC_H__
#define __GAME_NAME_STATIC_H__

#ifndef __NAME_DEFINE__
#include "core/name.h"

#define DECLARE_NAME(name)	extern Name g_n##name;
#define NAME(name) g_n##name
void GAMENAME_RemoveAll();
#endif

//DECLARE_NAME(NONE)
DECLARE_NAME(StepTowerPlacementDuration)
DECLARE_NAME(StepShootDuration)
DECLARE_NAME(StepWallsPlacementDuration)

#endif
