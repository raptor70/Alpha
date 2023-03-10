#ifndef __GAME_PLAYER_H__
#define __GAME_PLAYER_H__

#include "multiplayer\player.h"

class EntityComponentGameGrid;

BEGIN_SUPER_CLASS(GamePlayer, Player)
	DECLARE_SCRIPT_CLASS(GamePlayer)
public:
	GamePlayer();
	virtual ~GamePlayer();

	virtual void Update(Float _dTime);

	// message
	virtual void	ComputePlayerInfoMessage(SNetworkMessage& _message, Bool _write) OVERRIDE;

	Vec3f GetWorldPos() const { return m_pRoot->GetWorldPos(); }
	void SetWorldPos(const Vec3f& _pos) { m_pRoot->SetWorldPos(_pos); }

	void	Disable() { m_WantedState = STATE_NOTHING; }
	void	SetToCursor() { m_WantedState = STATE_CURSOR; }
	void	SetToSelectFort() { m_WantedState = STATE_SELECTFORT; }
	Bool	IsFortSelected() { return m_State == STATE_FORTSELECTED; }
	void	SetToPlaceTower() { m_WantedState = STATE_TOWERPLACEMENT; }
	void	SetToShoot() { m_WantedState = STATE_SHOOT; }
	void	SetToPlaceWall() { m_WantedState = STATE_WALLPLACEMENT; }
	Bool	HasTowerToPlace() const { return m_iNbTowers > 0; }

	const Vec2i& GetShootPos() const { return m_vShootPos; }
	void ResetShootPos() { m_vShootPos = Vec2i(-1, -1); }

protected:
	Entity*	CreateNewWall();
	const Color& ComputeColor() const;
	void UpdateCursor(Float _dTime);
	void UpdateSelectFort(Float _dTime);
	void FindAllForts();
	void PrepareFirstWalls();
	void UpdateTowerPlacement(Float _dTime);
	Bool TowerPlacementValid(S32 _x, S32 _y);
	void PlaceTower(S32 _x, S32 _y);
	void FindAllTowers();
	void UpdateShoot(Float _dTime);
	void UpdateWallPlacement(Float _dTime);
	Bool WallPlacementValid(S32 _x, S32 _y);
	void PlaceWall(S32 _x, S32 _y);
	void NextShape();
	void NextShapeRotation(U32 _delta);
	void UpdateWallsVisibility();
	Vec2i ComputeWallRotatedPos(const Vec2i& _pos);

	// states
	void FirstFrameInState();
	void UpdateState(Float _dTime);
	void LastFrameInState();

	enum EState
	{
		STATE_NONE = -1,
		STATE_NOTHING = 0,
		STATE_CURSOR,
		STATE_SELECTFORT,
		STATE_FORTSELECTED,
		STATE_TOWERPLACEMENT,
		STATE_SHOOT,
		STATE_WALLPLACEMENT
	};

	Entity* m_pRoot;
	Entity* m_pCursor;
	Entity* m_pTower;
	EntityPtrArray m_Walls;
	EntityComponentGameGrid* m_pGrid;

	EState		m_State;
	EState		m_WantedState;
	Float		m_fTimeInState;
	Vec2iArray	m_Forts;
	U32			m_iCurrentFort;
	Vec2iArray	m_Towers;
	FloatArray	m_TowersCooldown;
	U32			m_iCurrentTower;
	Color		m_Color;
	U32			m_iNbTowers;
	Float		m_fCursorAlpha;
	U32			m_iCurrentWallShape;
	U32			m_iCurrentWallRotation;
	Vec3f		m_vRemotePos;
	Vec2i		m_vShootPos;

END_SUPER_CLASS

#endif
