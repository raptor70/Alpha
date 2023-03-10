#ifndef __GRID_H__
#define __GRID_H__

#include "grid_cell.h"

template<class T>
class Grid
{
public:
	Grid();
	Grid(U32 _sizeX, U32 _sizeY)	{ Init(_sizeX,_sizeY); }
	virtual ~Grid();

	void	Init(U32 _sizeX, U32 _sizeY);

	U32		GetSizeX() const	{ return m_iSizeX; }
	U32		GetSizeY() const	{ return m_iSizeY; }
	U32		GetNbCell() const	{ return m_Cells.GetCount(); }

	inline ArrayOf<T>& GetCells()						{ return m_Cells; }
	inline T& GetCell(U32 _i)							{ return m_Cells[_i]; }
	inline const T& GetCell(U32 _i)		const			{ return m_Cells[_i]; }
	inline T& GetCell(U32 _x, U32 _y)					{ DEBUG_Require(IsInside(_x,_y)); return m_Cells[_y*m_iSizeX+_x]; }
	inline const T& GetCell(U32 _x, U32 _y) const		{ DEBUG_Require(IsInside(_x,_y)); return m_Cells[_y*m_iSizeX+_x]; }
	inline T& GetCell(const Vec2i& _pos)				{ return GetCell(_pos.x, _pos.y); }
	inline const T& GetCell(const Vec2i& _pos) const	{ return GetCell(_pos.x, _pos.y); }
	inline T& operator[](U32 _i)						{ return GetCell(_i); }
	inline const T& operator[](U32 _i) const			{ return GetCell(_i); }
	inline T& operator()(U32 _i)						{ return GetCell(_i); }
	inline const T& operator()(U32 _i)	const			{ return GetCell(_i); }
	inline T& operator()(U32 _x,U32 _y)					{ return GetCell(_x,_y); }
	inline const T& operator()(U32 _x,U32 _y) const		{ return GetCell(_x,_y); }
	inline T& operator()(const Vec2i& _pos) { return GetCell(_pos.x, _pos.y); }
	inline const T& operator()(const Vec2i& _pos) const { return GetCell(_pos.x, _pos.y); }

	inline T*	GetCellPtr(U32 _x, U32 _y)				{ if (IsInside(_x,_y)) return &GetCell(_x, _y); return NULL; }
	inline const T*	GetCellPtr(U32 _x, U32 _y) const	{ if (IsInside(_x,_y)) return &GetCell(_x, _y); return NULL; }

	Bool	IsInside(U32 _x,U32 _y)	const 				{ return (_x>=0 && _x<m_iSizeX && _y>=0 && _y<m_iSizeY); }
	Bool	IsInside(const Vec2i& _pos)	const			{ return IsInside(_pos.x,_pos.y); }

	Vec2i	GetSafePos(const Vec2i& _pos) const;

	T*		GetNeighbor(U32 _x,U32 _y,S32 _deltax,S32 _deltay);
	void	GetNeighbors(U32 _cellIdx, S32Array& _outNeightbors);

	Bool	ComputePath(U32 _startX,U32 _startY,U32 _endX,U32 _endY, Vec2iArray& _outResult);
	Float	ComputePathScore(U32 _fromIdx, U32 _toIdx);

private:
	U32	m_iSizeX;
	U32	m_iSizeY;
	ArrayOf<T>				m_Cells;
};

#include "grid.hpp"

#endif