#ifndef __GAME_GRID_H__
#define __GAME_GRID_H__

#include "grid/grid.h"
#include "grid/grid_cell.h"

#define GAME_GRID_CELL_PLAYER0		0
#define GAME_GRID_CELL_PLAYER1		1
#define GAME_GRID_CELL_PLAYER2		2
#define GAME_GRID_CELL_PLAYER3		3	// first two bits
#define GAME_GRID_CELL_PLAYERMASK	((1<<0)|(1<<1))
#define GAME_GRID_CELL_GROUND		(1<<2)
#define GAME_GRID_CELL_WATER		(1<<3)
#define GAME_GRID_CELL_FORT			(1<<4)
#define GAME_GRID_CELL_WALL			(1<<5)
#define GAME_GRID_CELL_REF			(1<<6)
#define GAME_GRID_CELL_TOWER		(1<<7)
#define GAME_GRID_CELL_INSIDE		(1<<8)

//-----------------------------------------------------------------------

struct GameGridShape
{
	U32	m_iSizeX;
	U32 m_iSizeY;
	ArrayOf<BoolArray> m_Pattern;

	INLINE Bool IsWall(U32 _x, U32 _y) const 
	{
		if (_x < m_iSizeX && _y < m_iSizeY)
			return m_Pattern[_y][_x];

		return FALSE;
	}
};

typedef ArrayOf<GameGridShape> GameGridShapeArray;

//-----------------------------------------------------------------------

BEGIN_SUPER_CLASS(GameGridCell,GridCell)
public:
	U32				m_iGameFlag;
	Vec3f			m_vPos;
	Entity*		m_pParentEntity;
	Entity*		m_pGroundEntity;
	Entity*		m_pTowerEntity;
	S32				m_iOutsideDist;
	U8				m_iLife;

	GameGridCell()
	{
		m_iGameFlag = 0;
		m_pGroundEntity = NULL;
		m_pParentEntity = NULL;
		m_pTowerEntity = NULL;
		m_vPos = Vec3f::Zero;
		m_iOutsideDist = -1;
		m_iLife = 0;
	}

	void CreateGroundEntity(const Str& _prefab);
	void CreateTowerEntity(const Str& _prefab, const Vec3f& _deltaPos = Vec3f::Zero);
	void RemoveTowerEntity();
	void SetGroundColor(const Color& _color);
	Bool IsForPlayer(U32 _player) const { return ((m_iGameFlag & (GAME_GRID_CELL_WATER|GAME_GRID_CELL_PLAYERMASK)) == _player); }
	S8	GetPlayerId() const;

END_SUPER_CLASS

//-----------------------------------------------------------------------

BEGIN_SUPER_CLASS(GameGrid, Grid<GameGridCell>)
public:
	GameGrid();
	void	GetCellsWithFlag(U32 _flags, Vec2iArray& _outPos);
	void	GetCellsForPlayer(U32 _player, U32 _flags, Vec2iArray& _outPos);
	void	ComputeInside();

	U32		GetShapeCount() const { return m_Shapes.GetCount(); }
	const GameGridShape& GetShape(U32 _idx) { return m_Shapes[_idx]; }
private:
	GameGridShapeArray	m_Shapes;
END_SUPER_CLASS

#endif
