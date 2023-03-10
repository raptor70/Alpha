#include "script.h"

#include "script_register.h"
#include "file/base_file.h"

void* scriptAlloc(void *ud, void *ptr, size_t osize, size_t nsize)
{
	if (nsize == 0) 
	{
		DELETE[] (U8*)ptr;
		return NULL;
	}
	else
	{
		void* result = NEW U8[nsize];
		if( ptr )
		{
			memcpy(result,ptr,Min(nsize,osize));
			DELETE[] (U8*)ptr;
		}

		return result;
	}
}

//-------------------------------------

int scriptMessage(lua_State *L)
{
    int nArgs = lua_gettop(L);
    
	for(S32 i = 1; i <= nArgs; i++)
    {
		lua_getglobal(L, "tostring");
		lua_pushvalue(L,i);
		lua_call(L,1,1);
		const Char* str= lua_tostring(L,-1);
		LOGGER_Log(str);
		lua_pop(L,1);
    };
    
	LOGGER_Log("\n");
    return 0;
}

//-------------------------------------

int scriptError(lua_State *L)
{
#ifdef USE_LOGGER
	Str error = lua_tostring(L, -1);
	if (!error.IsEmpty())
	{
		StrArray values;
		error.Split(":", values);

		// get script pointer
		lua_getglobal(L, "this");
		Script* pScript = (Script*)lua_touserdata(L, -1);
		lua_pop(L, -1);

		// require path
		Str path;
		const Str& requirePath = pScript->GetCurrentRequirePath();
		if (requirePath.GetLength() > 0)
			path = requirePath;
		else
			path = pScript->GetPath();

		OldFile::SetPathNearExe(path);

		Logger::GetInstance().Log(eLOG_Error, path.GetArray(), values[1].GetInteger(), "%s\n", values[2].GetArray());
	}
#endif
	return 0;
}

//-------------------------------------

int scriptRequire(lua_State *L)
{
    DEBUG_Require(lua_gettop(L) == 1);

    Str str= lua_tostring(L,-1);
	str += ".lua";

	// get script pointer
	lua_getglobal(L,"this");
	Script* pScript = (Script*)lua_touserdata(L,-1);
	lua_pop(L,-1);
	{
		pScript->SetCurrentRequirePath(str);

		S8Array		buffer;
		OldFile file(str,OldFile::USE_TYPE_ReadBuffer);
		file.Open();
		if( !file.IsOpen() )
		{
			LOGGER_LogError("Unable to find %s !!\n",str.GetArray());
			scriptError(L);
			pScript->SetCurrentRequirePath("");
			return 0;
		}
		S32 size = (S32)file.GetSize();
		buffer.SetItemCount(size+1);
		file.ReadByte(buffer.GetPointer(),size);
		buffer[size] = 0;
		file.Close();
		if( luaL_loadbuffer(L, buffer.GetPointer(), size, buffer.GetPointer()) != LUA_OK )
		{
			scriptError(L);
			pScript->SetCurrentRequirePath("");
			return 0;
		}
	}

	if( lua_pcall(L,0,0,0) != LUA_OK )
	{
		scriptError(L);
		pScript->SetCurrentRequirePath("");
		return 0;
	}

	pScript->SetCurrentRequirePath("");
    return 1;
}

//-------------------------------------

Script::Script()
{
	m_Path = "";
	m_pState = NULL;
}

//-------------------------------------

Script::~Script()
{
	Reset();
}

//-------------------------------------

Bool Script::LoadFromFile(const Str & _path)
{
	m_Path = _path;
	S8Array		buffer;
	OldFile file(_path.GetArray());
	file.Open();
	if (file.IsOpen())
	{	
		m_pState = lua_newstate(scriptAlloc, NULL);

		luaL_openlibs(m_pState);
		lua_register(m_pState, "print", scriptMessage);
		lua_register(m_pState, "require", scriptRequire);

		lua_pushinteger(m_pState, 1);
#ifdef WIN32
		lua_setglobal(m_pState, "WINDOWS");
#elif defined(IOS)
		lua_setglobal(m_pState, "IOS");
#elif defined(ANDROID)
		lua_setglobal(m_pState, "ANDROID");
#endif
		lua_pop(m_pState, -1);

		ScriptRegisterManager::GetInstance().Register(m_pState);

		lua_pushlightuserdata(m_pState, this);
		lua_setglobal(m_pState, "this");
		
		U32 size = (U32)file.GetSize();
		buffer.SetItemCount(size + 1);
		file.ReadByte(buffer.GetPointer(), size);
		buffer[size] = 0;
		file.Close();
		//LOGGER_Log("Buffer %s\n",buffer.GetPointer());
		if (luaL_loadbuffer(m_pState, buffer.GetPointer(), size, buffer.GetPointer()) != LUA_OK)
		{
			LOGGER_LogError("Unable to parse script %s !!\n", _path.GetArray());
			scriptError(m_pState);
			lua_close(m_pState);
			m_pState = NULL;
			return FALSE;
		}
	}
	else
	{
		LOGGER_LogError("Unable to find %s !!\n",_path.GetArray());
		return FALSE;
	}

	return TRUE;
}

//-------------------------------------

Bool Script::Execute()
{
	if( lua_pcall(m_pState,0,0,0) != LUA_OK )
	{
		scriptError(m_pState);
		return FALSE;
	}

	Clean();

	return TRUE;
}

//-------------------------------------

void Script::Reset()
{
	Clean();

	for (U32 i = 0; i<m_localIntances.GetCount(); i++)
	{
		DELETE[](U8*)(m_localIntances[i]);
	}
	m_localIntances.Flush();

	if( m_pState )
	{
		lua_close(m_pState);
		m_pState = NULL;
	}
}

//-------------------------------------

void Script::Clean()
{
	if(m_pState)
		lua_gc(m_pState,LUA_GCCOLLECT,0);
}

//-------------------------------------

Bool Script::HasFunction(const Char* _function)
{
	if (!m_pState)
		return FALSE;

	lua_getglobal(m_pState, _function);
	Bool ok = !(lua_isnil(m_pState,-1));
	lua_pop(m_pState,1);
	return ok;
}
