#include "game_lobby.h"

#include "input\input_manager.h"
#include "grid\component_game_grid.h"
#include "game_name_static.h"

BEGIN_SCRIPT_CLASS_INHERITED(GameLobby, Lobby)
	SCRIPT_VOID_METHOD(GoToGame)
	SCRIPT_RESULT_METHOD(GetTimeLeft,Float)
END_SCRIPT_CLASS

GameLobby::GameLobby():SUPER()
{
	SetMaxPlayer(4);
	m_WantedState = STATE_ADDPLAYERS;
	m_State = STATE_NONE;
	m_bLocalPrepared = FALSE;
}

//------------------------------

GameLobby::~GameLobby()
{
}

//------------------------------

void GameLobby::Update(Float _dTime, NetworkPtr& _pNetwork)
{
	SUPER::Update(_dTime, _pNetwork);

	for (U32 p = 0; p<m_Players.GetCount(); p++)
	{
		PlayerPtr& player = m_Players[p];
		if (player.IsValid())
		{
			if (player->IsLocal()) // send pos to all other
			{
				GamePlayer* gameplayer = player.GetCastPtr<GamePlayer>();
				Vec3f pos = gameplayer->GetWorldPos();
				SNetworkMessage message(MESSAGE_UpdatePlayerPos, TRUE);
				message.m_Stream.WriteU8((U8)gameplayer->GetController());
				message.m_Stream.WriteFloat(pos.x);
				message.m_Stream.WriteFloat(pos.y);
				message.m_Stream.WriteFloat(pos.z);
				SendPacketToOthers(_pNetwork,message);
			}
		}
	}

	UpdateState(_dTime);
}

//------------------------------

void	GameLobby::ReadPacket(NetworkPtr& _pNetwork, const NetworkUserID& _senderID, SNetworkMessage& _message)
{
	SUPER::ReadPacket(_pNetwork, _senderID, _message);

	switch (_message.m_iType)
	{
		case MESSAGE_UpdatePlayerPos:
		{
			Player::Controller controller = (Player::Controller)_message.m_Stream.ReadU8();
			Vec3f pos;
			pos.x = _message.m_Stream.ReadFloat();
			pos.y = _message.m_Stream.ReadFloat();
			pos.z = _message.m_Stream.ReadFloat();
			for (U32 p = 0; p < m_Players.GetCount(); p++)
			{
				PlayerPtr& player = m_Players[p];
				if (player.IsValid())
				{
					GamePlayer* gameplayer = player.GetCastPtr<GamePlayer>();
					if (gameplayer->IsRemote() && gameplayer->GetRemoteUserID() == _senderID && gameplayer->GetController() == controller)
					{
						gameplayer->SetWorldPos(pos);
						break;
					}
				}
			}
			break;
		}
		case MESSAGE_Forts:
		{
			U8 playerId = _message.m_Stream.ReadU8();
			U32 count = _message.m_Stream.ReadU8();
			Vec2iArray forts;
			for (U8 i=0; i<count;i++)
			{
				Vec2i pos;
				pos.x = _message.m_Stream.ReadU16();
				pos.y = _message.m_Stream.ReadU16();
				forts.AddLastItem(pos);
			}

			EntityComponentGameGrid* grid = World::GetInstance().GetComponent<EntityComponentGameGrid>();
			if (grid && m_bLocalPrepared)
			{
				grid->PrepareForts(forts);
				DEBUG_Require(!m_PlayerPrepared[playerId]);
				m_PlayerPrepared[playerId] = TRUE;
			}
			else
			{
				SFortDefinition& def = m_WaitingPlayerForts.AddLastItem();
				def.m_PlayerId = playerId;
				def.m_Forts = forts;
			}
			break;
		}
		case MESSAGE_Walls:
		{
			EntityComponentGameGrid* grid = World::GetInstance().GetComponent<EntityComponentGameGrid>();
			if (grid)
			{
				U32 sizeX = grid->GetGridSizeX();
				U8 playerId = _message.m_Stream.ReadU8();
				U32 count = _message.m_Stream.ReadU16();
				Vec2iArray walls;
				for (U8 i = 0; i < count; i++)
				{
					U16 idx = _message.m_Stream.ReadU16();
					Vec2i pos;
					pos.x = idx % sizeX;
					pos.y = idx / sizeX;
					walls.AddLastItem(pos);
				}

				for (U32 x = 0; x<sizeX; x++)
					for (U32 y = 0; y < grid->GetGridSizeY(); y++)
					{
						GameGridCell& cell = grid->GetCell(x, y);
						if (!cell.IsForPlayer(playerId))
							continue;

						Vec2i curPos(x,y);
						if (walls.Find(curPos)) // is a wall 
						{
							if ((cell.m_iGameFlag & GAME_GRID_CELL_WALL)==0) // ..but was not
							{
								grid->PlaceWall(curPos);
							}
						}
						else // not a wall
						{
							if (cell.m_iGameFlag & GAME_GRID_CELL_WALL) // ..but was
							{
								grid->RemoveWall(curPos);
							}
						}
					}
			}
			break;
		}
		case MESSAGE_Towers:
		{
			EntityComponentGameGrid* grid = World::GetInstance().GetComponent<EntityComponentGameGrid>();
			if (grid)
			{
				U32 flags = (GAME_GRID_CELL_TOWER | GAME_GRID_CELL_REF);
				U32 sizeX = grid->GetGridSizeX();
				U8 playerId = _message.m_Stream.ReadU8();
				U32 count = _message.m_Stream.ReadU16();
				Vec2iArray towers;
				for (U8 i = 0; i < count; i++)
				{
					U16 idx = _message.m_Stream.ReadU16();
					Vec2i pos;
					pos.x = idx % sizeX;
					pos.y = idx / sizeX;
					towers.AddLastItem(pos);
				}

				for (U32 x = 0; x<sizeX; x++)
					for (U32 y = 0; y < grid->GetGridSizeY(); y++)
					{
						GameGridCell& cell = grid->GetCell(x, y);
						if (!cell.IsForPlayer(playerId))
							continue;

						Vec2i curPos(x, y);
						if (towers.Find(curPos)) // is a tower 
						{
							if ((cell.m_iGameFlag & flags) == 0) // ..but was not
							{
								grid->PlaceTower(curPos);
							}
						}
						else // not a tower
						{
							if ((cell.m_iGameFlag & flags) == flags) // ..but was
							{
								grid->RemoveTower(curPos);
							}
						}
					}
			}
			break;
		}
		case MESSAGE_Damage:
		{
			EntityComponentGameGrid* grid = World::GetInstance().GetComponent<EntityComponentGameGrid>();
			if (grid)
			{
				U8 senderId = _message.m_Stream.ReadU8();
				U8 targetId = _message.m_Stream.ReadU8();
				U16 posIdx = _message.m_Stream.ReadU16();
				U32 sizeX = grid->GetGridSizeX();
				Vec2i pos;
				pos.x = posIdx % sizeX;
				pos.y = posIdx / sizeX;
				ProcessDamage(pos);
			}
		}
	}
}

//------------------------------


void GameLobby::DrawDebug(S32& _y, const NetworkPtr& _pNetwork)
{
	SUPER::DrawDebug(_y, _pNetwork);

	Str state;
	switch (m_State)
	{
	case STATE_NONE:
	{
		state = "none";
		break;
	}
	case STATE_ADDPLAYERS:
	{
		state = "AddPlayers";
		break;
	}
	case STATE_PREPAREGAME:
	{
		state = "PrepareGame";
		break;
	}
	case STATE_FORTCHOICE:
	{
		state = "FortChoice";
		break;
	}
	case STATE_TOWERPLACEMENT:
	{
		state = "TowerPlacement";
		break;
	}
	case STATE_SHOOT:
	{
		state = "Shoot";
		break;
	}
	case STATE_WALLS:
	{
		state = "Walls";
		break;
	}
	case STATE_GAMEOVER:
	{
		state = "GameOver";
		break;
	}
	default:
	{
		state = "Unknown";
		break;
	}
	}

	Str line;
	line.SetFrom("STATE %s (%.2f)", state.GetArray(),m_fTimeInState);
	DEBUG_DrawText(line.GetArray(), 200, _y, Color::Grey);
	_y += 16;
}

//------------------------------

void GameLobby::FirstFrameInState()
{
	switch (m_State)
	{
	case STATE_PREPAREGAME:
	{
		m_bLocalPrepared = FALSE;

		break;
	}
	case STATE_FORTCHOICE:
	{
		for (U32 p = 0; p < m_Players.GetCount(); p++)
		{
			PlayerPtr& player = m_Players[p];
			if (player.IsValid())
			{
				GamePlayer* gameplayer = player.GetCastPtr<GamePlayer>();
				gameplayer->SetToSelectFort();
			}
		}
		break;
	}

	case STATE_TOWERPLACEMENT:
	{
		m_fMaxTime = WORLDPARAMFLOAT(StepWallsPlacementDuration);
		EntityComponentGameGrid* grid = World::GetInstance().GetComponent<EntityComponentGameGrid>();
		if (grid && grid->IsPrepared())
		{
			grid->GetGrid().ComputeInside();
			grid->UpdateGroundColor(TRUE);
		}

		for (U32 p = 0; p < m_Players.GetCount(); p++)
		{
			PlayerPtr& player = m_Players[p];
			if (player.IsValid())
			{
				GamePlayer* gameplayer = player.GetCastPtr<GamePlayer>();
				gameplayer->SetToPlaceTower();
			}
		}
		break;
	}

	case STATE_SHOOT:
	{
		m_fMaxTime = WORLDPARAMFLOAT(StepShootDuration);
		for (U32 p = 0; p < m_Players.GetCount(); p++)
		{
			PlayerPtr& player = m_Players[p];
			if (player.IsValid())
			{
				GamePlayer* gameplayer = player.GetCastPtr<GamePlayer>();
				gameplayer->SetToShoot();
			}
		}
		break;
	}

	case STATE_WALLS:
	{
		m_fMaxTime = WORLDPARAMFLOAT(StepWallsPlacementDuration);
		EntityComponentGameGrid* grid = World::GetInstance().GetComponent<EntityComponentGameGrid>();
		if (grid && grid->IsPrepared())
		{
			grid->UpdateGroundColor(TRUE);
		}

		for (U32 p = 0; p < m_Players.GetCount(); p++)
		{
			PlayerPtr& player = m_Players[p];
			if (player.IsValid())
			{
				GamePlayer* gameplayer = player.GetCastPtr<GamePlayer>();
				gameplayer->SetToPlaceWall();
			}
		}
		break;
	}
	}
}

//------------------------------

void GameLobby::UpdateState(Float _dTime)
{
	// change state
	if (m_WantedState != STATE_NONE)
	{
		LastFrameInState(); 
		m_State = m_WantedState;
		m_fTimeInState = 0.f;
		FirstFrameInState();
		m_WantedState = STATE_NONE;
	}

	// update
	m_fTimeInState += _dTime;
	switch (m_State)
	{
	case STATE_ADDPLAYERS:
	{
		// register
		DeviceKeyboard& keyboard = InputManager::GetInstance().GetKeyboard();

		if (keyboard.IsJustPressed(KEY_Enter))
		{
			RegisterPlayer(m_pNetwork, Player::CONTROLLER_KeyboardMouse);
		}
		for (S32 p = 0; p < MAX_PAD_COUNT; p++)
		{
			if (InputManager::GetInstance().HasPad(p))
			{
				DevicePad& pad = InputManager::GetInstance().GetPad(p);
				if (pad.IsJustPressed(PADKEY_Start))
				{
					RegisterPlayer(m_pNetwork, (Player::Controller)(Player::CONTROLLER_Pad0 + p));
				}
			}
		}

		break;
	}
	case STATE_STARTGAME:
	{
		if (m_fTimeInState > 3.f)
		{
			Application::GetInstance().GetFlowScriptManager()->LaunchEvent("TO_GAME_SCRIPT");
			m_WantedState = STATE_PREPAREGAME;
		}
		break;
	}
	case STATE_PREPAREGAME:
	{
		if (!m_bLocalPrepared)
		{
			EntityComponentGameGrid* grid = World::GetInstance().GetComponent<EntityComponentGameGrid>();
			if (grid)
			{
				for (U32 p = 0; p < m_Players.GetCount(); p++)
				{
					PlayerPtr& player = m_Players[p];
					Bool bPrepared = FALSE;
					if (!player.IsValid())
					{
						bPrepared = TRUE;
					}
					else if(player->IsLocal())
					{
						Vec2iArray fortPos;
						grid->PrepareFortsForPlayer((U8)p, fortPos);
						grid->PrepareForts(fortPos);
						BroadcastForts(m_pNetwork, p, fortPos);
						bPrepared = TRUE;
					}

					m_PlayerPrepared.AddLastItem(bPrepared);
				}

				// setup waiting player forts
				for (SFortDefinition& def : m_WaitingPlayerForts)
				{
					grid->PrepareForts(def.m_Forts);
					DEBUG_Require(!m_PlayerPrepared[def.m_PlayerId]);
					m_PlayerPrepared[def.m_PlayerId] = TRUE;					
				}

				m_bLocalPrepared = TRUE;
			}
		}
		else
		{
			Bool bAllPrepared = TRUE;
			for (Bool bPrepared : m_PlayerPrepared)
				if (!bPrepared)
				{
					bAllPrepared = FALSE;
					break;
				}

			if (bAllPrepared)
			{
				m_WantedState = STATE_FORTCHOICE;
			}
		}
		break;
	}
	case STATE_FORTCHOICE:
	{
		Bool bAllReady = TRUE;
		for (U32 p = 0; p < m_Players.GetCount(); p++)
		{
			PlayerPtr& player = m_Players[p];
			if (player.IsValid())
			{
				GamePlayer* gameplayer = player.GetCastPtr<GamePlayer>();
				if (!gameplayer->IsFortSelected())
				{
					bAllReady = FALSE;
					break;
				}
			}
		}

		if (bAllReady)
			m_WantedState = STATE_TOWERPLACEMENT;
		else
			BroadcastWalls(m_pNetwork, TRUE);

		break;
	}
	case STATE_TOWERPLACEMENT:
	{
		if (m_fTimeInState > m_fMaxTime)
			m_WantedState = STATE_SHOOT;

		Bool bNoMoreTower = TRUE;
		for (U32 p = 0; p < m_Players.GetCount(); p++)
		{
			PlayerPtr& player = m_Players[p];
			if (player.IsValid())
			{
				GamePlayer* gameplayer = player.GetCastPtr<GamePlayer>();
				if (gameplayer->HasTowerToPlace())
				{
					bNoMoreTower = FALSE;
					break;
				}
			}
		}

		if (bNoMoreTower)
			m_fTimeInState = Max(m_fTimeInState, m_fMaxTime-3.f);
		
		BroadcastTowers(m_pNetwork, TRUE);

		break;
	}

	case STATE_SHOOT:
	{
		if (m_fTimeInState > m_fMaxTime)
			m_WantedState = STATE_WALLS;
		else
		{
			BroadcastTowers(m_pNetwork, TRUE);
			BroadcastWalls(m_pNetwork, TRUE);
			BroadcastDamage(m_pNetwork);
		}

		break;
	}
	case STATE_WALLS:
	{
		EntityComponentGameGrid* grid = World::GetInstance().GetComponent<EntityComponentGameGrid>();
		if (grid && grid->IsPrepared())
		{
			grid->GetGrid().ComputeInside();
			grid->UpdateGroundColor(TRUE,TRUE);
		}

		if (m_fTimeInState > m_fMaxTime)
			m_WantedState = STATE_TOWERPLACEMENT;
		else
			BroadcastWalls(m_pNetwork, TRUE);

		break;
	}
	}
}

//------------------------------

void GameLobby::LastFrameInState()
{
	switch (m_State)
	{
	case STATE_PREPAREGAME:
	{
		DisableAllPlayers();
		break;
	}
	case STATE_FORTCHOICE:
	{
		BroadcastWalls(m_pNetwork, FALSE);
		break;
	}
	case STATE_TOWERPLACEMENT:
	{
		BroadcastTowers(m_pNetwork, FALSE);
		ResetGroundColor();
		DisableAllPlayers();
		break;
	}
	case STATE_SHOOT:
	{
		BroadcastTowers(m_pNetwork, FALSE);
		BroadcastWalls(m_pNetwork, FALSE);
		DisableAllPlayers();
		break;
	}
	case STATE_WALLS:
	{
		BroadcastWalls(m_pNetwork, FALSE);
		ResetGroundColor();
		DisableAllPlayers();
		break;
	}
	}

}

//------------------------------

void	GameLobby::RegisterPlayer(NetworkPtr& _pNetwork, const Player::Controller& _controller)
{
	if (m_bIsMaster)
	{
		GamePlayer* newPlayer = (GamePlayer*)NewPlayer();
		newPlayer->SetLocalPlayer(_controller);
		if (!TryResisterPlayer(newPlayer))
		{
			DELETE newPlayer;
		}
	}
	else
	{
		SNetworkMessage message(MESSAGE_RegisterPlayer);
		message.m_Stream.WriteS32(_controller);
		_pNetwork->SendPacket(m_LobbyOwnerID, message);
	}
}

//------------------------------

void GameLobby::DisableAllPlayers()
{
	for (U32 p = 0; p < m_Players.GetCount(); p++)
	{
		PlayerPtr& player = m_Players[p];
		if (player.IsValid())
		{
			GamePlayer* gameplayer = player.GetCastPtr<GamePlayer>();
			gameplayer->Disable();
		}
	}
}

//------------------------------

void GameLobby::ResetGroundColor()
{
	EntityComponentGameGrid* grid = World::GetInstance().GetComponent<EntityComponentGameGrid>();
	if (grid && grid->IsPrepared())
	{
		grid->UpdateGroundColor(FALSE);
	}
}

//------------------------------

void	GameLobby::BroadcastForts(NetworkPtr& _pNetwork, U32 _playerId, Vec2iArray& _forts)
{
	SNetworkMessage message(MESSAGE_Forts);
	message.m_Stream.WriteU8((U8)_playerId);
	message.m_Stream.WriteU8((U8)_forts.GetCount());
	for (Vec2i& pos : _forts)
	{
		message.m_Stream.WriteU16((U16)pos.x);
		message.m_Stream.WriteU16((U16)pos.y);
	}
	SendPacketToOthers(_pNetwork, message);
}

//------------------------------

void	GameLobby::BroadcastWalls(NetworkPtr& _pNetwork,Bool _canBeLost)
{
	EntityComponentGameGrid* grid = World::GetInstance().GetComponent<EntityComponentGameGrid>();
	if (grid)
	{
		ArrayOf<U16Array>	tmpmessages;
		tmpmessages.SetItemCount(m_Players.GetCount());
		U32 sizeX = grid->GetGridSizeX();
		for(U32 x=0; x<sizeX;x++)
			for (U32 y = 0; y < grid->GetGridSizeY(); y++)
			{
				GameGridCell& cell = grid->GetCell(x, y);
				if (cell.m_iGameFlag & GAME_GRID_CELL_WALL)
				{
					S8 playerId = cell.GetPlayerId();
					if (playerId >= 0 && m_Players[playerId]->IsLocal())
					{
						tmpmessages[playerId].AddLastItem((U16)(y*sizeX + x));
					}
				}
			}

		for(U32 i=0; i<tmpmessages.GetCount(); i++)
		{
			U16Array& walls = tmpmessages[i];
			if (walls.GetCount() == 0)
				continue;

			SNetworkMessage message(MESSAGE_Walls, _canBeLost);
			message.m_Stream.WriteU8((U8)i);
			message.m_Stream.WriteU16((U16)walls.GetCount());
			for (U16 wall : walls)
			{
				message.m_Stream.WriteU16(wall);
			}
			SendPacketToOthers(_pNetwork, message);
		}
	}
	
}

void	GameLobby::BroadcastDamage(NetworkPtr& _pNetwork)
{
	EntityComponentGameGrid* grid = World::GetInstance().GetComponent<EntityComponentGameGrid>();
	if (grid)
	{
		for (U32 p = 0; p < m_Players.GetCount(); p++)
		{
			GamePlayer* player = m_Players[p].GetCastPtr<GamePlayer>();
			if (player && player->IsLocal())
			{
				const Vec2i& shootPos = player->GetShootPos();
				if (shootPos.x >= 0)
				{
					GameGridCell& cell = grid->GetCell(shootPos.x, shootPos.y);
					S8 targetPlayerId = cell.GetPlayerId();
					if (targetPlayerId >= 0)
					{
						GamePlayer* targetPlayer = m_Players[targetPlayerId].GetCastPtr<GamePlayer>();
						if (targetPlayer->IsLocal())
						{
							ProcessDamage(shootPos);
						}
						else if(targetPlayer->IsRemote())
						{
							SNetworkMessage message(MESSAGE_Damage, FALSE);
							message.m_Stream.WriteU8((U8)p);
							message.m_Stream.WriteU8(targetPlayerId);
							message.m_Stream.WriteU16((U16)(shootPos.y*grid->GetGridSizeX() + shootPos.x));
							_pNetwork->SendPacket(targetPlayer->GetRemoteUserID(), message);
						}
					}

					player->ResetShootPos();
				}
			}
		}
	}
}

//------------------------------

void	GameLobby::ProcessDamage(const Vec2i& _pos)
{
	EntityComponentGameGrid* grid = World::GetInstance().GetComponent<EntityComponentGameGrid>();
	GameGridCell& cell = grid->GetCell(_pos.x, _pos.y);
	DEBUG_Require(m_Players[cell.GetPlayerId()]->IsLocal());
	if (cell.m_iGameFlag & (GAME_GRID_CELL_TOWER | GAME_GRID_CELL_WALL))
	{	
		Vec2i refPos = grid->GetRefCell(_pos.x, _pos.y);
		grid->Damage(refPos);
	}
}

//------------------------------

void	GameLobby::BroadcastTowers(NetworkPtr& _pNetwork, Bool _canBeLost)
{
	EntityComponentGameGrid* grid = World::GetInstance().GetComponent<EntityComponentGameGrid>();
	if (grid)
	{
		ArrayOf<U16Array>	tmpmessages;
		tmpmessages.SetItemCount(m_Players.GetCount());
		U32 sizeX = grid->GetGridSizeX();
		U32 flags = (GAME_GRID_CELL_TOWER | GAME_GRID_CELL_REF);
		for (U32 x = 0; x<sizeX; x++)
			for (U32 y = 0; y < grid->GetGridSizeY(); y++)
			{
				GameGridCell& cell = grid->GetCell(x, y);
				if((cell.m_iGameFlag & flags) == flags)
				{
					S8 playerId = cell.GetPlayerId();
					if (playerId >= 0 && m_Players[playerId]->IsLocal())
					{
						tmpmessages[playerId].AddLastItem((U16)(y*sizeX + x));
					}
				}
			}

		for (U32 i = 0; i<tmpmessages.GetCount(); i++)
		{
			U16Array& towers = tmpmessages[i];
			if (towers.GetCount() == 0)
				continue;

			SNetworkMessage message(MESSAGE_Towers, _canBeLost);
			message.m_Stream.WriteU8((U8)i);
			message.m_Stream.WriteU16((U16)towers.GetCount());
			for (U16 tower : towers)
			{
				message.m_Stream.WriteU16(tower);
			}
			SendPacketToOthers(_pNetwork, message);
		}
	}
}

//------------------------------

void	GameLobby::ComputeLobbyStateMessage(SNetworkMessage& _message, Bool _write, const NetworkUserID& _senderID)
{
	SUPER::ComputeLobbyStateMessage(_message, _write, _senderID);
	if (_write)
	{
		_message.m_Stream.WriteU32(m_State);
		_message.m_Stream.WriteFloat(m_fTimeInState);
	}
	else
	{
		EState curState = (EState)_message.m_Stream.ReadU32();
		if (curState != m_State)
		{
			m_WantedState = curState;
		}

		Float timeInState = _message.m_Stream.ReadFloat();
		for (U32 u = 0; u < m_RemoteUsers.GetCount(); u++)
		{
			Lobby::RemoteUsers&  user = m_RemoteUsers[u];
			if (user.m_UserID == _senderID)
			{
				timeInState += user.m_fPing * .5f; // add half ping to be more precise
				break;
			}
		}
		m_fTimeInState = timeInState;
	}
}