#ifndef __NETWORK_STEAM_H__
#define __NETWORK_STEAM_H__
#ifdef USE_STEAM

#include "network.h"

BEGIN_SUPER_CLASS(NetworkSteam, Network)
public:
	NetworkSteam();

	virtual void	AskCreateLobby() OVERRIDE;
	virtual void	LaunchLobbyInviteUI() const OVERRIDE;
	virtual void	GetLobbyUserIds(NetworkUserIDArray& _users) const OVERRIDE;
	virtual NetworkUserID GetLocalUserId() const OVERRIDE;
	virtual const char *	GetNetworkUserName(const NetworkUserID& _id) const OVERRIDE;

	virtual void SendPacket(const NetworkUserID& _toUserID, const SNetworkMessage& _message) OVERRIDE;
	virtual Bool ReadPacket(NetworkUserID& _fromUserID, SNetworkMessage& _message) OVERRIDE;
protected:
	void OnLobbyCreated(LobbyCreated_t *pCallback, bool bIOFailure);

	CCallResult<NetworkSteam, LobbyCreated_t> m_SteamCallResultLobbyCreated;
	CCallResult<NetworkSteam, LobbyEnter_t> m_SteamCallResultLobbyEntered;
	STEAM_CALLBACK(NetworkSteam, OnLobbyEntered, LobbyEnter_t);
	STEAM_CALLBACK(NetworkSteam, OnLobbyJoinRequested, GameLobbyJoinRequested_t);
	STEAM_CALLBACK(NetworkSteam, OnP2PSessionRequest, P2PSessionRequest_t);
END_SUPER_CLASS

#endif
#endif
