#include "game_component_multiplayer.h"

BEGIN_SCRIPT_CLASS_INHERITED(EntityComponentGameMultiplayer, EntityComponentMultiplayer)
	SCRIPT_RESULT_METHOD(GetLobby, Lobby*)
END_SCRIPT_CLASS

DEFINE_COMPONENT_CLASS_INHERITED(GameMultiplayer,Multiplayer)
END_DEFINE_COMPONENT

EntityComponentGameMultiplayer::EntityComponentGameMultiplayer()
{
}

//------------------------------

EntityComponentGameMultiplayer::~EntityComponentGameMultiplayer()
{
}
