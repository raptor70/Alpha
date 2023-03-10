#ifndef __COMPONENT_GAME_MULTIPLAYER_H__
#define __COMPONENT_GAME_MULTIPLAYER_H__

#include "game_lobby.h"
#include "multiplayer\component_multiplayer.h"

BEGIN_COMPONENT_CLASS_INHERITED(GameMultiplayer,Multiplayer)
	DECLARE_SCRIPT_CLASS(EntityComponentGameMultiplayer)
public:
	EntityComponentGameMultiplayer();
	virtual ~EntityComponentGameMultiplayer();

	virtual Lobby* NewLobby() { return NEW GameLobby; }

END_COMPONENT_CLASS

#endif
