#ifndef __GAME_H__
#define __GAME_H__

#include "application.h"

//--------------------------------------------

BEGIN_SUPER_CLASS(Game,Application)
	DECLARE_SCRIPT_CLASS(Game)
public:
	static Game&	GetInstance() { return *((Game*)g_pInstance); }

	Game();
	virtual ~Game();

	virtual Bool Init(S32 _argc, Char ** _argv);

protected:
	virtual	void InitWorld() OVERRIDE;

END_SUPER_CLASS

#endif
