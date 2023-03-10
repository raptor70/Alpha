#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "network.h"

class Player
{
	DECLARE_SCRIPT_CLASS(Player)
public:
	Player();
	virtual ~Player();

	enum Type
	{
		TYPE_None = -1,
		TYPE_Local = 0,
		TYPE_Remote
	};

	enum Controller
	{
		CONTROLLER_None = -1,
		CONTROLLER_KeyboardMouse,
		CONTROLLER_Pad0,
		CONTROLLER_Pad1,
		CONTROLLER_Pad2,
		CONTROLLER_Pad3,
		CONTROLLER_AI
	};

	virtual void Update(Float _dTime);

	virtual void	ComputePlayerInfoMessage(SNetworkMessage& _message, Bool _write) {}

	void SetId(U8 _id) { m_Id = _id; }
	void SetLocalPlayer(const Controller& _controller) { m_Type = TYPE_Local; m_Controller = _controller; }
	void SetRemovePlayer(const NetworkUserID& _userId, const Controller& _controller) { m_Type = TYPE_Remote; m_RemoteUserID = _userId; m_Controller = _controller; }
	Bool IsLocal() const { return m_Type == TYPE_Local; }
	Bool IsRemote() const { return m_Type == TYPE_Remote; }
	const NetworkUserID& GetRemoteUserID() const { return m_RemoteUserID; }
	const Controller& GetController() const { return m_Controller; }

	Bool operator==(const Player& _other)
	{
		return (m_Type == _other.m_Type) && (m_RemoteUserID == _other.m_RemoteUserID) && (m_Controller == _other.m_Controller);
	}

protected:
	U8				m_Id;
	Type			m_Type;
	Controller		m_Controller;
	NetworkUserID	m_RemoteUserID;
};

typedef RefTo<Player>	PlayerPtr;
typedef ArrayOf<PlayerPtr>	PlayerPtrArray;

#endif
