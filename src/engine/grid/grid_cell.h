#ifndef __GRID_CELL_H__
#define __GRID_CELL_H__
 
#define GRID_CELL_BLOCKED	(1<<0)
#define GRID_CELL_OPEN		(1<<1)
#define GRID_CELL_CLOSED	(1<<2)

class GridCell
{
	DECLARE_SCRIPT_CLASS_FINAL(GridCell)
	DECLARE_USE_IN_LIST(GridCell)
public:
	GridCell();
	~GridCell();

	template<class T>
	T*	GetValue() const	{return (T*)m_pValue; }
	template<class T>
	void SetValue(T* _value)	{m_pValue = _value; }

	Bool	HasAllFlag(U32	_flag)	const	{ return (m_iFlag & _flag) == _flag; }
	Bool	HasAnyFlag(U32	_flag)	const	{ return (m_iFlag & _flag) != 0; }
	void	AddFlag(U32	_flag)			{ m_iFlag |= _flag; }
	void	RemoveFlag(U32	_flag)		{ m_iFlag &= ~_flag; }

private:
	void*	m_pValue;
	U32		m_iFlag;
};

#endif