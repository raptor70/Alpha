#undef __GAME_NAME_STATIC_H__
#define __NAME_DEFINE__
#define DECLARE_NAME(name)	Name g_n##name = #name;
#include "game_name_static.h"
#undef DECLARE_NAME

void GAMENAME_RemoveAll()
{
#undef __GAME_NAME_STATIC_H__
#define DECLARE_NAME(name)	g_n##name = Name::Null;
#include "game_name_static.h"
#undef DECLARE_NAME
}