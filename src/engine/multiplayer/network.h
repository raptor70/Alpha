#ifndef __NETWORK_H__
#define __NETWORK_H__

#include "file\stream.h"

#define MAX_PACKET_SIZE (100 * 1024) //100ko

#ifdef USE_STEAM
typedef CSteamID NetworkUserID;
#else
struct NetworkUserID
{
	U64 m_Id;
	NetworkUserID()
	{
		m_Id = 0;
	}

	Bool operator==(const NetworkUserID& _other) const { return m_Id == _other.m_Id; }
	Bool operator==(U64 _id) const { return m_Id == _id; }
	U64	 ConvertToUint64() const { return m_Id; }
	void SetFromUint64(U64 _id) { m_Id = _id; }
};
#endif

typedef ArrayOf<NetworkUserID>	NetworkUserIDArray;

enum EMessageType
{
	MESSAGE_Ping = 0,
	MESSAGE_Pong,
	MESSAGE_RegisterPlayer,
	MESSAGE_UpdatePlayerList,
	MESSAGE_UpdateLobbyState,
	MESSAGE_UpdatePlayerPos,
	MESSAGE_PlayerInfo,
	MESSAGE_Forts,
	MESSAGE_Walls,
	MESSAGE_Towers,
	MESSAGE_Damage,
};

struct SNetworkMessage
{
	SNetworkMessage() { m_Stream.MoveToBegin(); m_bCanBeLost = FALSE; m_iType = 0; }
	SNetworkMessage(U8 _type, Bool _canBeLost = FALSE) { m_Stream.WriteU8(_type); m_iType = _type; m_bCanBeLost = _canBeLost; }
	SNetworkMessage(S8* _data, S32 _dataSize) { SetFrom(_data, _dataSize); m_bCanBeLost = FALSE; }

	void SetFrom(S8* _data, S32 _dataSize)
	{ 
		m_Stream.MoveToBegin();
		m_Stream.WriteByte(_data, _dataSize); 
		m_Stream.MoveToBegin();
		m_iType = m_Stream.ReadU8();
	}
	Bool m_bCanBeLost;
	U8	m_iType;
	Stream m_Stream;
};

class Network
{
public:
	Network();

	virtual void	Update();

	// lobby creation
	virtual void	AskCreateLobby() { DEBUG_Require(!IsInLobby()); m_bLobbyCreation = TRUE;  }
	Bool	IsWaitingLobbyCreation() const { return m_bLobbyCreation; }
	Bool	IsLobbyCreated() const { return m_bLobbyCreated; }
	virtual void	AskJoinLobby() { DEBUG_Require(!IsInLobby()); m_bLobbyJoining = TRUE; }
	Bool	IsWaitingLobbyJoining() const { return m_bLobbyJoining; }
	Bool	IsLobbyJoined() const { return m_bLobbyJoined; }
	virtual void	LaunchLobbyInviteUI() const {}
	Bool IsInLobby() const { return m_bLobbyCreation || m_bLobbyCreated || m_bLobbyJoining || m_bLobbyJoined;  }
	void ExitLobby() { m_bLobbyCreation = m_bLobbyCreated = m_bLobbyJoining = m_bLobbyJoined = FALSE; }

	// lobby infos
	const NetworkUserID& GetLobbyMasterId() const { return m_LobbyMasterId; }
	const NetworkUserID& GetLobbyOwnerId() const { return m_LobbyOwnerId; }
	virtual NetworkUserID GetLocalUserId() const { return NetworkUserID(); }
	virtual void GetLobbyUserIds(NetworkUserIDArray& _users) const	{}
	virtual const char *	GetNetworkUserName(const NetworkUserID& _id) const { return "Local"; }

	// packet
	virtual void SendPacket(const NetworkUserID& _toUserID, const SNetworkMessage& _message) { DEBUG_Require(IsInLobby()); }
	virtual Bool ReadPacket(NetworkUserID& _fromUserID, SNetworkMessage& _message) { return FALSE; }

protected:
	// lobby
	void CreateLobby() { m_bLobbyCreated = TRUE; }
	void JoinLobby() { m_bLobbyJoined = TRUE; }
	void SetLobbyMasterId(const NetworkUserID& _id) { m_LobbyMasterId = _id; }
	void SetLobbyOwnerId(const NetworkUserID& _id) { m_LobbyOwnerId = _id; }

	// lobby
	Bool	m_bLobbyCreation;
	Bool	m_bLobbyCreated;
	Bool	m_bLobbyJoining;
	Bool	m_bLobbyJoined;
	NetworkUserID	m_LobbyMasterId;
	NetworkUserID	m_LobbyOwnerId;
};

BEGIN_SUPER_CLASS(NetworkLocal, Network)
	virtual void	Update() OVERRIDE;
END_SUPER_CLASS

typedef RefTo<Network> NetworkPtr;

#endif
