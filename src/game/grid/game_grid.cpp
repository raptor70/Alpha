#include "game_grid.h"

void GameGridCell::CreateGroundEntity(const Str& _prefab)
{
	m_pGroundEntity = &(World::GetInstance().CreateNewEntity("Cell", m_pParentEntity));
	World::GetInstance().AddNewComponentToEntity(*m_pGroundEntity, "Script");
	m_pGroundEntity->SetPos(m_vPos);
	Str path;
	path.SetFrom("script/prefab/%s.lua", _prefab.GetArray());
	m_pGroundEntity->SetParamStr("ScriptPath", path);
}

//..

void GameGridCell::CreateTowerEntity(const Str& _prefab, const Vec3f& _deltaPos /*= Vec3f::Zero*/)
{
	m_pTowerEntity = &(World::GetInstance().CreateNewEntity("Tower", m_pParentEntity));
	World::GetInstance().AddNewComponentToEntity(*m_pTowerEntity, "Script");
	m_pTowerEntity->SetPos(m_vPos+_deltaPos);
	Str path;
	path.SetFrom("script/prefab/%s.lua", _prefab.GetArray());
	m_pTowerEntity->SetParamStr("ScriptPath", path);
}

//..

void GameGridCell::RemoveTowerEntity()
{
	World::GetInstance().RemoveEntity(m_pTowerEntity);
	m_pTowerEntity = NULL;
}

//..

void GameGridCell::SetGroundColor(const Color& _color)
{
	if(m_pGroundEntity)
		m_pGroundEntity->GetSon(0)->SetColor(_color);
}

//..

S8	GameGridCell::GetPlayerId() const
{
	if ((m_iGameFlag & (GAME_GRID_CELL_WATER)) == 0)
	{
		return (U8)(m_iGameFlag & GAME_GRID_CELL_PLAYERMASK);
	}

	return -1;
}

//..

GameGrid::GameGrid()
{
	{
		GameGridShape& newShape = m_Shapes.AddLastItem();
		newShape.m_iSizeX = 1;
		newShape.m_iSizeY = 1;
		{
			BoolArray& newLine = newShape.m_Pattern.AddLastItem();
			newLine.AddLastItem(TRUE);
		}
	}
	//----------------
	{
		GameGridShape& newShape = m_Shapes.AddLastItem();
		newShape.m_iSizeX = 1;
		newShape.m_iSizeY = 2;
		{
			BoolArray& newLine = newShape.m_Pattern.AddLastItem();
			newLine.AddLastItem(TRUE);
		}
		{
			BoolArray& newLine = newShape.m_Pattern.AddLastItem();
			newLine.AddLastItem(TRUE);
		}
	}
	//----------------
	{
		GameGridShape& newShape = m_Shapes.AddLastItem();
		newShape.m_iSizeX = 1;
		newShape.m_iSizeY = 3;
		{
			BoolArray& newLine = newShape.m_Pattern.AddLastItem();
			newLine.AddLastItem(TRUE);
		}
		{
			BoolArray& newLine = newShape.m_Pattern.AddLastItem();
			newLine.AddLastItem(TRUE);
		}
		{
			BoolArray& newLine = newShape.m_Pattern.AddLastItem();
			newLine.AddLastItem(TRUE);
		}
	}
	//----------------
	{
		GameGridShape& newShape = m_Shapes.AddLastItem();
		newShape.m_iSizeX = 2;
		newShape.m_iSizeY = 2;
		{
			BoolArray& newLine = newShape.m_Pattern.AddLastItem();
			newLine.AddLastItem(TRUE); newLine.AddLastItem(TRUE);
		}
		{
			BoolArray& newLine = newShape.m_Pattern.AddLastItem();
			newLine.AddLastItem(TRUE); newLine.AddLastItem(FALSE);
		}
	}
	//----------------
	{
		GameGridShape& newShape = m_Shapes.AddLastItem();
		newShape.m_iSizeX = 3;
		newShape.m_iSizeY = 2;
		{
			BoolArray& newLine = newShape.m_Pattern.AddLastItem();
			newLine.AddLastItem(FALSE); newLine.AddLastItem(TRUE); newLine.AddLastItem(TRUE);
		}
		{
			BoolArray& newLine = newShape.m_Pattern.AddLastItem();
			newLine.AddLastItem(TRUE); newLine.AddLastItem(TRUE); newLine.AddLastItem(FALSE);
		}
	}
	//----------------
	{
		GameGridShape& newShape = m_Shapes.AddLastItem();
		newShape.m_iSizeX = 2;
		newShape.m_iSizeY = 3;
		{
			BoolArray& newLine = newShape.m_Pattern.AddLastItem();
			newLine.AddLastItem(TRUE); newLine.AddLastItem(TRUE);
		}
		{
			BoolArray& newLine = newShape.m_Pattern.AddLastItem();
			newLine.AddLastItem(TRUE); newLine.AddLastItem(FALSE);
		}
		{
			BoolArray& newLine = newShape.m_Pattern.AddLastItem();
			newLine.AddLastItem(TRUE); newLine.AddLastItem(FALSE);
		}
	}
	//----------------
	{
		GameGridShape& newShape = m_Shapes.AddLastItem();
		newShape.m_iSizeX = 3;
		newShape.m_iSizeY = 2;
		{
			BoolArray& newLine = newShape.m_Pattern.AddLastItem();
			newLine.AddLastItem(TRUE); newLine.AddLastItem(TRUE); newLine.AddLastItem(TRUE);
		}
		{
			BoolArray& newLine = newShape.m_Pattern.AddLastItem();
			newLine.AddLastItem(FALSE); newLine.AddLastItem(TRUE); newLine.AddLastItem(FALSE);
		}
	}
	//----------------
	{
		GameGridShape& newShape = m_Shapes.AddLastItem();
		newShape.m_iSizeX = 3;
		newShape.m_iSizeY = 2;
		{
			BoolArray& newLine = newShape.m_Pattern.AddLastItem();
			newLine.AddLastItem(TRUE); newLine.AddLastItem(TRUE); newLine.AddLastItem(TRUE);
		}
		{
			BoolArray& newLine = newShape.m_Pattern.AddLastItem();
			newLine.AddLastItem(TRUE); newLine.AddLastItem(FALSE); newLine.AddLastItem(TRUE);
		}
	}
}

//..

void	GameGrid::GetCellsWithFlag(U32 _flags, Vec2iArray& _outPos)
{
	for(U32 x=0; x<GetSizeX(); x++)
		for (U32 y = 0; y< GetSizeY(); y++)
		{
			const GameGridCell& cell = GetCell(x, y);
			if ((cell.m_iGameFlag & _flags) == _flags)
			{
				_outPos.AddLastItem(Vec2i(x, y));
			}
		}
}

//..

void	GameGrid::GetCellsForPlayer(U32 _player, U32 _flags, Vec2iArray& _outPos)
{
	for (U32 x = 0; x<GetSizeX(); x++)
		for (U32 y = 0; y< GetSizeY(); y++)
		{
			const GameGridCell& cell = GetCell(x, y);
			if (((cell.m_iGameFlag & _flags) == _flags)
				&& (cell.IsForPlayer(_player)))
			{
				_outPos.AddLastItem(Vec2i(x, y));
			}
		}
}

//..

void GameGrid::ComputeInside()
{
	// first pass, start cells
	for (U32 x = 0; x<GetSizeX(); x++)
		for (U32 y = 0; y < GetSizeY(); y++)
		{
			GameGridCell& cell = GetCell(x, y);
			cell.m_iOutsideDist = -1;
			cell.m_iGameFlag |= GAME_GRID_CELL_INSIDE;
			if (cell.m_iGameFlag & GAME_GRID_CELL_WATER)
			{
				cell.m_iGameFlag &= ~GAME_GRID_CELL_INSIDE;
				cell.m_iOutsideDist = 0;
				continue;
			}

			if ((x == 0) || (y == 0) || (x == (GetSizeX() - 1)) || (y == (GetSizeY() - 1)))
			{
				if ((cell.m_iGameFlag & GAME_GRID_CELL_WALL)==0)
				{
					cell.m_iGameFlag &= ~GAME_GRID_CELL_INSIDE;
					cell.m_iOutsideDist = 0;
					continue;
				}
			}
		}

	// all other pass, compute from border
	static Vec2i neightbors[] =
	{
		Vec2i(-1,-1),
		Vec2i(-1,0),
		Vec2i(-1,1),
		Vec2i(0,1),
		Vec2i(0,-1),
		Vec2i(1,1),
		Vec2i(1,0),
		Vec2i(1,-1)
	};
	U32 curDist = 0;
	U32 count = 0;
	do
	{
		count = 0;
		for (U32 x = 0; x<GetSizeX(); x++)
			for (U32 y = 0; y < GetSizeY(); y++)
			{
				GameGridCell& cell = GetCell(x, y);
				if (curDist == 0)
				{
					if ((cell.m_iGameFlag & GAME_GRID_CELL_WALL) != 0)
					{
						cell.m_iGameFlag &= ~GAME_GRID_CELL_INSIDE;
					}
				}

				if (cell.m_iOutsideDist == curDist)
				{
					for (const Vec2i& n : neightbors)
					{
						Vec2i npos(x + n.x, y + n.y);
						if (npos.x < 0 || npos.y < 0 || npos.x >= (S32)GetSizeX() || npos.y >= (S32)GetSizeY())
							continue;

						GameGridCell& neightbor = GetCell(npos.x, npos.y);
						if (neightbor.m_iOutsideDist == -1 && ((neightbor.m_iGameFlag & GAME_GRID_CELL_WALL) == 0))
						{
							neightbor.m_iGameFlag &= ~GAME_GRID_CELL_INSIDE;
							neightbor.m_iOutsideDist = curDist + 1;
							count++;
						}
					}
				}
			}

		curDist++;
	} while (count != 0);

}