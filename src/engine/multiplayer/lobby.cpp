#include "lobby.h"

BEGIN_SCRIPT_CLASS(Lobby)
	SCRIPT_RESULT_METHOD(GetNbPlayers,S32)
	SCRIPT_RESULT_METHOD(IsMaster,Bool)
END_SCRIPT_CLASS

Lobby::Lobby()
{
	m_bIsMaster = TRUE;
}

//------------------------------

Lobby::~Lobby()
{
}

//------------------------------

void	Lobby::Initialize()
{
	m_fGlobalTime = 0.f;
	m_fUpdatePlayerDelay = 0.2f;
	m_fUpdateLobbyDelay = 0.1f;
}

//------------------------------

void	Lobby::Update(Float _dTime, NetworkPtr& _pNetwork)
{
	m_fGlobalTime += _dTime;
	m_pNetwork = _pNetwork.GetPtr();

	// remote list
	UpdateRemoteUsersFromLobbyMembers(_pNetwork);

	// ping
	UpdatePing(_pNetwork);

	// update player list
	if (m_bIsMaster)
	{
		BroadcastPlayerList(_dTime, _pNetwork);
		BroadcastLobbyState(_dTime, _pNetwork);
	}

	// send local players packet
	BroadcastLocalPlayers(_dTime, _pNetwork);

	// read packet
	ProcessPackets(_pNetwork);

	// update playes
	for (U32 p = 0; p < m_Players.GetCount(); p++)
	{
		PlayerPtr& player = m_Players[p];
		if (player.IsValid())
		{
			player->Update(_dTime);
		}
	}
}

//------------------------------

void	Lobby::UpdateRemoteUsersFromLobbyMembers(const NetworkPtr& _pNetwork)
{
	NetworkUserIDArray users;
	_pNetwork->GetLobbyUserIds(users);
	Str line;
	NetworkUserID localId =  _pNetwork->GetLocalUserId();
	for (NetworkUserID& userId : users)
	{
		Bool bFound = FALSE;
		for (U32 u = 0; u < m_RemoteUsers.GetCount(); u++)
		{
			if (m_RemoteUsers[u].m_UserID == userId)
			{
				bFound = TRUE;
				break;
			}
		}

		if (!bFound)
		{
			Lobby::RemoteUsers&  newuser = m_RemoteUsers.AddLastItem();
			newuser.m_UserID = userId;
			newuser.m_bIsLocal = (userId == localId);
		}
	}
}

//------------------------------

void	Lobby::UpdatePing(NetworkPtr& _pNetwork)
{
	// ping
	for (U32 u = 0; u < m_RemoteUsers.GetCount(); u++)
	{
		Lobby::RemoteUsers&  newuser = m_RemoteUsers[u];
		if (!newuser.m_bIsLocal)
		{
			Float deltaTime = m_fGlobalTime - newuser.m_fLastPingSend;
			if (!newuser.m_bWaitPong || deltaTime > 1.f)
			{
				newuser.m_bWaitPong = TRUE;
				newuser.m_fLastPingSend = m_fGlobalTime;

				_pNetwork->SendPacket(newuser.m_UserID, SNetworkMessage(MESSAGE_Ping, TRUE));
			}
		}
	}
}

//------------------------------

void	Lobby::BroadcastPlayerList(Float _dTime, NetworkPtr& _pNetwork)
{
	m_fUpdatePlayerDelay -= _dTime;
	if (m_fUpdatePlayerDelay < 0.f)
	{
		m_fUpdatePlayerDelay += 0.2f;
		SNetworkMessage message(MESSAGE_UpdatePlayerList, TRUE);
		for (U32 p = 0; p < m_Players.GetCount(); p++)
		{
			PlayerPtr& player = m_Players[p];
			if (player.IsValid())
			{
				message.m_Stream.WriteBool(TRUE);
				if (player->IsRemote())
				{
					message.m_Stream.WriteU64(player->GetRemoteUserID().ConvertToUint64());
					message.m_Stream.WriteU8(player->GetController());
				}
				else
				{
					message.m_Stream.WriteU64(_pNetwork->GetLocalUserId().ConvertToUint64());
					message.m_Stream.WriteU8(player->GetController());
				}
			}
			else
			{
				message.m_Stream.WriteBool(FALSE);
			}
		}

		SendPacketToOthers(_pNetwork,message);
	}
}

//------------------------------

void	Lobby::BroadcastLobbyState(Float _dTime, NetworkPtr& _pNetwork)
{
	m_fUpdateLobbyDelay -= _dTime;
	if (m_fUpdateLobbyDelay < 0.f)
	{
		m_fUpdateLobbyDelay += 0.2f;
		SNetworkMessage message(MESSAGE_UpdateLobbyState, TRUE);
		ComputeLobbyStateMessage(message, TRUE);
		SendPacketToOthers(_pNetwork, message);
	}
}

//------------------------------

void	Lobby::BroadcastLocalPlayers(Float _dTime, NetworkPtr& _pNetwork)
{
	U32 id = 0;
	for(PlayerPtr player : m_Players)
	{
		if (player.IsValid())
		{
			if (player->IsLocal())
			{
				SNetworkMessage message(MESSAGE_PlayerInfo, TRUE);
				message.m_Stream.WriteU32(id);
				player->ComputePlayerInfoMessage(message, TRUE);
				SendPacketToOthers(_pNetwork, message);
			}
		}
		id++;
	}
}

//------------------------------

void	Lobby::ComputeLobbyStateMessage(SNetworkMessage& _message, Bool _write, const NetworkUserID& _senderID)
{
	if (_write)
	{
		_message.m_Stream.WriteFloat(m_fGlobalTime);
	}
	else
	{
		Float masterLocalTime = _message.m_Stream.ReadFloat();
		for (U32 u = 0; u < m_RemoteUsers.GetCount(); u++)
		{
			Lobby::RemoteUsers&  user = m_RemoteUsers[u];
			if (user.m_UserID == _senderID)
			{
				masterLocalTime += user.m_fPing * .5f; // add half ping to be more precise
				break;
			}
		}
		m_fGlobalTime = masterLocalTime;
	}
}

//------------------------------

void	Lobby::SendPacketToOthers(NetworkPtr& _pNetwork, const SNetworkMessage& _message)
{
	for (U32 u = 0; u < m_RemoteUsers.GetCount(); u++)
	{
		Lobby::RemoteUsers&  newuser = m_RemoteUsers[u];
		if (!newuser.m_bIsLocal)
		{
			_pNetwork->SendPacket(newuser.m_UserID, _message);
		}
	}
}

//------------------------------

void	Lobby::ProcessPackets(NetworkPtr& _pNetwork)
{
	NetworkUserID packetUserId;
	SNetworkMessage message;
	while (_pNetwork->ReadPacket(packetUserId, message))
	{
		ReadPacket(_pNetwork, packetUserId, message);
	}
}

//------------------------------

void	Lobby::ReadPacket(NetworkPtr& _pNetwork, const NetworkUserID& _senderID, SNetworkMessage& _message)
{
	switch (_message.m_iType)
	{
	case MESSAGE_Ping:
	{
		_pNetwork->SendPacket(_senderID, SNetworkMessage(MESSAGE_Pong, TRUE));
		break;
	}

	case MESSAGE_Pong:
	{
		for (U32 u = 0; u < m_RemoteUsers.GetCount(); u++)
		{
			Lobby::RemoteUsers&  newuser = m_RemoteUsers[u];
			if (newuser.m_UserID == _senderID)
			{
				newuser.m_bWaitPong = FALSE;
				newuser.m_fPing = m_fGlobalTime - newuser.m_fLastPingSend;
				break;
			}
		}
		break;
	}

	case MESSAGE_RegisterPlayer:
	{
		Player* newPlayer = NewPlayer();
		S32 controller = _message.m_Stream.ReadS32();
		newPlayer->SetRemovePlayer(_senderID, (Player::Controller)controller);
		if (!TryResisterPlayer(newPlayer))
		{
			DELETE newPlayer;
		}
		break;
	}

	case MESSAGE_UpdatePlayerList:
	{
		for (U32 p = 0; p < m_Players.GetCount(); p++)
		{
			PlayerPtr& player = m_Players[p];
			if (_message.m_Stream.ReadBool()) // is valid
			{
				if (!player.IsValid())
				{
					player = NewPlayer();
					player->SetId((U8)p);
				}

				NetworkUserID userID;
				userID.SetFromUint64(_message.m_Stream.ReadU64());
				if (userID == _pNetwork->GetLocalUserId())
				{
					player->SetLocalPlayer((Player::Controller)_message.m_Stream.ReadU8());
				}
				else
				{
					player->SetRemovePlayer(userID, (Player::Controller)_message.m_Stream.ReadU8());
				}
			}
			else
			{
				player.SetNull();
			}
		}
		break;
	}

	case MESSAGE_UpdateLobbyState:
	{
		ComputeLobbyStateMessage(_message, FALSE,_senderID);
		break;
	}

	case MESSAGE_PlayerInfo:
	{
		U32 id = _message.m_Stream.ReadU32();
		//DEBUG_Require(m_Players[id].IsValid() && m_Players[id]->GetRemoteUserID() == _senderID);
		if(m_Players[id].IsValid() && m_Players[id]->GetRemoteUserID() == _senderID)
			m_Players[id]->ComputePlayerInfoMessage(_message, FALSE);
		break;
	}
	}
}

//------------------------------

Bool	Lobby::AddPlayerOnFreeSlot(Player* _player)
{
	for (U32 p = 0; p < m_Players.GetCount(); p++)
	{
		PlayerPtr& player = m_Players[p];
		if (player.IsNull())
		{
			player = _player;
			player->SetId((U8)p);
			return TRUE;
		}
	}

	return FALSE;
}

//------------------------------

S32		Lobby::GetNbPlayers() const
{
	S32 count = 0;
	for (U32 p = 0; p < m_Players.GetCount(); p++)
	{
		const PlayerPtr& player = m_Players[p];
		if (!player.IsNull())
		{
			count++;
		}
	}

	return count;
}

//------------------------------

Bool	Lobby::TryResisterPlayer(Player* _newPlayer)
{
	for (U32 p = 0; p < m_Players.GetCount(); p++)
	{
		PlayerPtr& player = m_Players[p];

		if (player.IsValid())
		{
			if ((*_newPlayer) == (*(player.GetCastPtr<Player>())))
			{
				return FALSE;
			}
		}
	}

	if (!AddPlayerOnFreeSlot(_newPlayer))
		return FALSE;

	return TRUE;
}

//------------------------------

void Lobby::DrawDebug(S32& _y, const NetworkPtr& _pNetwork)
{
	Str line;
	line.SetFrom("Global Time synchro %.2f", m_fGlobalTime);
	DEBUG_DrawText(line.GetArray(), 200, _y, Color::Grey);
	_y += 16;

	NetworkUserID lobbyId = GetLobbyId();
	for (U32 u = 0; u < m_RemoteUsers.GetCount(); u++)
	{
		Lobby::RemoteUsers&  newuser = m_RemoteUsers[u];
		const char *pchName = _pNetwork->GetNetworkUserName(newuser.m_UserID);
		line.SetFrom("%d - %s(%s) : ping %.2fs", u, pchName, newuser.m_bIsLocal?"Local":"Remote",newuser.m_fPing * 1000.f);
		DEBUG_DrawText(line.GetArray(), 200, _y, Color::Grey);
		_y += 16;
	}
	_y += 16;
	for (U32 p = 0; p < m_Players.GetCount(); p++)
	{
		PlayerPtr& player = m_Players[p];
		if (player.IsValid())
		{
			Str name = "Local";
			if (player->IsRemote())
			{
				name = _pNetwork->GetNetworkUserName(player->GetRemoteUserID());
			}
			
			Str controller;
			switch (player->GetController())
			{
			case Player::CONTROLLER_KeyboardMouse:
			{
				controller = "KeyboardMouse";
				break;
			}
			case Player::CONTROLLER_Pad0:
			{
				controller = "Pad0";
				break;
			}
			case Player::CONTROLLER_Pad1:
			{
				controller = "Pad1";
				break;
			}
			case Player::CONTROLLER_Pad2:
			{
				controller = "Pad2";
				break;
			}
			case Player::CONTROLLER_Pad3:
			{
				controller = "Pad3";
				break;
			}
			default:
			{
				controller = "Unknown";
			}
			}

			line.SetFrom("SLOT %d - %s(%s)", p, name.GetArray(), controller.GetArray());
			DEBUG_DrawText(line.GetArray(), 200, _y, Color::Grey);
			_y += 16;
		}
	}
}
