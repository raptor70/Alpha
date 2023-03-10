#include "window_menu.h"

#include "platform/platform_win32.h"

WindowMenu::WindowMenu( Str _name )
{   
#ifdef WIN32
	m_pPlatformData = NEW WIN32MenuPlatformData;
#endif
	m_sName = _name;
}

//..

void WindowMenu::AddCommand(const Char* _name, S32 _commandId)
{
	m_aCommands.AddLastItem(NEW MenuCommand(_name,_commandId));
}

//..

Bool WindowMenu::Create(const PtrTo<Object>& _pParentPlatformData)
{
#ifdef WIN32
	const WIN32MenuPlatformData* pParentData = _pParentPlatformData.GetDynamicCastPtr<WIN32MenuPlatformData>();
	WIN32MenuPlatformData* pData = m_pPlatformData.GetDynamicCastPtr<WIN32MenuPlatformData>();
	
	pData->m_hMenu = CreateMenu();

	for(U32 i=0; i<m_aCommands.GetCount(); i++)
	{
		const RefTo<MenuCommand>& current = m_aCommands[i];
		AppendMenu(pData->m_hMenu, MF_STRING, current->m_id, current->m_sName.GetArray());
	}

	AppendMenu(pParentData->m_hMenu,MF_POPUP,UINT_PTR(pData->m_hMenu),m_sName.GetArray());
#endif
	return TRUE;
}