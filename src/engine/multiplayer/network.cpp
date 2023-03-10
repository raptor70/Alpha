#include "network.h"

Network::Network()
{
	m_bLobbyCreation = FALSE;
	m_bLobbyCreated = FALSE;
	m_bLobbyJoined = FALSE;
}

//------------------------------------------

void	Network::Update()
{
	DEBUG_Require(!m_bLobbyJoined || (!m_bLobbyCreation && !m_bLobbyCreated));
	DEBUG_Require(!m_bLobbyCreated || (!m_bLobbyJoining && !m_bLobbyJoined));
}

//------------------------------------------

void	NetworkLocal::Update()
{
	SUPER::Update();

	if (m_bLobbyCreation)
		m_bLobbyCreated = TRUE;

	if (m_bLobbyJoining)
		m_bLobbyJoined = TRUE;
}