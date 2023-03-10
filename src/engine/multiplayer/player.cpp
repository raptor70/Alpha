#include "player.h"

BEGIN_SCRIPT_CLASS(Player)
END_SCRIPT_CLASS

Player::Player()
{
	m_Type = TYPE_None;
	m_Controller = CONTROLLER_None;
}

//------------------------------

Player::~Player()
{
}

//------------------------------

void	Player::Update(Float _dTime)
{
}