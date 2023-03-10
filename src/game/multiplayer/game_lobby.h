#ifndef __GAME_LOBBY_H__
#define __GAME_LOBBY_H__

#include "multiplayer\lobby.h"
#include "game_player.h"

BEGIN_SUPER_CLASS(GameLobby,Lobby)
	DECLARE_SCRIPT_CLASS(GameLobby)
public:
	GameLobby();
	virtual ~GameLobby();

	virtual Player* NewPlayer() { return NEW GamePlayer; }

	virtual void Update(Float _dTime, NetworkPtr& _pNetwork) OVERRIDE;
	virtual void DrawDebug(S32& _y, const NetworkPtr& _pNetwork) OVERRIDE;

	void GoToGame() { m_WantedState = STATE_STARTGAME; }
	Float GetTimeLeft() const { return m_fMaxTime - m_fTimeInState; }
protected:
	virtual void	ReadPacket(NetworkPtr& _pNetwork, const NetworkUserID& _userID, SNetworkMessage& _message) OVERRIDE;
	virtual void	ComputeLobbyStateMessage(SNetworkMessage& _message, Bool _write, const NetworkUserID& _senderID = NetworkUserID()) OVERRIDE;
	void	BroadcastForts(NetworkPtr& _pNetwork, U32 _playerId, Vec2iArray& _forts);
	void	BroadcastWalls(NetworkPtr& _pNetwork, Bool _canBeLost);
	void	BroadcastTowers(NetworkPtr& _pNetwork, Bool _canBeLost);
	void	BroadcastDamage(NetworkPtr& _pNetwork);
	void	ProcessDamage(const Vec2i& _pos);

	// states
	void FirstFrameInState();
	void UpdateState(Float _dTime);
	void LastFrameInState();

	// helpers
	void RegisterPlayer(NetworkPtr& _pNetwork, const Player::Controller& _controller);
	void DisableAllPlayers();
	void ResetGroundColor();

	enum EState
	{
		STATE_NONE = -1,
		STATE_ADDPLAYERS = 0,
		STATE_STARTGAME,
		STATE_PREPAREGAME,
		STATE_FORTCHOICE,
		STATE_TOWERPLACEMENT,
		STATE_SHOOT,
		STATE_WALLS,
		STATE_GAMEOVER
	};

	struct SFortDefinition
	{
		U8 m_PlayerId;
		Vec2iArray m_Forts;
	};

	BoolArray m_PlayerPrepared;
	Bool m_bLocalPrepared;
	ArrayOf<SFortDefinition> m_WaitingPlayerForts;
	EState	m_State;
	EState	m_WantedState;
	Float	m_fTimeInState;
	Float	m_fMaxTime;
END_SUPER_CLASS

#endif
