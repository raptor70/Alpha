#include "component_game_grid.h"

BEGIN_SCRIPT_CLASS_INHERITED(EntityComponentGameGrid,EntityComponent)
END_SCRIPT_CLASS

DEFINE_COMPONENT_CLASS(GameGrid)
	INIT_ENTITY_PARAMS(S32, SizeX, 256);
	INIT_ENTITY_PARAMS(S32, SizeY, 256);
END_DEFINE_COMPONENT

//------------------------------

EntityComponentGameGrid::EntityComponentGameGrid():SUPER()
{
	m_pCellRoot = NULL;
	m_bPrepared = FALSE;
}

//------------------------------

EntityComponentGameGrid::~EntityComponentGameGrid()
{
}

//------------------------------

void	EntityComponentGameGrid::Initialize()
{
	SUPER::Initialize();

	Prepare();
}

//------------------------------

void	EntityComponentGameGrid::Finalize()
{
	SUPER::Finalize();
}

//------------------------------

void	EntityComponentGameGrid::OnParamChanged() 
{
	SUPER::OnParamChanged();
}

//------------------------------

void	EntityComponentGameGrid::Update(Float _dTime)
{
	SUPER::Update(_dTime);
}

//------------------------------

void EntityComponentGameGrid::Draw(Renderer*	_renderer)
{
	SUPER::Draw(_renderer);
}

//------------------------------

void EntityComponentGameGrid::Prepare()
{
	if (m_pCellRoot)
	{
		World::GetInstance().RemoveEntity(m_pCellRoot);
	}

	m_pCellRoot = &(World::GetInstance().CreateNewEntity("CellsRoot", GetEntity()));

	S32 sizex = GET_ENTITY_PARAMS(S32, GameGridSizeX);
	S32 sizey = GET_ENTITY_PARAMS(S32, GameGridSizeY);
	m_Grid.Init(sizex, sizey);
	Vec3f size = Vec3f((Float)sizex, 0.01f, (Float)sizey);
	Vec3f center = Vec3f(size.x*.5f-0.5f, 0.f, size.z*.5f-0.5f);
	GetEntity()->SetBBox(Box(size, center));

	// init grid
	S32 halfX = sizex / 2;
	S32 halfY = sizey / 2;
	for (S32 x = 0; x<sizex; x++)
		for (S32 y = 0; y < sizey; y++)
		{
			GameGridCell& cell = m_Grid.GetCell(x, y);
			cell.m_pParentEntity = m_pCellRoot;
			cell.m_vPos = Vec3f(Float(x), 0.f, Float(y));
			if (x == halfX - 1 || x == halfX || y == halfY - 1 || y == halfY)
			{
				cell.m_iGameFlag |= GAME_GRID_CELL_WATER;
			}
			else
			{
				cell.m_iGameFlag |= GAME_GRID_CELL_GROUND;
				cell.CreateGroundEntity("ground");

				if (x < halfX)
				{
					if (y < halfY)
						cell.m_iGameFlag |= GAME_GRID_CELL_PLAYER0;
					else
						cell.m_iGameFlag |= GAME_GRID_CELL_PLAYER1;
				}
				else
				{
					if (y < halfY)
						cell.m_iGameFlag |= GAME_GRID_CELL_PLAYER2;
					else
						cell.m_iGameFlag |= GAME_GRID_CELL_PLAYER3;
				}
			}
		}

	// water
	struct Neighbor
	{
		Vec2i	m_vPos;
		Quat	m_qRot;
	} g_neightbors[] = {
		Vec2i(-1,0),	Quat(MATH_PI_DIV_2,Vec3f::YAxis),
		Vec2i(1,0),		Quat(-MATH_PI_DIV_2,Vec3f::YAxis),
		Vec2i(0,-1),	Quat(0.f,Vec3f::YAxis),
		Vec2i(0,1),		Quat(MATH_PI,Vec3f::YAxis)
	};

	for (S32 x = 0; x<sizex; x++)
		for (S32 y = 0; y < sizey; y++)
		{
			GameGridCell& cell = m_Grid.GetCell(x, y);
			if ((cell.m_iGameFlag & GAME_GRID_CELL_WATER) != 0)
			{
				Quat rot = Quat::Identity;
				Bool bFound = FALSE;
				for (S32 n = 0; n < _countof(g_neightbors); n++)
				{
					const GameGridCell* pNeightborcell = m_Grid.GetCellPtr(x + g_neightbors[n].m_vPos.x, y + g_neightbors[n].m_vPos.y);
					if (pNeightborcell && ((pNeightborcell->m_iGameFlag & GAME_GRID_CELL_GROUND) != 0))
					{
						bFound = TRUE;
						rot = g_neightbors[n].m_qRot;
						break;
					}
				}

				if (bFound)
				{
					cell.CreateGroundEntity("water");
					cell.m_pGroundEntity->SetRot(rot);
				}
			}
		}

	m_bPrepared = TRUE;
}

//------------------------------

void EntityComponentGameGrid::PrepareFortsForPlayer(U8 _playerId, Vec2iArray& _outForts)
{
	// tower
	S32 sizex = GET_ENTITY_PARAMS(S32, GameGridSizeX);
	S32 sizey = GET_ENTITY_PARAMS(S32, GameGridSizeY);
	S32 halfX = sizex / 2;
	S32 halfY = sizey / 2;
	Vec2i playerZoneSize(halfX - 1, halfY - 1);
	struct PlayerInfo
	{
		Vec2i m_vMin;
	} players[] =
	{
		Vec2i(0,0),
		Vec2i(0,sizey - playerZoneSize.y),
		Vec2i(sizex - playerZoneSize.x,0),
		Vec2i(sizex - playerZoneSize.x,sizey - playerZoneSize.y),
	};

	S32 deltaBorder = 2;
	Vec2i fortZoneSize((playerZoneSize.x / 2) - 2 * deltaBorder, (playerZoneSize.y / 2) - 2 * deltaBorder);
	struct TowerInfo
	{
		Vec2i m_vMin;
	} forts[] =
	{
		Vec2i(deltaBorder,deltaBorder),
		Vec2i(playerZoneSize.x - deltaBorder - fortZoneSize.x,deltaBorder),
		Vec2i(deltaBorder,playerZoneSize.y - fortZoneSize.y - deltaBorder),
		Vec2i(playerZoneSize.x - deltaBorder - fortZoneSize.x,playerZoneSize.y - fortZoneSize.y - deltaBorder),
	};

	DEBUG_Require(_playerId >= 0 && _playerId < _countof(players));
	const Vec2i& vMin = players[_playerId].m_vMin;
	Vec2i vMax = vMin + playerZoneSize - Vec2i::One;
	for (U32 t = 0; t < _countof(forts); t++)
	{
		Vec2i fortMin = vMin + forts[t].m_vMin;
		Vec2i fortMax = fortMin + fortZoneSize - Vec2i::One;
		Vec2i startPos(Random(fortMin.x, fortMax.x - 1), Random(fortMin.y, fortMax.y - 1));
		_outForts.AddLastItem(startPos);
	}
}

//------------------------------

void EntityComponentGameGrid::PrepareForts(const Vec2iArray& _forts)
{
	for (const Vec2i& fort : _forts)
	{
		GameGridCell* cell = m_Grid.GetCellPtr(fort.x, fort.y);
		cell->m_iGameFlag |= GAME_GRID_CELL_FORT | GAME_GRID_CELL_REF;
		cell->CreateTowerEntity("fort");
		cell = m_Grid.GetCellPtr(fort.x + 1, fort.y);
		cell->m_iGameFlag |= GAME_GRID_CELL_FORT;
		cell = m_Grid.GetCellPtr(fort.x, fort.y + 1);
		cell->m_iGameFlag |= GAME_GRID_CELL_FORT;
		cell = m_Grid.GetCellPtr(fort.x + 1, fort.y + 1);
		cell->m_iGameFlag |= GAME_GRID_CELL_FORT;
	}
}

//------------------------------

Vec2i EntityComponentGameGrid::GetRefCell(U32 _x, U32 _y)
{
	Vec2i curPos(_x, _y);
	GameGridCell& curCell = GetCell(curPos.x, curPos.y);
	if (curCell.m_iGameFlag & (GAME_GRID_CELL_FORT | GAME_GRID_CELL_TOWER))
	{
		if (curCell.m_iGameFlag & GAME_GRID_CELL_REF)
			return curPos;
		if (_x > 0)
		{
			curPos.x = _x - 1;
			curPos.y = _y;
			GameGridCell& cell1 = GetCell(curPos.x, curPos.y);
			if (cell1.m_iGameFlag & GAME_GRID_CELL_REF)
				return curPos;
			
			if(_y > 0)
			{
				curPos.x = _x - 1;
				curPos.y = _y - 1;
				GameGridCell& cell2 = GetCell(curPos.x, curPos.y);
				if (cell2.m_iGameFlag & GAME_GRID_CELL_REF)
					return curPos;
			}
		}
		
		if (_y > 0)
		{
			curPos.x = _x;
			curPos.y = _y -1;
			GameGridCell& cell3 = GetCell(curPos.x, curPos.y);
			if (cell3.m_iGameFlag & GAME_GRID_CELL_REF)
				return curPos;
		}
	}

	return curPos;
}

//------------------------------

void EntityComponentGameGrid::Damage(const Vec2i& _refPos)
{
	GameGridCell& curCell = GetCell(_refPos.x, _refPos.y);
	curCell.m_iLife--;
	if (curCell.m_iLife == 0)
	{
		U32 flag = (GAME_GRID_CELL_REF | GAME_GRID_CELL_TOWER);
		if (curCell.m_iGameFlag & flag)
		{
			RemoveTower(_refPos);
			return;
		}

		flag = (GAME_GRID_CELL_WALL);
		if (curCell.m_iGameFlag & flag)
		{
			RemoveWall(_refPos);
			return;
		}
	}
}

//------------------------------

void EntityComponentGameGrid::PrepareFirstWall(const Vec2i& _startPos)
{

	// first walls
	Vec2i posmax = m_Grid.GetSafePos(_startPos + Vec2i(4, 4));
	Vec2i posmin = m_Grid.GetSafePos(_startPos - Vec2i(3, 3));

	GameGridCell* cell = m_Grid.GetCellPtr(_startPos.x, _startPos.y);
	U32 playerId = cell->m_iGameFlag & GAME_GRID_CELL_PLAYERMASK;
	
	for (S32 x = posmin.x; x <= posmax.x; x++)
	{
		// min
		for (S32 y = posmin.y; y <= posmax.y; y++)
			if (AddWallToPos(x,y,playerId))
				break;

		// max
		for (S32 y = posmax.y; y >= posmin.y; y--)
			if (AddWallToPos(x, y, playerId))
				break;
	}

	for (S32 y = posmin.y; y <= posmax.y; y++)
	{
		// min
		for (S32 x = posmin.x; x <= posmax.x; x++)
			if (AddWallToPos(x, y, playerId))
				break;

		// max
		for (S32 x = posmax.x; x >= posmin.x; x--)
			if (AddWallToPos(x, y, playerId))
				break;
	}
}

//------------------------------

void EntityComponentGameGrid::PlaceTower(const Vec2i& _startPos)
{
	GameGridCell* cell = m_Grid.GetCellPtr(_startPos.x, _startPos.y);
	cell->m_iGameFlag |= GAME_GRID_CELL_TOWER | GAME_GRID_CELL_REF;
	cell->m_iLife = 3;
	cell->CreateTowerEntity("tower",Vec3f(0.5f,0.f,0.5f));
	cell = m_Grid.GetCellPtr(_startPos.x+1, _startPos.y);
	cell->m_iGameFlag |= GAME_GRID_CELL_TOWER;
	cell = m_Grid.GetCellPtr(_startPos.x, _startPos.y+1);
	cell->m_iGameFlag |= GAME_GRID_CELL_TOWER;
	cell = m_Grid.GetCellPtr(_startPos.x+1, _startPos.y+1);
	cell->m_iGameFlag |= GAME_GRID_CELL_TOWER;
}

//------------------------------

void EntityComponentGameGrid::RemoveTower(const Vec2i& _startPos)
{
	GameGridCell* cell = m_Grid.GetCellPtr(_startPos.x, _startPos.y);
	cell->m_iGameFlag &= (~(GAME_GRID_CELL_TOWER | GAME_GRID_CELL_REF));
	cell->m_iLife = 0;
	cell->RemoveTowerEntity();
	cell = m_Grid.GetCellPtr(_startPos.x + 1, _startPos.y);
	cell->m_iGameFlag &= (~GAME_GRID_CELL_TOWER);
	cell = m_Grid.GetCellPtr(_startPos.x, _startPos.y + 1);
	cell->m_iGameFlag &= (~GAME_GRID_CELL_TOWER);
	cell = m_Grid.GetCellPtr(_startPos.x + 1, _startPos.y + 1);
	cell->m_iGameFlag &= (~GAME_GRID_CELL_TOWER);
}

//------------------------------

void EntityComponentGameGrid::PlaceWall(const Vec2i& _startPos)
{
	GameGridCell* cell = m_Grid.GetCellPtr(_startPos.x, _startPos.y);
	cell->m_iGameFlag |= GAME_GRID_CELL_WALL;
	cell->m_iLife = 1;
	cell->CreateTowerEntity("wall");
}

//------------------------------

void EntityComponentGameGrid::RemoveWall(const Vec2i& _startPos)
{
	GameGridCell* cell = m_Grid.GetCellPtr(_startPos.x, _startPos.y);
	cell->m_iGameFlag &= (~GAME_GRID_CELL_WALL);
	cell->m_iLife = 0;
	cell->RemoveTowerEntity();
}

//------------------------------

void EntityComponentGameGrid::UpdateGroundColor(Bool _displayGrid, Bool _displayOutside /*=FALSE*/)
{
	if (_displayGrid)
	{
		for(U32 x=0;x<m_Grid.GetSizeX();x++)
			for (U32 y = 0; y < m_Grid.GetSizeY(); y++)
			{
				GameGridCell& cell = m_Grid.GetCell(x, y);
				U32 sum = x + y;
				if ((cell.m_iGameFlag & GAME_GRID_CELL_INSIDE) != 0)
				{
					if (sum % 2 == 0)
						cell.SetGroundColor(Color::Cyan);
					else
						cell.SetGroundColor(Color::LightCyan);
				}
				else
				{
					
					if (_displayOutside && sum % 2 == 0 && ((cell.m_iGameFlag & GAME_GRID_CELL_GROUND) != 0))
						cell.SetGroundColor(Color::LightGrey);
					else
						cell.SetGroundColor(Color::White);
				}
			}
	}
	else
	{
		for (GameGridCell& cell : m_Grid.GetCells())
			cell.SetGroundColor(Color::White);
	}
}

//------------------------------

Bool EntityComponentGameGrid::AddWallToPos(U32 _x, U32 _y, U32 _playerId)
{
	GameGridCell* cell = m_Grid.GetCellPtr(_x, _y);
	if (((cell->m_iGameFlag & GAME_GRID_CELL_PLAYERMASK) == _playerId)
		&& ((cell->m_iGameFlag & GAME_GRID_CELL_GROUND) != 0)
		)
	{
		if ((cell->m_iGameFlag & GAME_GRID_CELL_WALL) == 0)
		{
			cell->m_iGameFlag |= GAME_GRID_CELL_WALL;
			cell->m_iLife = 1;
			cell->CreateTowerEntity("wall");
		}
		return TRUE;
	}

	return FALSE;
}