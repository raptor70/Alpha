#include "network_steam.h"
#ifdef USE_STEAM

NetworkSteam::NetworkSteam():SUPER()
{
	m_bLobbyCreation = FALSE;
	m_bLobbyCreated = FALSE;
}

//------------------------------------------------

void	NetworkSteam::AskCreateLobby()
{
	if (IsLobbyCreated() || IsLobbyJoined() || IsWaitingLobbyCreation() || IsWaitingLobbyJoining())
	{
		LOGGER_LogWarning("Steam - AskCreateLobby - Already in lobby\n");
		return;
	}

	SteamAPICall_t hSteamAPICall = SteamMatchmaking()->CreateLobby(k_ELobbyTypePublic, 4);
	m_SteamCallResultLobbyCreated.Set(hSteamAPICall, this, &NetworkSteam::OnLobbyCreated);
	SUPER::AskCreateLobby();
}

//------------------------------------------------

void	NetworkSteam::LaunchLobbyInviteUI() const
{
	SteamFriends()->ActivateGameOverlayInviteDialog(GetLobbyMasterId());
}

//------------------------------------------------

void	NetworkSteam::GetLobbyUserIds(NetworkUserIDArray& _users) const
{
	DEBUG_Require(IsInLobby());

	S32 nbMembers = SteamMatchmaking()->GetNumLobbyMembers(GetLobbyMasterId());
	for (S32 m = 0; m < nbMembers; m++)
		_users.AddLastItem(SteamMatchmaking()->GetLobbyMemberByIndex(GetLobbyMasterId(), m));
}

//------------------------------------------------

NetworkUserID NetworkSteam::GetLocalUserId() const
{
	return SteamUser()->GetSteamID();
}

//------------------------------------------------

const char *	NetworkSteam::GetNetworkUserName(const NetworkUserID& _id) const
{
	return SteamFriends()->GetFriendPersonaName(_id);
}

//------------------------------------------------

void NetworkSteam::SendPacket(const NetworkUserID& _userID, const SNetworkMessage& _message)
{
	SUPER::SendPacket(_userID, _message);
	SteamNetworking()->SendP2PPacket(_userID, _message.m_Stream.GetBuffer(), _message.m_Stream.GetSize(), _message.m_bCanBeLost ? k_EP2PSendUnreliable : k_EP2PSendReliable);
}

//------------------------------------------------

Bool NetworkSteam::ReadPacket(NetworkUserID& _fromUserID, SNetworkMessage& _message)
{
	NetworkUserID packetUserId;
	uint32 packetsize = 0;
	U8 datas[MAX_PACKET_SIZE];
	if(SteamNetworking()->ReadP2PPacket(&datas, MAX_PACKET_SIZE, &packetsize, &packetUserId))
	{
		DEBUG_Require(packetsize < MAX_PACKET_SIZE);
		_fromUserID = packetUserId;
		_message.SetFrom((S8*)datas, packetsize);
		return TRUE;
	}

	return FALSE;
}

//------------------------------------------------

void NetworkSteam::OnLobbyCreated(LobbyCreated_t *_pCallback, bool bIOFailure)
{
	if (!m_bLobbyCreation)
		return;

	// record which lobby we're in
	if (_pCallback->m_eResult == k_EResultOK)
	{
		SteamMatchmaking()->SetLobbyData(_pCallback->m_ulSteamIDLobby, "name", "Test lobby");
		SetLobbyMasterId(_pCallback->m_ulSteamIDLobby);
		SetLobbyOwnerId(SteamMatchmaking()->GetLobbyOwner(_pCallback->m_ulSteamIDLobby));
		CreateLobby();
	}
	else
	{
		// failed, show error
		LOGGER_LogError("Failed to create lobby (lost connection to Steam back-end servers.");
		ExitLobby();
	}
}

//------------------------------------------------

void NetworkSteam::OnLobbyEntered(LobbyEnter_t *_pCallback)
{
	if (IsWaitingLobbyJoining())
	{
		SetLobbyMasterId(_pCallback->m_ulSteamIDLobby);
		SetLobbyOwnerId(SteamMatchmaking()->GetLobbyOwner(_pCallback->m_ulSteamIDLobby));
		JoinLobby();
	}
}

//------------------------------

void NetworkSteam::OnLobbyJoinRequested(GameLobbyJoinRequested_t *pCallback)
{
	if (IsLobbyCreated() || IsLobbyJoined() || IsWaitingLobbyCreation() || IsWaitingLobbyJoining())
	{
		LOGGER_LogWarning("Steam - OnLobbyJoinRequested - Already in lobby\n");
		return;
	}

	// move forward the state
	SteamMatchmaking()->JoinLobby(pCallback->m_steamIDLobby);
	SUPER::AskJoinLobby();
}

//------------------------------

void NetworkSteam::OnP2PSessionRequest(P2PSessionRequest_t *_pCallback)
{
	NetworkUserID lobbyId = GetLobbyMasterId();
	S32 nbMembers = SteamMatchmaking()->GetNumLobbyMembers(lobbyId);
	for (S32 m = 0; m < nbMembers; m++)
	{
		NetworkUserID userId = SteamMatchmaking()->GetLobbyMemberByIndex(lobbyId, m);
		if (userId == _pCallback->m_steamIDRemote)
		{
			SteamNetworking()->AcceptP2PSessionWithUser(_pCallback->m_steamIDRemote);
		}
	}
}

#endif