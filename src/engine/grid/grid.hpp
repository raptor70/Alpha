
template<class T>
Grid<T>::Grid()
{
	m_iSizeX = 0;
	m_iSizeY = 0;
}

//------------------------------

template<class T>
Grid<T>::~Grid()
{
}

//------------------------------

template<class T>
void Grid<T>::Init(U32 _sizeX, U32 _sizeY)
{
	DEBUG_Require(_sizeX>0 && _sizeY>0);
	m_iSizeX = _sizeX;
	m_iSizeY = _sizeY;
	m_Cells.SetItemCount(m_iSizeX*m_iSizeY);
}

//------------------------------

template<class T>
Vec2i	Grid<T>::GetSafePos(const Vec2i& _pos) const
{
	return Vec2i(
		Max<S32>(0,Min<S32>(_pos.x, m_iSizeX - 1)),
		Max<S32>(0, Min<S32>(_pos.y, m_iSizeY - 1))
		);
}

//------------------------------

template<class T>
T*	Grid<T>::GetNeighbor(U32 _x,U32 _y,S32 _deltax,S32 _deltay)
{
	DEBUG_Require(_x>=0 && _x<m_iSizeX);
	DEBUG_Require(_y>=0 && _y<m_iSizeY);

	U32 nx = _x + _deltax;
	if( nx >= 0 && nx<m_iSizeX)
	{
		U32 ny = _y + _deltay;
		if( ny >= 0 && ny<m_iSizeY)
		{
			return &GetCell(nx,ny);
		}
	}

	return NULL;
}

//------------------------------

template<class T>
void	Grid<T>::GetNeighbors(U32 _cellIdx, S32Array& _outNeightbors)
{
	DEBUG_Require(_cellIdx >=0 && _cellIdx < m_Cells.GetCount());

	_outNeightbors.SetEmpty();

	U32 cellX = (_cellIdx%m_iSizeX);
	U32 cellY = (_cellIdx/m_iSizeX);

	if( cellX >= 1 )
	{
		if( cellY >= 1)
		{
			_outNeightbors.AddLastItem(_cellIdx-m_iSizeX); // (0,-1)
		}

		_outNeightbors.AddLastItem(_cellIdx-1); // (-1, 0)

		if( cellY <= m_iSizeY-2 )
		{
			_outNeightbors.AddLastItem(_cellIdx+m_iSizeX); // (0, 1)
		}
	}
	else // cell == 0
	{
		if( cellY >= 1 )
		_outNeightbors.AddLastItem(_cellIdx-m_iSizeX); // (0, -1)

		if( cellY <= m_iSizeY-2 )
			_outNeightbors.AddLastItem(_cellIdx+m_iSizeX); // (0, 1)
	}
	
	if( cellX <= m_iSizeX-2 )
	{
		_outNeightbors.AddLastItem(_cellIdx+1); // (1, 0)
	}	
}

//------------------------------

template<class T>
Bool	Grid<T>::ComputePath(U32 _startX,U32 _startY,U32 _endX,U32 _endY, Vec2iArray& _outResult)
{
	DEBUG_Require(_startX>=0 && _startX<m_iSizeX);
	DEBUG_Require(_endX>=0 && _endX<m_iSizeX);
	DEBUG_Require(_startY>=0 && _startY<m_iSizeY);
	DEBUG_Require(_endY>=0 && _endY<m_iSizeY);

	// init
	for(U32 i=0; i<m_Cells.GetCount(); i++)
		m_Cells[i].RemoveFlag(GRID_CELL_CLOSED|GRID_CELL_OPEN);

	U32 startIdx = _startY * m_iSizeX + _startX;
	U32 endIdx = _endY * m_iSizeX + _endX;
	
	S32Array froms;
	FloatArray estimateScores;
	FloatArray fromStartScores;
	U32 cellCount = m_Cells.GetCount();
	froms.SetItemCount(cellCount);
	estimateScores.SetItemCount(cellCount);
	fromStartScores.SetItemCount(cellCount);
	for(U32 i=0; i<cellCount; i++)
	{
		froms[i] = -1;
		estimateScores[i] = 1e7f;
		fromStartScores[i] = 1e7f;
	}

	S32Array openList;
	S32Array neighborList;

	openList.AddLastItem(startIdx);
	m_Cells[startIdx].AddFlag(GRID_CELL_OPEN);
	fromStartScores[startIdx] = 0.f;
	estimateScores[startIdx] = ComputePathScore(startIdx,endIdx);

	while(openList.GetCount() > 0 )
	{
		// find next minimum path
		U32 c = 0;
		for(U32 o=1; o<openList.GetCount(); o++)
		{
			if( estimateScores[openList[o]] < estimateScores[openList[c]] )
				c = o;
		}

		// check end of path
		S32 currentIdx = openList[c]; 
		if( currentIdx == endIdx ) // FOUND
		{
			_outResult.SetEmpty();
			while(currentIdx >= 0 )
			{
				_outResult.AddLastItem(Vec2i(currentIdx%m_iSizeX,currentIdx/m_iSizeX));
				currentIdx = froms[currentIdx];
			}
			_outResult.Reverse();
			return TRUE;
		}

		// put current cell from open to close list
		openList.RemoveItems(c,1);

		T& cell = m_Cells[currentIdx];
		cell.RemoveFlag(GRID_CELL_OPEN);
		cell.AddFlag(GRID_CELL_CLOSED);

		// browse neighbors
		GetNeighbors(currentIdx,neighborList);
		for(U32 n=0; n<neighborList.GetCount(); n++)
		{
			// neighbors already browsed
			U32 neighborIdx = neighborList[n];

			T& neighborCell = m_Cells[neighborIdx];
			if( !cell.CanGoTo(neighborCell) || neighborCell.HasAnyFlag(GRID_CELL_BLOCKED|GRID_CELL_CLOSED) )
				continue; 

			// compute score from start for the neighbor
			Float neighborFromStartScore = fromStartScores[currentIdx] + 1.f;

			// check never saw
			Float estimateScore = 0.f;
			if( neighborCell.HasAnyFlag(GRID_CELL_OPEN) )
			{
				if( neighborFromStartScore >= fromStartScores[neighborIdx]) // check better path for this cell
					continue;
			}
			else
			{
				openList.AddLastItem(neighborIdx);
				neighborCell.AddFlag(GRID_CELL_OPEN);
			}

			froms[neighborIdx] = currentIdx;
			fromStartScores[neighborIdx] = neighborFromStartScore;
			estimateScores[neighborIdx] = neighborFromStartScore  + ComputePathScore(neighborIdx,endIdx);
		}
	}

	return FALSE;
}
	
//------------------------------

template<class T>
Float	Grid<T>::ComputePathScore(U32 _fromIdx, U32 _toIdx)
{
	DEBUG_Require(_fromIdx >=0 && _fromIdx < m_Cells.GetCount());
	DEBUG_Require(_toIdx >=0 && _toIdx < m_Cells.GetCount());

	Vec2f from;
	Vec2f to;

	from.x = Float(_fromIdx%m_iSizeX);
	from.y = Float(_fromIdx/m_iSizeX); 
	to.x = Float(_toIdx%m_iSizeX);
	to.y = Float(_toIdx/m_iSizeX); 

	return (to - from).GetLength2();
}