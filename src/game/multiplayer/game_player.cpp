#include "game_player.h"

#include "input\input_manager.h"
#include "grid\component_game_grid.h"

BEGIN_SCRIPT_CLASS_INHERITED(GamePlayer, Player)
END_SCRIPT_CLASS

#define MAX_WALLS_LINE 3

GamePlayer::GamePlayer():SUPER()
{
	m_pGrid = NULL;
	m_pRoot = &World::GetInstance().CreateNewEntity("PlayerRoot");

	m_pCursor = &World::GetInstance().CreateNewEntity("Cursor",m_pRoot);
	World::GetInstance().AddNewComponentToEntity(*m_pCursor, "Script");
	m_pCursor->SetParamStr("ScriptPath","script/prefab/Cursor.lua");
	m_pCursor->SetParamBool("ScriptDefaultHide", TRUE);
	
	m_pTower = &World::GetInstance().CreateNewEntity("Tower", m_pRoot);
	World::GetInstance().AddNewComponentToEntity(*m_pTower, "Script");
	m_pTower->SetParamStr("ScriptPath", "script/prefab/Tower.lua");
	m_pTower->SetParamBool("ScriptDefaultHide", TRUE);

	for (U32 i = 0; i < MAX_WALLS_LINE*MAX_WALLS_LINE; i++)
		m_Walls.AddLastItem(CreateNewWall());

	m_WantedState = STATE_NOTHING;
	m_State = STATE_NONE;
	m_iCurrentWallShape = 0;
	m_iCurrentWallRotation = 0;
	m_iCurrentFort = 0;
	m_vRemotePos = m_pCursor->GetWorldPos();
	ResetShootPos();
}

//------------------------------

GamePlayer::~GamePlayer()
{
	World::GetInstance().RemoveEntity(m_pRoot);
}

//------------------------------

void GamePlayer::Update(Float _dTime)
{
	SUPER::Update(_dTime);

	m_pGrid = World::GetInstance().GetComponent<EntityComponentGameGrid>();

	m_fCursorAlpha = 1.f;

	UpdateState(_dTime);

	Color col = ComputeColor();
	col.a = m_fCursorAlpha;	
	if (m_pCursor->GetNbSons() >= 1)
		m_pCursor->GetSon(0)->SetParamColor("QuadColor", col);
}

//------------------------------

Entity*	GamePlayer::CreateNewWall()
{
	Entity* pWall = &World::GetInstance().CreateNewEntity("Wall", m_pRoot);
	World::GetInstance().AddNewComponentToEntity(*pWall, "Script");
	pWall->SetParamStr("ScriptPath", "script/prefab/wall.lua");
	pWall->SetParamBool("ScriptDefaultHide", TRUE);
	return pWall;
}

//------------------------------

const Color& GamePlayer::ComputeColor() const
{
	switch (m_Id)
	{
	case 0: return Color::Red; break;
	case 1: return Color::Green; break;
	case 2: return Color::Blue; break;
	case 3: return Color::Yellow; break;
	}

	return Color::White;
}
//------------------------------

void GamePlayer::FirstFrameInState()
{
	switch (m_State)
	{
	case STATE_CURSOR:
	{
		m_pCursor->Show();
		break;
	}
	case STATE_SELECTFORT:
	{
		m_pCursor->Show();
		FindAllForts();
		m_iCurrentFort = 0;
		m_iNbTowers = 2;
		break;
	}
	case STATE_FORTSELECTED:
	{
		PrepareFirstWalls();
		break;
	};
	case STATE_TOWERPLACEMENT:
	{
		m_pTower->Show();
		
		Vec2iArray fortsSafe;
		m_pGrid->GetGrid().GetCellsForPlayer(m_Id, GAME_GRID_CELL_FORT | GAME_GRID_CELL_REF | GAME_GRID_CELL_INSIDE, fortsSafe);
		m_iNbTowers += fortsSafe.GetCount();
		break;
	}
	case STATE_SHOOT:
	{
		ResetShootPos();
		m_pCursor->Show();
		FindAllTowers();
		m_iCurrentTower = 0;
		break;
	}
	case STATE_WALLPLACEMENT:
	{
		NextShape();

		break;
	}
	}
}

//------------------------------

void GamePlayer::UpdateState(Float _dTime)
{
	// change state
	if (m_WantedState != STATE_NONE)
	{
		LastFrameInState();
		m_State = m_WantedState;
		m_fTimeInState = 0.f;
		FirstFrameInState();
		m_WantedState = STATE_NONE;
	}

	// update
	m_fTimeInState += _dTime;

	switch (m_State)
	{
		case STATE_CURSOR:
		{
			UpdateCursor(_dTime);
			break;
		}
		case STATE_SELECTFORT:
		{
			UpdateSelectFort(_dTime);
			break;
		}
		case STATE_TOWERPLACEMENT:
		{
			UpdateTowerPlacement(_dTime);
			if (!HasTowerToPlace())
			{
				Disable();
			}
			break;
		}
		case STATE_SHOOT:
		{
			UpdateCursor(_dTime);
			UpdateShoot(_dTime);

			break;
		}
		case STATE_WALLPLACEMENT:
		{
			UpdateWallPlacement(_dTime);
			break;
		}
	};
}

//------------------------------

void GamePlayer::LastFrameInState()
{
	switch (m_State)
	{
	case STATE_CURSOR:
	{
		m_pCursor->Hide();
		break;
	}
	case STATE_SELECTFORT:
	{
		Vec3f pos;
		pos.x = Float(m_Forts[m_iCurrentFort].x) + 0.5f;
		pos.y = 0.2f;
		pos.z = Float(m_Forts[m_iCurrentFort].y) + 0.5f;
		m_pTower->SetWorldPos(pos);
		pos.x = Float(m_Forts[m_iCurrentFort].x);
		pos.z = Float(m_Forts[m_iCurrentFort].y);
		for (S32 x = 0; x < MAX_WALLS_LINE; x++)
			for (S32 y = 0; y < MAX_WALLS_LINE; y++)
			{
				U32 idx = y * MAX_WALLS_LINE + x;
				m_Walls[idx]->SetWorldPos(pos+Vec3f(Float(x),0.f,Float(y)));
			}
		
		m_pCursor->Hide();
		break;
	}
	case STATE_TOWERPLACEMENT:
	{
		m_pTower->Hide();
		break;
	}
	case STATE_SHOOT:
	{
		m_pCursor->Hide();
		break;
	}
	case STATE_WALLPLACEMENT:
	{
		for(Entity* ent : m_Walls)
			ent->Hide();
		break;
	}
	};
}

//------------------------------

void GamePlayer::UpdateCursor(Float _dTime)
{
	if (m_Type == TYPE_Local)
	{
		switch (m_Controller)
		{
		case CONTROLLER_KeyboardMouse:
		{
			DeviceKeyboard& keyboard = InputManager::GetInstance().GetKeyboard();
			Vec3f worldPos = m_pCursor->GetWorldPos();
			static Float fSpeed = 400.f;
			if (keyboard.IsPressed(KEY_Z) || keyboard.IsPressed(KEY_Up))
				worldPos.y -= fSpeed * _dTime;
			if (keyboard.IsPressed(KEY_S) || keyboard.IsPressed(KEY_Down))
				worldPos.y += fSpeed * _dTime;
			if (keyboard.IsPressed(KEY_Q) || keyboard.IsPressed(KEY_Left))
				worldPos.x -= fSpeed * _dTime;
			if (keyboard.IsPressed(KEY_D) || keyboard.IsPressed(KEY_Right))
				worldPos.x += fSpeed * _dTime;
			m_pCursor->SetWorldPos(worldPos);
			break;
		}
		case CONTROLLER_Pad0:
		case CONTROLLER_Pad1:
		case CONTROLLER_Pad2:
		case CONTROLLER_Pad3:
		{
			DevicePad& pad = InputManager::GetInstance().GetPad(m_Controller - CONTROLLER_Pad0);
			Vec3f worldPos = m_pCursor->GetWorldPos();
			static Float fSpeed = 400.f;
			if (pad.IsPressed(PADKEY_Dpad_Up))
				worldPos.y -= fSpeed * _dTime;
			if (pad.IsPressed(PADKEY_Dpad_Down))
				worldPos.y += fSpeed * _dTime;
			if (pad.IsPressed(PADKEY_Dpad_Left))
				worldPos.x -= fSpeed * _dTime;
			if (pad.IsPressed(PADKEY_Dpad_Right))
				worldPos.x += fSpeed * _dTime;
			m_pCursor->SetWorldPos(worldPos);
			break;
		}
		}

		m_vRemotePos = m_pCursor->GetWorldPos();
	}
	else // remote
	{
		m_pCursor->SetWorldPos(m_vRemotePos);
		
	}
}

//------------------------------

void GamePlayer::UpdateSelectFort(Float _dTime)
{
	Vec2i delta = Vec2i::Zero;
	if (m_Type == TYPE_Local)
	{
		switch (m_Controller)
		{
		case CONTROLLER_KeyboardMouse:
		{
			DeviceKeyboard& keyboard = InputManager::GetInstance().GetKeyboard();
			if (keyboard.IsJustPressed(KEY_Z) || keyboard.IsJustPressed(KEY_Up))
				delta.y = 1;
			if (keyboard.IsJustPressed(KEY_S) || keyboard.IsJustPressed(KEY_Down))
				delta.y = -1;
			if (keyboard.IsJustPressed(KEY_Q) || keyboard.IsJustPressed(KEY_Left))
				delta.x = 1;
			if (keyboard.IsJustPressed(KEY_D) || keyboard.IsJustPressed(KEY_Right))
				delta.x = -1;

			if (keyboard.IsJustPressed(KEY_Enter)|| keyboard.IsJustPressed(KEY_Space))
				m_WantedState = STATE_FORTSELECTED;

			break;
		}
		case CONTROLLER_Pad0:
		case CONTROLLER_Pad1:
		case CONTROLLER_Pad2:
		case CONTROLLER_Pad3:
		{
			DevicePad& pad = InputManager::GetInstance().GetPad(m_Controller - CONTROLLER_Pad0);
			if (pad.IsJustPressed(PADKEY_Dpad_Up))
				delta.y = 1;
			if (pad.IsJustPressed(PADKEY_Dpad_Down))
				delta.y = -1;
			if (pad.IsJustPressed(PADKEY_Dpad_Left))
				delta.x = 1;
			if (pad.IsJustPressed(PADKEY_Dpad_Right))
				delta.x = -1;

			if (pad.IsJustPressed(PADKEY_A))
				m_WantedState = STATE_FORTSELECTED;
			break;
		}
		}

		U32 mindist = 1000;
		U32 newTower = m_iCurrentFort;
		for (U32 i = 0; i<m_Forts.GetCount(); i++)
		{
			Vec2i pos = m_Forts[i];

			if (m_iCurrentFort == i)
				continue;

			Vec2i deltaPos = pos - m_Forts[m_iCurrentFort];

			U32 curDist = 0;
			if (Abs(deltaPos.x) > Abs(deltaPos.y))
			{
				curDist = Abs(deltaPos.y);
				deltaPos.y = 0;
			}
			else
			{
				curDist = Abs(deltaPos.x);
				deltaPos.x = 0;
			}

			deltaPos *= delta;
			if (deltaPos.x > 0 || deltaPos.y > 0)
			{
				if (curDist < mindist)
				{
					mindist = curDist;
					newTower = i;
				}
			}
		}
		m_iCurrentFort = newTower;
	}
	else // remote
	{

	}

	Vec3f worldPos;
	worldPos.x = Float(m_Forts[m_iCurrentFort].x);
	worldPos.y = 0.f;
	worldPos.z = Float(m_Forts[m_iCurrentFort].y);

	/*Vec3f pos2D = Renderer::GetInstance()->GetViewProjMatrix() * worldPos;
	pos2D.y = -pos2D.y;
	pos2D += Vec3f::One;
	pos2D *= 0.5f;

	Vec3f cursorPos = m_pCursor->GetWorldPos();
	cursorPos.x = pos2D.x * Renderer::GetInstance()->GetViewportSize().x;
	cursorPos.y = pos2D.y * Renderer::GetInstance()->GetViewportSize().y;
	m_pCursor->SetWorldPos(cursorPos);*/
}


//------------------------------

void GamePlayer::FindAllForts()
{
	m_Forts.SetEmpty();
	m_pGrid->GetGrid().GetCellsForPlayer(m_Id,GAME_GRID_CELL_FORT | GAME_GRID_CELL_REF, m_Forts);
}

//------------------------------

void	GamePlayer::PrepareFirstWalls()
{
	m_pGrid->PrepareFirstWall(m_Forts[m_iCurrentFort]);
}

//------------------------------

void GamePlayer::UpdateTowerPlacement(Float _dTime)
{
	Bool bResquestPlacement = FALSE;
	Vec3f delta = Vec3f::Zero;
	if (m_Type == TYPE_Local)
	{
		switch (m_Controller)
		{
		case CONTROLLER_KeyboardMouse:
		{
			DeviceKeyboard& keyboard = InputManager::GetInstance().GetKeyboard();
			
			if (keyboard.IsJustPressed(KEY_Z)|| keyboard.IsJustPressed(KEY_Up))
				delta.z += 1.f;
			if (keyboard.IsJustPressed(KEY_S) || keyboard.IsJustPressed(KEY_Down))
				delta.z -= 1.f;
			if (keyboard.IsJustPressed(KEY_Q)||keyboard.IsJustPressed(KEY_Left))
				delta.x += 1.f;
			if (keyboard.IsJustPressed(KEY_D) || keyboard.IsJustPressed(KEY_Right))
				delta.x -= 1.f;

			bResquestPlacement = keyboard.IsJustPressed(KEY_Enter) || keyboard.IsJustPressed(KEY_Space);
			
			break;
		}
		case CONTROLLER_Pad0:
		case CONTROLLER_Pad1:
		case CONTROLLER_Pad2:
		case CONTROLLER_Pad3:
		{
			DevicePad& pad = InputManager::GetInstance().GetPad(m_Controller - CONTROLLER_Pad0);
			if (pad.IsJustPressed(PADKEY_Dpad_Up))
				delta.z += 1.f;
			if (pad.IsJustPressed(PADKEY_Dpad_Down))
				delta.z -= 1.f;
			if (pad.IsJustPressed(PADKEY_Dpad_Left))
				delta.x += 1.f;
			if (pad.IsJustPressed(PADKEY_Dpad_Right))
				delta.x -= 1.f;
			
			bResquestPlacement = pad.IsJustPressed(PADKEY_A);
			break;
		}
		}
	}
	else // remote
	{
		Vec3f worldPos = m_pTower->GetWorldPos();
		delta = m_vRemotePos - worldPos;
	}

	//move 
	Vec3f worldPos = m_pTower->GetWorldPos();
	S32 x = FLOORINT(worldPos.x);
	S32 y = FLOORINT(worldPos.z);
	if (delta.GetLength2() > Float_Eps)
	{
		worldPos += delta;
		U32 newx = FLOORINT(worldPos.x);
		U32 newy = FLOORINT(worldPos.z);
		if (m_pGrid->GetGrid().GetCell(newx, newy).IsForPlayer(m_Id))
		{
			x = newx;
			y = newy;
			m_pTower->SetWorldPos(worldPos);
		}
	}

	if (m_Type == TYPE_Local)
	{
		m_vRemotePos = m_pTower->GetWorldPos();
	}

	Color col = Color((S32)255, 0, 0, (S32)100);
	if (x > 0 && y > 0 && TowerPlacementValid(x, y))
	{
		col = Color(0, (S32)255, 0, (S32)100);
		if (bResquestPlacement)
		{
			PlaceTower(x, y);
			m_iNbTowers--;
		}
	}

	if (m_pTower->GetNbSons() >= 1)
		m_pTower->GetSon(0)->SetColor(Color(col));
}

//------------------------------

Bool GamePlayer::TowerPlacementValid(S32 _x, S32 _y)
{
	if (_x >= (S32)m_pGrid->GetGridSizeX() - 1 || _y >= (S32)m_pGrid->GetGridSizeX() - 1)
		return FALSE;

	U32 wantedFlag = GAME_GRID_CELL_GROUND | GAME_GRID_CELL_INSIDE | m_Id;
	U32 cellflag = m_pGrid->GetCell(_x, _y).m_iGameFlag;
	if (cellflag != wantedFlag)
		return FALSE;

	cellflag = m_pGrid->GetCell(_x+1, _y).m_iGameFlag;
	if (cellflag != wantedFlag)
		return FALSE;

	cellflag = m_pGrid->GetCell(_x, _y+1).m_iGameFlag;
	if (cellflag != wantedFlag)
		return FALSE;

	cellflag = m_pGrid->GetCell(_x+1, _y+1).m_iGameFlag;
	if (cellflag != wantedFlag)
		return FALSE;

	return TRUE;
}

//------------------------------

void GamePlayer::PlaceTower(S32 _x, S32 _y)
{
	m_pGrid->PlaceTower(Vec2i(_x, _y));
}
//------------------------------

void GamePlayer::FindAllTowers()
{
	m_Towers.SetEmpty();
	m_TowersCooldown.SetEmpty();
	m_pGrid->GetGrid().GetCellsForPlayer(m_Id, GAME_GRID_CELL_TOWER | GAME_GRID_CELL_REF | GAME_GRID_CELL_INSIDE, m_Towers);
	for (const Vec2i& t : m_Towers)
		m_TowersCooldown.AddLastItem(0.f);
}

//------------------------------

void GamePlayer::UpdateShoot(Float _dTime)
{
	for (Float& t : m_TowersCooldown)
	{
		if (t > 0.f)
			t -= _dTime;
	}

	if (m_Towers.GetCount() == 0 || m_TowersCooldown[m_iCurrentTower] > 0.f)
	{

		m_fCursorAlpha = 0.5f;
		return;
	}

	Bool bMustShoot = FALSE;
	if (m_Type == TYPE_Local)
	{
		switch (m_Controller)
		{
		case CONTROLLER_KeyboardMouse:
		{
			DeviceKeyboard& keyboard = InputManager::GetInstance().GetKeyboard();
			bMustShoot = keyboard.IsJustPressed(KEY_Enter) || keyboard.IsJustPressed(KEY_Space);
			break;
		}
		case CONTROLLER_Pad0:
		case CONTROLLER_Pad1:
		case CONTROLLER_Pad2:
		case CONTROLLER_Pad3:
		{
			DevicePad& pad = InputManager::GetInstance().GetPad(m_Controller - CONTROLLER_Pad0);
			bMustShoot = pad.IsJustPressed(PADKEY_A);
			break;
		}
		}
	}
	else // remote
	{

	}

	if (bMustShoot)
	{
		/*Vec3f pos2d = m_pCursor->GetWorldPos();
		const Renderer* pRenderer = Renderer::GetInstance();
		pos2d.x *= 2.f / Float(pRenderer->GetViewportSize().x);
		pos2d.x -= 1.f;
		pos2d.y *= -2.f / Float(pRenderer->GetViewportSize().y);
		pos2d.y += 1.f;
		pos2d.z = 1.f;
		Vec3f p1 = pRenderer->GetInvViewProjMatrix()* pos2d;
		pos2d.z = -1.f;
		Vec3f p2 = pRenderer->GetInvViewProjMatrix()* pos2d;
		Segment seg(p1, p2);
		Vec3f inter;
		if (m_pGrid->GetEntity()->GetBBox().IntersectSegment(seg, inter))
		{
			S32 x = ROUNDINT(inter.x);
			S32 y = ROUNDINT(inter.z);
			GameGridCell& cell = m_pGrid->GetCell(x, y);
			if ((cell.m_iGameFlag & GAME_GRID_CELL_PLAYERMASK) != m_Id) // not friendly fire
			{
				m_vShootPos.x = x;
				m_vShootPos.y = y;
				m_TowersCooldown[m_iCurrentTower] = 5.f;
				m_iCurrentTower = ((m_iCurrentTower + 1) % m_Towers.GetCount());
			}
		}*/
	}
}

//------------------------------

Mat22 g_WallRot[] = {
	Mat22(1.0f, 0.f, 0.f, 1.f),
	Mat22(0.0f, 1.f, -1.f, 0.f),
	Mat22(-1.0f, 0.f, 0.f, -1.f),
	Mat22(0.0f, -1.f, 1.f, 0.f)
};

void GamePlayer::UpdateWallPlacement(Float _dTime)
{
	const GameGridShape& shape = m_pGrid->GetGrid().GetShape(m_iCurrentWallShape);
	
	Bool bResquestPlacement = FALSE;
	Vec3f delta = Vec3f::Zero;
	if (m_Type == TYPE_Local)
	{
		switch (m_Controller)
		{
		case CONTROLLER_KeyboardMouse:
		{
			DeviceKeyboard& keyboard = InputManager::GetInstance().GetKeyboard();
			if (keyboard.IsJustPressed(KEY_Z) || keyboard.IsJustPressed(KEY_Up))
				delta.z += 1.f;
			if (keyboard.IsJustPressed(KEY_S) || keyboard.IsJustPressed(KEY_Down))
				delta.z -= 1.f;
			if (keyboard.IsJustPressed(KEY_Q) || keyboard.IsJustPressed(KEY_Left))
				delta.x += 1.f;
			if (keyboard.IsJustPressed(KEY_D) || keyboard.IsJustPressed(KEY_Right))
				delta.x -= 1.f;
			
			if (keyboard.IsJustPressed(KEY_A)|| keyboard.IsJustPressed(KEY_PageUp))
				NextShapeRotation(1);
			if (keyboard.IsJustPressed(KEY_E) || keyboard.IsJustPressed(KEY_PageDown))
				NextShapeRotation(-1);

			bResquestPlacement = keyboard.IsJustPressed(KEY_Enter) || keyboard.IsJustPressed(KEY_Space);
			break;
		}
		case CONTROLLER_Pad0:
		case CONTROLLER_Pad1:
		case CONTROLLER_Pad2:
		case CONTROLLER_Pad3:
		{
			DevicePad& pad = InputManager::GetInstance().GetPad(m_Controller - CONTROLLER_Pad0);
			if (pad.IsJustPressed(PADKEY_Dpad_Up))
				delta.z += 1.f;
			if (pad.IsJustPressed(PADKEY_Dpad_Down))
				delta.z -= 1.f;
			if (pad.IsJustPressed(PADKEY_Dpad_Left))
				delta.x += 1.f;
			if (pad.IsJustPressed(PADKEY_Dpad_Right))
				delta.x -= 1.f;
			
			if (pad.IsJustPressed(PADKEY_X))
				NextShapeRotation(1);
			if (pad.IsJustPressed(PADKEY_B))
				NextShapeRotation(-1);

			bResquestPlacement = pad.IsJustPressed(PADKEY_A);
			break;
		}
		}
	}
	else // remote
	{
		Vec3f worldPos = m_Walls[0]->GetWorldPos();
		delta = m_vRemotePos - worldPos;
		UpdateWallsVisibility();
	}

	//move 
	Vec3f worldPos = m_Walls[0]->GetWorldPos();
	S32 targetx = ROUNDINT(worldPos.x);
	S32 targety = ROUNDINT(worldPos.z);
	if (delta.GetLength2() > Float_Eps)
	{
		Vec3f newWorldPos = worldPos + delta;
		U32 newx = ROUNDINT(newWorldPos.x);
		U32 newy = ROUNDINT(newWorldPos.z);
		
		if (m_pGrid->GetGrid().GetCell(newx, newy).IsForPlayer(m_Id))
		{
			worldPos = newWorldPos;
			targetx = newx;
			targety = newy;
		}
	}

	Bool bAllValid = TRUE;
	for (U32 x = 0; x < shape.m_iSizeX; x++)
		for (U32 y = 0; y < shape.m_iSizeY; y++)
		{
			if (shape.IsWall(x, y))
			{
				Vec2i rotPos = ComputeWallRotatedPos(Vec2i(x, y));
				if (!WallPlacementValid(targetx + rotPos.x, targety + rotPos.y))
				{
					bAllValid = FALSE;
					break;
				}
			}
		}

	Color col = Color((S32)255, 0, 0, (S32)100);
	if (bAllValid)
	{
		col = Color(0, (S32)255, 0, (S32)100);
		if (bResquestPlacement)
		{
			for (U32 x = 0; x < shape.m_iSizeX; x++)
				for (U32 y = 0; y < shape.m_iSizeY; y++)
					if (shape.IsWall(x, y))
					{
						Vec2i rotPos = ComputeWallRotatedPos(Vec2i(x, y));
						PlaceWall(targetx + rotPos.x, targety + rotPos.y);
					}

			NextShape();
		}
	}

	if (m_Type == TYPE_Local)
	{
		m_vRemotePos = worldPos;
	}

	for (U32 x = 0; x < shape.m_iSizeX; x++)
		for (U32 y = 0; y < shape.m_iSizeY; y++)
		{
			U32 idx = y * MAX_WALLS_LINE + x;
			Vec2i rotPos = ComputeWallRotatedPos(Vec2i(x, y));
			m_Walls[idx]->SetWorldPos(worldPos + Vec3f(Float(rotPos.x), 0.f, Float(rotPos.y)));
			if (m_Walls[idx]->GetNbSons() >= 1)
				m_Walls[idx]->GetSon(0)->SetColor(Color(col));
		}
}

//------------------------------

Bool GamePlayer::WallPlacementValid(S32 _x, S32 _y)
{
	if (_x < 0 || _y < 0 || _x >= (S32)m_pGrid->GetGridSizeX() || _y >= (S32)m_pGrid->GetGridSizeY())
		return FALSE;

	U32 wantedFlag = GAME_GRID_CELL_GROUND | m_Id;
	U32 cellflag = m_pGrid->GetCell(_x, _y).m_iGameFlag;
	if ((cellflag & (~GAME_GRID_CELL_INSIDE)) != wantedFlag)
		return FALSE;

	return TRUE;
}

//------------------------------

void GamePlayer::PlaceWall(S32 _x, S32 _y)
{
	DEBUG_Require(_x >= 0 && _y >= 0 && _x < (S32)m_pGrid->GetGridSizeX() && _y < (S32)m_pGrid->GetGridSizeY());
	m_pGrid->PlaceWall(Vec2i(_x, _y));
}

//------------------------------

void GamePlayer::NextShape()
{
	U32 nbShapes = m_pGrid->GetGrid().GetShapeCount();
	m_iCurrentWallShape = (m_iCurrentWallShape + Random(1, (S32)(nbShapes - 1))) % nbShapes;

	UpdateWallsVisibility();
}

//------------------------------

void GamePlayer::NextShapeRotation(U32 _delta)
{
	m_iCurrentWallRotation = ((m_iCurrentWallRotation + _delta + _countof(g_WallRot)) % _countof(g_WallRot));
	UpdateWallsVisibility();
}

//------------------------------

void GamePlayer::UpdateWallsVisibility()
{
	const GameGridShape& shape = m_pGrid->GetGrid().GetShape(m_iCurrentWallShape);
	for (U32 x = 0; x < MAX_WALLS_LINE; x++)
		for (U32 y = 0; y < MAX_WALLS_LINE; y++)
		{
			U32 idx = y * MAX_WALLS_LINE + x;
			if (shape.IsWall(x, y))
				m_Walls[idx]->Show();
			else
				m_Walls[idx]->Hide();
		}
}

//------------------------------

Vec2i GamePlayer::ComputeWallRotatedPos(const Vec2i& _pos)
{
	Vec2f fPos = g_WallRot[m_iCurrentWallRotation] * Vec2f(Float(_pos.x), Float(_pos.y));
	return Vec2i(ROUNDINT(fPos.x), ROUNDINT(fPos.y));
}

//------------------------------

void	GamePlayer::ComputePlayerInfoMessage(SNetworkMessage& _message, Bool _write)
{
	SUPER::ComputePlayerInfoMessage(_message, _write);

	if (_write)
	{
		_message.m_Stream.WriteU32(m_State);
		_message.m_Stream.WriteU32(m_iCurrentFort);
		_message.m_Stream.WriteU32(m_iCurrentWallShape);
		_message.m_Stream.WriteU32(m_iCurrentWallRotation);
		_message.m_Stream.WriteU32(m_iNbTowers);

		_message.m_Stream.WriteFloat(m_vRemotePos.x);
		_message.m_Stream.WriteFloat(m_vRemotePos.y);
		_message.m_Stream.WriteFloat(m_vRemotePos.z);
		
	}
	else
	{
		EState newState = (EState)_message.m_Stream.ReadU32();
		if (newState != m_State)
			m_WantedState = newState;

		m_iCurrentFort = _message.m_Stream.ReadU32();
		m_iCurrentWallShape = _message.m_Stream.ReadU32();
		m_iCurrentWallRotation = _message.m_Stream.ReadU32();
		m_iNbTowers = _message.m_Stream.ReadU32();

		m_vRemotePos.x = _message.m_Stream.ReadFloat();
		m_vRemotePos.y = _message.m_Stream.ReadFloat();
		m_vRemotePos.z = _message.m_Stream.ReadFloat();
	}
}