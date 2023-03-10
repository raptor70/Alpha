#ifndef __LOBBY_H__
#define __LOBBY_H__

#include "network.h"
#include "player.h"

class Lobby
{
	DECLARE_SCRIPT_CLASS(Lobby)
public:
	Lobby();
	virtual ~Lobby();

	void Initialize();
	virtual void Update(Float _dTime, NetworkPtr& _pNetwork);

	const NetworkUserID& GetLobbyId() const { return m_LobbyID; }
	void SetMaster(const NetworkUserID& _lobbyID) { m_LobbyID = _lobbyID; m_bIsMaster = TRUE; }
	void SetSlave(const NetworkUserID& _lobbyID, const NetworkUserID& _lobbyOwnerID) { m_LobbyID = _lobbyID; m_bIsMaster = FALSE; m_LobbyOwnerID = _lobbyOwnerID; }
	Bool IsMaster() const { return m_bIsMaster; }

	virtual Player* NewPlayer() { return NEW Player; }
	Bool	AddPlayerOnFreeSlot(Player* _player);
	S32		GetNbPlayers() const;
	void	SetMaxPlayer(S32 _maxCount) { m_Players.SetItemCount(_maxCount); }
	Bool	TryResisterPlayer(Player* _newPlayer);

	void	SendPacketToOthers(NetworkPtr& _pNetwork, const SNetworkMessage& _message);
	virtual void	DrawDebug(S32& _y, const NetworkPtr& _pNetwork);

protected:
	void	UpdateRemoteUsersFromLobbyMembers(const NetworkPtr& _pNetwork);
	void	UpdatePing(NetworkPtr& _pNetwork);
	void	BroadcastPlayerList(Float _dTime, NetworkPtr& _pNetwork);
	void	BroadcastLobbyState(Float _dTime, NetworkPtr& _pNetwork);
	void	BroadcastLocalPlayers(Float _dTime, NetworkPtr& _pNetwork);
	virtual void ComputeLobbyStateMessage(SNetworkMessage& _message, Bool _write, const NetworkUserID& _senderID = NetworkUserID());

	void	ProcessPackets(NetworkPtr& _pNetwork);
	virtual void	ReadPacket(NetworkPtr& _pNetwork,const NetworkUserID& _userID, SNetworkMessage& _message);

	Float m_fGlobalTime;
	Bool m_bIsMaster;
	NetworkPtr m_pNetwork;
	NetworkUserID	m_LobbyID;
	NetworkUserID m_LobbyOwnerID;
	PlayerPtrArray	m_Players;
	Float m_fUpdatePlayerDelay;
	Float m_fUpdateLobbyDelay;

	struct RemoteUsers
	{
		RemoteUsers()
		{
			m_fLastPingSend = 0.f;
			m_fPing = 0.f;
			m_bWaitPong = FALSE;
			m_bIsLocal = FALSE;
		}

		NetworkUserID m_UserID;
		Float	m_fLastPingSend;
		Float	m_fPing;
		Bool	m_bWaitPong;
		Bool	m_bIsLocal;
	};
	ArrayOf<RemoteUsers>	m_RemoteUsers;
};

typedef RefTo<Lobby>	LobbyPtr;

#endif
