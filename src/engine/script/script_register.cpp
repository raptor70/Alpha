#include "script_register.h"

SINGLETON_Define(ScriptRegisterManager)


ScriptRegisterManager::~ScriptRegisterManager()
{
	for(U32 i=0;i<g_daRegisteredClass.GetCount(); i++)
	{
		g_daRegisteredClass[i]->m_daMethods.Flush();
	}

	g_daRegisteredClass.Flush();
}

//--------------------------------------------

void	ScriptRegisterManager::Register(lua_State* L)
{
	for(U32 i=0;i<g_daRegisteredClass.GetCount(); i++)
		g_daRegisteredClass[i]->Register(L);
}

//--------------------------------------------

int ScriptRegisterManager::ClassRegister::PushMethods(lua_State *L) 
{
	for(U32 i=0;i<m_daMethods.GetCount();i++)					
	{																
		Method& m = m_daMethods[i];									
		lua_pushcfunction(L, m.m_function);							
		lua_setfield(L, -2, m.m_name.GetArray());					
	}	

	return 0;
}