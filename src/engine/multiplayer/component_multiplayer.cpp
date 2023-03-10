#include "component_multiplayer.h"

#include "application.h"
#include "network.h"
#include "network_steam.h"

BEGIN_SCRIPT_CLASS_INHERITED(EntityComponentMultiplayer,EntityComponent)
	SCRIPT_VOID_METHOD(CreateLobby)
	SCRIPT_VOID_METHOD(StopLobby)
	SCRIPT_VOID_METHOD(InviteToLobby)
	SCRIPT_RESULT_METHOD(GetLobby,Lobby)
	SCRIPT_RESULT_METHOD(IsInLobby,Bool)
	SCRIPT_VOID_METHOD(DrawDebug)
END_SCRIPT_CLASS

DEFINE_COMPONENT_CLASS(Multiplayer)
END_DEFINE_COMPONENT

EntityComponentMultiplayer::EntityComponentMultiplayer()
{
}

//------------------------------

EntityComponentMultiplayer::~EntityComponentMultiplayer()
{
}

//------------------------------

void	EntityComponentMultiplayer::Initialize()
{
	SUPER::Initialize();

	if (Application::GetInstance().MustUseSteam())
		m_pNetwork = NEW NetworkSteam;
	else
		m_pNetwork = NEW NetworkLocal;

	m_PendingStatus = STATUS_NoLobby;
	m_Status = STATUS_None;
	m_fTimeOut = 0.f;
}

//------------------------------

void	EntityComponentMultiplayer::Update(Float _dTime)
{
	SUPER::Update(_dTime);

	m_pNetwork->Update();

	if (m_fTimeOut > 0.f)
		m_fTimeOut -= _dTime;
	UpdateStatus(_dTime);
}

//------------------------------

void EntityComponentMultiplayer::DrawDebug()
{
	Str status = "";
	switch(m_Status)
	{ 
	case STATUS_None: status = ""; break;
	case STATUS_NoLobby: status = "NoLobby"; break;
	case STATUS_WaitingCreateLobby: status = "WaitingCreateLobby"; break;
	case STATUS_WaitingJoinLobby: status = "WaitingJoinLobby"; break;
	case STATUS_InLobby: status = "InLobby"; break;
	}

	S32 y = 200;
	Str line;
	line.SetFrom("Status : %s", status.GetArray());
	DEBUG_DrawText(line.GetArray(), 200, y, Color::Grey);
	y += 16;

	line.SetFrom("TimeOut : %.2f", m_fTimeOut);
	DEBUG_DrawText(line.GetArray(), 200, y, Color::Grey);
	y += 16;

	if (m_Status == STATUS_InLobby)
	{
		m_pLobby->DrawDebug(y,m_pNetwork);
	}
}

//------------------------------

void EntityComponentMultiplayer::ManageChangeStatus()
{
	if (m_PendingStatus != STATUS_None)
	{
		OnEndStatus();
		OnStartStatus();
		m_Status = m_PendingStatus;
		m_PendingStatus = STATUS_None;
	}
}

//------------------------------

void EntityComponentMultiplayer::OnStartStatus()
{
	switch (m_PendingStatus)
	{
		case STATUS_NoLobby:
		{
			m_pLobby.SetNull();
			m_pNetwork->ExitLobby();
			break;
		}

		case STATUS_WaitingCreateLobby:
		{
			m_pLobby = NewLobby();
			m_fTimeOut = 5.f;
			m_pNetwork->AskCreateLobby();
			break;
		}

		case STATUS_WaitingJoinLobby:
		{
			m_pLobby = NewLobby();
			m_fTimeOut = 5.f;
			break;
		}

		case STATUS_InLobby:
		{
			m_pLobby->Initialize();
			break;
		}
	}
}

//------------------------------

void EntityComponentMultiplayer::UpdateStatus(Float _dTime)
{
	ManageChangeStatus();

	switch (m_Status)
	{
	case STATUS_NoLobby:
	{
		if (m_pNetwork->IsWaitingLobbyJoining())
		{
			m_PendingStatus = STATUS_WaitingJoinLobby;
		}
		break;
	}
	case STATUS_WaitingCreateLobby:
	{
		if (m_fTimeOut <= 0.f || !m_pNetwork->IsWaitingLobbyCreation())
		{
			m_PendingStatus = STATUS_NoLobby;
		}
		else if (m_pNetwork->IsLobbyCreated())
		{
			m_pLobby->SetMaster(m_pNetwork->GetLobbyMasterId());
			m_PendingStatus = STATUS_InLobby;
		}

		break;
	}

	case STATUS_WaitingJoinLobby:
	{
		if (m_fTimeOut <= 0.f || !m_pNetwork->IsWaitingLobbyJoining())
		{
			m_PendingStatus = STATUS_NoLobby;
		}
		else if (m_pNetwork->IsLobbyJoined())
		{
			m_pLobby->SetSlave(m_pNetwork->GetLobbyMasterId(), m_pNetwork->GetLobbyOwnerId());
			m_PendingStatus = STATUS_InLobby;
		}

		break;
	}

	case STATUS_InLobby:
	{
		m_pLobby->Update(_dTime,m_pNetwork);
		break;
	}
	}
}

//------------------------------

void EntityComponentMultiplayer::OnEndStatus()
{
	switch (m_Status)
	{
	case STATUS_InLobby:
	{
		m_pLobby.SetNull();
		break;
	}
	}
}

//------------------------------

void EntityComponentMultiplayer::InviteToLobby()
{
	if (m_Status != STATUS_InLobby)
		return;

	m_pNetwork->LaunchLobbyInviteUI();
}
