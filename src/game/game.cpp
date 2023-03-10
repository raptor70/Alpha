#include "game.h"

BEGIN_SCRIPT_CLASS_INHERITED(Game,Application)
END_SCRIPT_CLASS

Game::Game():SUPER()
{
}

//---------------------------------

Game::~Game()
{
}

//---------------------------------

Bool Game::Init(S32 _argc, Char ** _argv)
{
	if (!SUPER::Init(_argc, _argv))
		return FALSE;

	return TRUE;
}

void Game::InitWorld()
{
	SUPER::InitWorld();

	World& world = World::GetInstance();
	world.AddNewComponentToEntity(*world.GetRoot(), "GameMultiplayer");
}