#ifndef __COMPONENT_GAME_GRID_H__
#define __COMPONENT_GAME_GRID_H__

#include "world/component.h"
#include "grid/game_grid.h"

//-----------------------------------------------------------------------

BEGIN_COMPONENT_CLASS(GameGrid)
	DECLARE_SCRIPT_CLASS(EntityComponentGameGrid)
public:
	EntityComponentGameGrid();
	~EntityComponentGameGrid();

	virtual Bool IsUpdater() const OVERRIDE { return TRUE; }
	virtual Bool IsDrawer() const OVERRIDE { return TRUE; }
	virtual Bool ReceiveEvent()	const OVERRIDE { return FALSE; }

	virtual void	Initialize() OVERRIDE;
	virtual void	Finalize() OVERRIDE;
	virtual void	Update(Float _dTime) OVERRIDE;
	virtual void	Draw(Renderer*	_renderer) OVERRIDE;
	virtual void	OnParamChanged() OVERRIDE;

	INLINE GameGrid&	GetGrid()	{ return m_Grid; }
	INLINE U32	GetGridSizeX() const { return m_Grid.GetSizeX(); }
	INLINE U32	GetGridSizeY() const { return m_Grid.GetSizeY(); }
	
	INLINE GameGridCell& GetCell(U32 _x, U32 _y) { return m_Grid(_x, _y); }
	Vec2i GetRefCell(U32 _x, U32 _y);

	Bool IsPrepared() const { return m_bPrepared; }
	void PrepareFortsForPlayer(U8 _playerId, Vec2iArray& _outTowers);
	void PrepareForts(const Vec2iArray& _towers);
	void PrepareFirstWall(const Vec2i& _startPos);
	void PlaceTower(const Vec2i& _startPos);
	void RemoveTower(const Vec2i& _startPos);
	void Damage(const Vec2i& _refPos);
	void PlaceWall(const Vec2i& _startPos);
	void RemoveWall(const Vec2i& _startPos);
	void UpdateGroundColor(Bool _displayGrid, Bool _displayOutside = FALSE);

protected:
	void Prepare();
	Bool AddWallToPos(U32 _x, U32 _y, U32 _playerId);

	Entity*	m_pCellRoot;
	GameGrid	m_Grid;
	Bool		m_bPrepared;

END_COMPONENT_CLASS

#endif
