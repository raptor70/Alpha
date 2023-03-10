#ifndef __COMPONENT_MULTIPLAYER_H__
#define __COMPONENT_MULTIPLAYER_H__

#include "world/component.h"
#include "lobby.h"
#include "network.h"

BEGIN_COMPONENT_CLASS(Multiplayer)
	DECLARE_SCRIPT_CLASS(EntityComponentMultiplayer)
protected:
	enum Status
	{
		STATUS_None = -1,
		STATUS_NoLobby,
		STATUS_WaitingCreateLobby,
		STATUS_WaitingJoinLobby,
		STATUS_InLobby
	};
public:
	EntityComponentMultiplayer();
	virtual ~EntityComponentMultiplayer();

	virtual Bool IsUpdater() const	{ return TRUE; }
	virtual Bool IsDrawer()	const	{ return FALSE; }
	virtual Bool ReceiveEvent()	const	{ return FALSE; }

	virtual void	Initialize() OVERRIDE;
	virtual void	Update(Float _dTime) OVERRIDE;

	void DrawDebug();

	void ManageChangeStatus();
	void OnStartStatus();
	void UpdateStatus(Float _dTime);
	void OnEndStatus();

	Bool IsInLobby() { return m_Status == STATUS_InLobby; }
	Lobby* GetLobby() { return m_pLobby; }
	void CreateLobby() { m_PendingStatus = STATUS_WaitingCreateLobby; }
	void JoinLobby() { m_PendingStatus = STATUS_WaitingJoinLobby; }
	void StopLobby() { m_PendingStatus = STATUS_NoLobby; }
	void InviteToLobby();
	virtual Lobby* NewLobby() { return NEW Lobby; }

protected:

	NetworkPtr	m_pNetwork;
	Status m_Status;
	Status m_PendingStatus;

	LobbyPtr		m_pLobby;
	Float m_fTimeOut;

END_COMPONENT_CLASS

#endif
