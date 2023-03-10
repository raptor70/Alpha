#ifndef __SCRIPT_H__
#define __SCRIPT_H__

#ifdef IOS
#include INCLUDE_LIB(LIB_LUA, src/lua.h)
#include INCLUDE_LIB(LIB_LUA, src/lualib.h)
#include INCLUDE_LIB(LIB_LUA, src/lauxlib.h)
#else
extern "C"
{
#include INCLUDE_LIB(LIB_LUA, src/lua.h)
#include INCLUDE_LIB(LIB_LUA, src/lualib.h)
#include INCLUDE_LIB(LIB_LUA, src/lauxlib.h)
}
#endif

#include "script_register.h"
#include "resource/resource.h"

int scriptError(lua_State *L);

//-----------------------------------------------------------------

BEGIN_SUPER_CLASS(Script, Resource)
private:
	lua_State*		m_pState;
	ArrayOf<void*>	m_localIntances;
	Str				m_Path;
public:
	Script();
	~Script();

	static  const Char* GetResourcePrefix() { return "script"; }

	const Str&	GetPath() const	{ return m_Path; }
	
	Bool LoadFromFile(const Str & _path);
	Bool Execute();
	void Reset();
	void Clean();

	template<typename param0Type>
	Bool Call(const Char* _function, const param0Type& _param0);
	template<typename param0Type, typename param1Type>
	Bool Call(const Char* _function, const param0Type& _param0, const param1Type& _param1);
	template<typename param0Type, typename param1Type, typename param2Type>
	Bool Call(const Char* _function, const param0Type& _param0, const param1Type& _param1, const param2Type& _param2);
	template<typename param0Type, typename param1Type, typename param2Type, typename param3Type>
	Bool Call(const Char* _function, const param0Type& _param0, const param1Type& _param1, const param2Type& _param2, const param3Type& _param3);

	template<typename resultType, typename param0Type>
	Bool Call(const Char* _function, const param0Type& _param0, resultType** _out);
	template<typename resultType, typename param0Type, typename param1Type>
	Bool Call(const Char* _function, const param0Type& _param0, const param1Type& _param1 , resultType** _out);
	template<typename resultType, typename param0Type, typename param1Type, typename param2Type>
	Bool Call(const Char* _function, const param0Type& _param0, const param1Type& _param1 , const param2Type& _param2, resultType** _out);
	template<typename resultType, typename param0Type, typename param1Type, typename param2Type, typename param3Type>
	Bool Call(const Char* _function, const param0Type& _param0, const param1Type& _param1 , const param2Type& _param2, const param3Type& _param3, resultType** _out);
	template<typename resultType, typename param0Type, typename param1Type, typename param2Type, typename param3Type, typename param4Type>
	Bool Call(const Char* _function, const param0Type& _param0, const param1Type& _param1 , const param2Type& _param2, const param3Type& _param3, const param4Type& _param4, resultType** _out);

	Bool HasFunction(const Char* _function);

	void AddLocalInstance(void* _ptr)	{ m_localIntances.AddLastItem(_ptr); }

	void	SetCurrentRequirePath(const Str& _path)	{ m_sCurrentRequirePath = _path; }
	const Str&	GetCurrentRequirePath()	{ return m_sCurrentRequirePath; }

protected:
	void	CallError(const Char* _function);

	Str		m_sCurrentRequirePath;
END_SUPER_CLASS

//--------------------------------------------

INLINE void Script::CallError(const Char* _function)
{
/*#ifdef USE_LOGGER
	Str error = lua_tostring(m_pState, -1);
	StrArray errors;
	error.Split(":", errors);

	Logger::GetInstance().Log(eLOG_Error, m_Path.GetArray(), errors[1].ToU32(),"During call of %s - %s\n", _function, errors[2].GetArray());
#endif*/
}

//--------------------------------------------

template<typename param0Type>
Bool Script::Call(const Char* _function, const param0Type& _param0)
{
	lua_getglobal(m_pState, _function);  /* function to be called */
	PushScriptValue(m_pState,_param0);
    
    if (lua_pcall(m_pState, 1, 0, 0) != 0) //(1 arguments, 0 result)
	{
		//CallError(_function);
		scriptError(m_pState);
		return FALSE;
	}

	Clean();
    
	return TRUE;
}

//--------------------------------------------

template<typename param0Type, typename param1Type>
Bool Script::Call(const Char* _function, const param0Type& _param0, const param1Type& _param1)
{
	lua_getglobal(m_pState, _function);  /* function to be called */
	PushScriptValue(m_pState,_param0);
	PushScriptValue(m_pState,_param1);
    
    if (lua_pcall(m_pState, 2, 0, 0) != 0) //(2 arguments, 0 result)
	{
		CallError(_function);
		scriptError(m_pState);
		return FALSE;
	}

	Clean();
    
	return TRUE;
}

//--------------------------------------------

template<typename param0Type, typename param1Type, typename param2Type>
Bool Script::Call(const Char* _function, const param0Type& _param0, const param1Type& _param1, const param2Type& _param2)
{
	lua_getglobal(m_pState, _function);  /* function to be called */
	PushScriptValue(m_pState,_param0);
	PushScriptValue(m_pState,_param1);
	PushScriptValue(m_pState,_param2);
    
    if (lua_pcall(m_pState, 3, 0, 0) != 0) //(3 arguments, 0 result)
	{
		CallError(_function);
		scriptError(m_pState);
		return FALSE;
	}

	Clean();
    
	return TRUE;
}

//--------------------------------------------

template<typename param0Type, typename param1Type, typename param2Type, typename param3Type>
Bool Script::Call(const Char* _function, const param0Type& _param0, const param1Type& _param1, const param2Type& _param2, const param3Type& _param3)
{
	lua_getglobal(m_pState, _function);  /* function to be called */
	PushScriptValue(m_pState,_param0);
	PushScriptValue(m_pState,_param1);
	PushScriptValue(m_pState,_param2);
	PushScriptValue(m_pState,_param3);
    
    if (lua_pcall(m_pState, 4, 0, 0) != 0) //(4 arguments, 0 result)
	{
		CallError(_function);
		scriptError(m_pState);
		return FALSE;
	}

	Clean();
    
	return TRUE;
}

//--------------------------------------------

template<typename resultType, typename param0Type>
Bool Script::Call(const Char* _function, const param0Type& _param0, resultType** _out)
{
	lua_getglobal(m_pState, _function);  /* function to be called */
	PushScriptValue(m_pState,_param0);
    
    if (lua_pcall(m_pState, 1, 1, 0) != 0) //(3 arguments, 1 result)
	{
		CallError(_function);
		scriptError(m_pState);
		return FALSE;
	}
    
	*_out = &(GetScriptValue(m_pState,-1,(resultType*)0));
	lua_pop(m_pState, 1); 

	Clean();

	return TRUE;
}

//--------------------------------------------

template<typename resultType, typename param0Type, typename param1Type>
Bool Script::Call(const Char* _function, const param0Type& _param0, const param1Type& _param1, resultType** _out)
{
	lua_getglobal(m_pState, _function);  /* function to be called */
	PushScriptValue(m_pState,_param0);
	PushScriptValue(m_pState,_param1);
    
    if (lua_pcall(m_pState, 1, 1, 0) != 0) //(3 arguments, 1 result)
	{
		CallError(_function);
		scriptError(m_pState);
		return FALSE;
	}
    
	*_out = &(GetScriptValue(m_pState,-1,(resultType*)0));
	lua_pop(m_pState, 1); 

	Clean();

	return TRUE;
}

//--------------------------------------------

template<typename resultType, typename param0Type, typename param1Type, typename param2Type>
Bool Script::Call(const Char* _function, const param0Type& _param0, const param1Type& _param1 , const param2Type& _param2, resultType** _out)
{
	lua_getglobal(m_pState, _function);  /* function to be called */
	PushScriptValue(m_pState,_param0);
	PushScriptValue(m_pState,_param1);
	PushScriptValue(m_pState,_param2);
    
    if (lua_pcall(m_pState, 3, 1, 0) != 0) //(3 arguments, 1 result)
	{
		CallError(_function);
		scriptError(m_pState);
		return FALSE;
	}
    
	*_out = &(GetScriptValue(m_pState,-1,(resultType*)0));
	lua_pop(m_pState, 1); 

	Clean();

	return TRUE;
}

//--------------------------------------------

template<typename resultType, typename param0Type, typename param1Type, typename param2Type, typename param3Type>
Bool Script::Call(const Char* _function, const param0Type& _param0, const param1Type& _param1 , const param2Type& _param2, const param3Type& _param3, resultType** _out)
{
	lua_getglobal(m_pState, _function);  /* function to be called */
	PushScriptValue(m_pState,_param0);
	PushScriptValue(m_pState,_param1);
	PushScriptValue(m_pState,_param2);
	PushScriptValue(m_pState,_param3);
    
    if (lua_pcall(m_pState, 4, 1, 0) != 0) //(4 arguments, 1 result)
	{
		CallError(_function);
		scriptError(m_pState);
		return FALSE;
	}
    
	*_out = &(GetScriptValue(m_pState,-1,(resultType*)0));
	lua_pop(m_pState, 1); 

	Clean();

	return TRUE;
}

//--------------------------------------------

template<typename resultType, typename param0Type, typename param1Type, typename param2Type, typename param3Type, typename param4Type>
Bool Script::Call(const Char* _function, const param0Type& _param0, const param1Type& _param1 , const param2Type& _param2, const param3Type& _param3, const param4Type& _param4, resultType** _out)
{
	lua_getglobal(m_pState, _function);  /* function to be called */
	PushScriptValue(m_pState,_param0);
	PushScriptValue(m_pState,_param1);
	PushScriptValue(m_pState,_param2);
	PushScriptValue(m_pState,_param3);
	PushScriptValue(m_pState,_param4);
    
    if (lua_pcall(m_pState, 5, 1, 0) != 0) //(4 arguments, 1 result)
	{
		CallError(_function);
		scriptError(m_pState);
		return FALSE;
	}
    
	*_out = &(GetScriptValue(m_pState,-1,(resultType*)0));
	lua_pop(m_pState, 1); 

	Clean();

	return TRUE;
}

//--------------------------------------------

template<typename T>
static T* GetScriptInstance(lua_State* L, S32 index) 
{
	T** value = ((T**)lua_touserdata(L,index));
	if( value )
		return *value;

	LOGGER_LogError("Lua error : bad parameter %d of class %s\n",index,T::GetScriptMetaTableName());
	return NEW T(); // crapito ca !
}

//--------------------------------------------

template<typename T>
static T& GetScriptValue(lua_State* L, S32 index, const T* dummy = NULL)
{
	return *(GetScriptInstance<T>(L,index));
}

static Float GetScriptValue(lua_State* L, S32 index, const Float* dummy = NULL)
{
	
	return Float(luaL_checknumber( L, index));
}

#define GET_SCRIPT_INT(type) \
static type GetScriptValue(lua_State* L, S32 index, const type* dummy = NULL)	\
{																				\
	return (type)luaL_checkinteger( L, index);										\
}

GET_SCRIPT_INT(S8)
GET_SCRIPT_INT(U8)
GET_SCRIPT_INT(S16)
GET_SCRIPT_INT(U16)
GET_SCRIPT_INT(S32)
GET_SCRIPT_INT(U32)
GET_SCRIPT_INT(S64)
GET_SCRIPT_INT(U64)
#undef GET_SCRIPT_INT

static Bool GetScriptValue(lua_State* L, S32 index, const Bool* dummy = NULL)
{
	if ( lua_isboolean( L, index ) )
		return lua_toboolean( L, index ) != 0;

	/// todo error
	return FALSE;
}

static Str GetScriptValue(lua_State* L, S32 index, const Str* dummy = NULL)
{	
	return luaL_checkstring(L,index);
}

static Name GetScriptValue(lua_State* L, S32 index, const Name* dummy = NULL)
{	
	return Name(luaL_checkstring(L,index));
}

//--------------------------------------------

template<typename T>
static int PushScriptValue(lua_State* L, T& value)
{
	T** inst = (T**)lua_newuserdata(L, sizeof(T*));				
	*inst = &value;
	luaL_setmetatable(L, value.GetInstanceMetaTableName());			
	return 1;
}

template<typename T, Bool _refcount>
static int PushScriptValue(lua_State* L, _PtrTo<T,_refcount>& value)
{
	T** inst = (T**)lua_newuserdata(L, sizeof(T*));
	*inst = value.GetPtr();
	luaL_setmetatable(L, value->GetInstanceMetaTableName());
	return 1;
}

template<typename T>
static int PushScriptValue(lua_State* L, T* value)
{
	if( value )
	{
		T** inst = (T**)lua_newuserdata(L, sizeof(T*));				
		*inst = value;
		luaL_setmetatable(L, value->GetInstanceMetaTableName());		
	}
	else
		lua_pushnil(L);
	return 1;
}

static int PushScriptValue(lua_State* L, Float value)
{
	lua_pushnumber(L,value);
	return 1;
}

#define PUSH_SCRIPT_INT(type) \
static int PushScriptValue(lua_State* L, type value) \
{													 \
	lua_pushinteger(L,(lua_Integer)value);			 \
	return 1;										 \
}

PUSH_SCRIPT_INT(S8)
PUSH_SCRIPT_INT(U8)
PUSH_SCRIPT_INT(S16)
PUSH_SCRIPT_INT(U16)
PUSH_SCRIPT_INT(S32)
PUSH_SCRIPT_INT(U32)
PUSH_SCRIPT_INT(S64)
PUSH_SCRIPT_INT(U64)
#undef PUSH_SCRIPT_INT

static int PushScriptValue(lua_State* L, Bool value)
{
	lua_pushboolean(L,value);
	return 1;
}

static int PushScriptValue(lua_State* L, const Str& value)
{
	lua_pushstring(L,value.GetArray());
	return 1;
}

//--------------------------------------------

template<typename T>
static int ScriptCreate(lua_State* L)
{
	T** inst = (T**)lua_newuserdata(L, sizeof(T*));				
	*inst = (T*)NEW_NAMED(T::GetScriptMetaTableName()) U8[sizeof(T)];												
	memset(*inst,0,sizeof(T));
	(**inst) = T();
	luaL_setmetatable(L, T::GetScriptMetaTableName());

	lua_getglobal(L,"this");
	Script* script = (Script*) lua_touserdata(L,-1);
	lua_pop(L,1);
	script->AddLocalInstance(*inst);

	return 1;													
}

template<typename T, typename U>
static int ScriptCreate(lua_State* L)
{
	T** inst = (T**)lua_newuserdata(L, sizeof(T*));				
	*inst = (T*)NEW_NAMED(T::GetScriptMetaTableName()) U8[sizeof(T)];
	memset(*inst,0,sizeof(T));
	(**inst) = T(GetScriptValue(L,1,(U*)0));												
	luaL_setmetatable(L, T::GetScriptMetaTableName());			

	lua_getglobal(L,"this");
	Script* script = (Script*) lua_touserdata(L,-1);
	lua_pop(L,1);
	script->AddLocalInstance(*inst);

	return 1;													
}

template<typename T, typename U1, typename U2>
static int ScriptCreate(lua_State* L)
{
	T** inst = (T**)lua_newuserdata(L, sizeof(T*));				
	*inst = (T*)NEW_NAMED(T::GetScriptMetaTableName()) U8[sizeof(T)];
	memset(*inst,0,sizeof(T));
	(**inst) = T(
		GetScriptValue(L,1,(U1*)0),
		GetScriptValue(L,2,(U2*)0));												
	luaL_setmetatable(L, T::GetScriptMetaTableName());	

	lua_getglobal(L,"this");
	Script* script = (Script*) lua_touserdata(L,-1);
	lua_pop(L,1);
	script->AddLocalInstance(*inst);

	return 1;													
}

template<typename T, typename U1, typename U2, typename U3>
static int ScriptCreate(lua_State* L)
{
	T** inst = (T**)lua_newuserdata(L, sizeof(T*));				
	*inst = (T*)NEW_NAMED(T::GetScriptMetaTableName()) U8[sizeof(T)];
	memset(*inst,0,sizeof(T));
	(**inst) = T(
		GetScriptValue(L,1,(U1*)0),
		GetScriptValue(L,2,(U2*)0),
		GetScriptValue(L,3,(U3*)0));												
	luaL_setmetatable(L, T::GetScriptMetaTableName());	

	lua_getglobal(L,"this");
	Script* script = (Script*) lua_touserdata(L,-1);
	lua_pop(L,1);
	script->AddLocalInstance(*inst);

	return 1;										
}

template<typename T, typename U1, typename U2, typename U3, typename U4>
static int ScriptCreate(lua_State* L)
{
	T** inst = (T**)lua_newuserdata(L, sizeof(T*));				
	*inst = (T*)NEW_NAMED(T::GetScriptMetaTableName()) U8[sizeof(T)];
	memset(*inst,0,sizeof(T));
	(**inst) = T(
		GetScriptValue(L,1,(U1*)0),
		GetScriptValue(L,2,(U2*)0),
		GetScriptValue(L,3,(U3*)0),
		GetScriptValue(L,4,(U4*)0));												
	luaL_setmetatable(L, T::GetScriptMetaTableName());	

	lua_getglobal(L,"this");
	Script* script = (Script*) lua_touserdata(L,-1);
	lua_pop(L,1);
	script->AddLocalInstance(*inst);

	return 1;										
}

//--------------------------------------------

#define DECLARE_SCRIPT_CLASS_FINAL(T)	DECLARE_SCRIPT_CLASS_CODE(T,)
#define DECLARE_SCRIPT_CLASS(T)			DECLARE_SCRIPT_CLASS_CODE(T,virtual)

#define DECLARE_SCRIPT_CLASS_CODE(T,TYPE)												\
public:			\
	static const Char* GetScriptMetaTableName() {return "lua"#T; }			\
	TYPE const Char* GetInstanceMetaTableName() const { return T::GetScriptMetaTableName(); } \
class ScriptRegister##T : public ScriptRegisterManager::ClassRegister		\
{																			\
public:																		\
	virtual int Register(lua_State *L)	OVERRIDE							\
	{																		\
		luaL_newmetatable(L, T::GetScriptMetaTableName());					\
																			\
		PushMethods(L);														\
																		    \
		lua_pushvalue(L, -1);    										    \
		lua_setfield(L, -1, "__index");									    \
																			\
		lua_pushstring(L, #T);    										    \
		lua_setfield(L, -2, "typeName");									\
																			\
		lua_setglobal(L, #T);												\
		lua_pop(L, -1); 													\
																			\
		return 0;															\
	}																		\
	virtual int PushMethods(lua_State *L)	OVERRIDE;						\
	static ScriptRegister##T scriptRegister;								\
};

#define BEGIN_SCRIPT_CLASS(T)												\
int T::ScriptRegister##T::PushMethods(lua_State *L)							\
{																			\
	return ScriptRegisterManager::ClassRegister::PushMethods(L);			\
}																			\
T::ScriptRegister##T T::ScriptRegister##T::scriptRegister;						\
namespace ScriptClass##T {													\
typedef T CLASS;															\
ScriptRegisterManager::ClassRegister& scriptRegister =  T::ScriptRegister##T::scriptRegister;

#define BEGIN_SCRIPT_CLASS_INHERITED(T,U)									\
int T::ScriptRegister##T::PushMethods(lua_State *L)							\
{																			\
	U::ScriptRegister##U::scriptRegister.PushMethods(L);						\
	return ScriptRegisterManager::ClassRegister::PushMethods(L);			\
}																			\
T::ScriptRegister##T T::ScriptRegister##T::scriptRegister;						\
namespace ScriptClass##T {													\
typedef T CLASS;															\
ScriptRegisterManager::ClassRegister& scriptRegister =  T::ScriptRegister##T::scriptRegister;


#define END_SCRIPT_CLASS 	}; 

#define SCRIPT_STATIC_CONSTRUCTOR													\
ScriptRegisterManager::MethodRegister constructor(scriptRegister,ScriptCreate<CLASS>,"new");

#define SCRIPT_STATIC_CONSTRUCTOR1(T)												\
ScriptRegisterManager::MethodRegister constructor1_##T(scriptRegister,ScriptCreate<CLASS,T>,"new");

#define SCRIPT_STATIC_CONSTRUCTOR2(T1,T2)											\
ScriptRegisterManager::MethodRegister constructor2_##T1##_##T2(scriptRegister,ScriptCreate<CLASS,T1,T2>,"new");

#define SCRIPT_STATIC_CONSTRUCTOR3(T1,T2,T3)										\
ScriptRegisterManager::MethodRegister constructor3_##T1##_##T2##_##T3(scriptRegister,ScriptCreate<CLASS,T1,T2,T3>,"new");

#define SCRIPT_STATIC_CONSTRUCTOR4(T1,T2,T3,T4)										\
ScriptRegisterManager::MethodRegister constructor4_##T1##_##T2##_##T3_##T4(scriptRegister,ScriptCreate<CLASS,T1,T2,T3,T4>,"new4");

#define SCRIPT_ATTRIBUTE(_attribute, T)										\
class ScriptAttribute_##_attribute											\
{																			\
public:																		\
	static int Set(lua_State* L)											\
	{																		\
		CLASS* instance = GetScriptInstance<CLASS>(L,1);					\
		instance->_attribute = GetScriptValue(L,2,(T*)0);					\
		return 1;															\
	}																		\
	static int Get(lua_State* L)											\
	{																		\
		CLASS* instance = GetScriptInstance<CLASS>(L,1);					\
		return PushScriptValue(L, instance->_attribute); 					\
	}																		\
};																			\
ScriptRegisterManager::MethodRegister attributeregisterset##_attribute(scriptRegister,ScriptAttribute_##_attribute::Set,"Set"#_attribute); \
ScriptRegisterManager::MethodRegister attributeregisterget##_attribute(scriptRegister,ScriptAttribute_##_attribute::Get,"Get"#_attribute);

#define SCRIPT_OPERATOR(Type,op, param)										\
class ScriptOperator##Type##param											\
{																			\
public:																		\
	static int Call(lua_State* L)											\
	{																		\
		CLASS& a = GetScriptValue(L,1,(CLASS*)0);						    \
		CLASS c = a op GetScriptValue(L,2,(param*)0); 						\
		return PushScriptValue(L,c);										\
	}																		\
};																			\
ScriptRegisterManager::MethodRegister operator##Type##register##param(scriptRegister,ScriptOperator##Type##param::Call,"__"#Type);

#define SCRIPT_OPERATOR_ADD		SCRIPT_OPERATOR(add,+,CLASS)
#define SCRIPT_OPERATOR_SUB		SCRIPT_OPERATOR(sub,-,CLASS)
#define SCRIPT_OPERATOR_MUL		SCRIPT_OPERATOR(mul,*,CLASS)
#define SCRIPT_OPERATOR_DIV		SCRIPT_OPERATOR(div,/,CLASS)
#define SCRIPT_OPERATOR_MOD		SCRIPT_OPERATOR(mod,%,CLASS)

#define SCRIPT_OPERATOR_ADD1(T)		SCRIPT_OPERATOR(add,+,T)
#define SCRIPT_OPERATOR_SUB1(T)		SCRIPT_OPERATOR(sub,-,T)
#define SCRIPT_OPERATOR_MUL1(T)		SCRIPT_OPERATOR(mul,*,T)
#define SCRIPT_OPERATOR_DIV1(T)		SCRIPT_OPERATOR(div,/,T)
#define SCRIPT_OPERATOR_MOD1(T)		SCRIPT_OPERATOR(mod,%,T)

#define SCRIPT_VOID_METHOD(_function)										\
class ScriptVoidMethod_##_function											\
{																			\
public:																		\
	static int Call(lua_State* L)											\
	{																		\
		CLASS* instance = GetScriptInstance<CLASS>(L,1);					\
		instance->_function();												\
		return 1;															\
	}																		\
};																			\
ScriptRegisterManager::MethodRegister voidMethodregister##_function(scriptRegister,ScriptVoidMethod_##_function::Call,#_function);

#define SCRIPT_VOID_METHOD1(_function, _t1)									\
class ScriptVoidMethod1_##_function										\
{																			\
public:																		\
	static int Call(lua_State* L)											\
	{																		\
		CLASS* instance = GetScriptInstance<CLASS>(L,1);					\
		instance->_function( 												\
			GetScriptValue(L,2,(_t1*)0));									\
		return 1;															\
	}																		\
};																			\
ScriptRegisterManager::MethodRegister voidMethod1register##_function(scriptRegister,ScriptVoidMethod1_##_function::Call,#_function);

#define SCRIPT_VOID_METHOD2(_function, _t1, _t2)							\
class ScriptVoidMethod2_##_function										\
{																			\
public:																		\
	static int Call(lua_State* L)											\
	{																		\
		CLASS* instance = GetScriptInstance<CLASS>(L,1);					\
		instance->_function( 												\
			GetScriptValue(L,2,(_t1*)0),									\
			GetScriptValue(L,3,(_t2*)0));									\
		return 1;															\
	}																		\
};																			\
ScriptRegisterManager::MethodRegister voidMethod3register##_function(scriptRegister,ScriptVoidMethod2_##_function::Call,#_function);

#define SCRIPT_VOID_METHOD3(_function, _t1, _t2, _t3)						\
class ScriptVoidMethod3_##_function										\
{																			\
public:																		\
	static int Call(lua_State* L)											\
	{																		\
		CLASS* instance = GetScriptInstance<CLASS>(L,1);					\
		instance->_function( 												\
			GetScriptValue(L,2,(_t1*)0),									\
			GetScriptValue(L,3,(_t2*)0),									\
			GetScriptValue(L,4,(_t3*)0));									\
		return 1;															\
	}																		\
};																			\
ScriptRegisterManager::MethodRegister voidMethod3register##_function(scriptRegister,ScriptVoidMethod3_##_function::Call,#_function);

#define SCRIPT_VOID_METHOD4(_function, _t1, _t2, _t3, _t4)					\
class ScriptVoidMethod4_##_function										\
{																			\
public:																		\
	static int Call(lua_State* L)											\
	{																		\
		CLASS* instance = GetScriptInstance<CLASS>(L,1);					\
		instance->_function( 												\
			GetScriptValue(L,2,(_t1*)0),									\
			GetScriptValue(L,3,(_t2*)0),									\
			GetScriptValue(L,4,(_t3*)0),									\
			GetScriptValue(L,5,(_t4*)0));									\
		return 1;															\
	}																		\
};																			\
ScriptRegisterManager::MethodRegister voidMethod4register##_function(scriptRegister,ScriptVoidMethod4_##_function::Call,#_function);

#define SCRIPT_VOID_METHOD6(_function, _t1, _t2, _t3, _t4, _t5, _t6)		\
class ScriptVoidMethod6_##_function										\
{																			\
public:																		\
	static int Call(lua_State* L)											\
	{																		\
		CLASS* instance = GetScriptInstance<CLASS>(L,1);					\
		instance->_function( 												\
			GetScriptValue(L,2,(_t1*)0),									\
			GetScriptValue(L,3,(_t2*)0),									\
			GetScriptValue(L,4,(_t3*)0),									\
			GetScriptValue(L,5,(_t4*)0),									\
			GetScriptValue(L,6,(_t5*)0),									\
			GetScriptValue(L,7,(_t6*)0));									\
		return 1;															\
	}																		\
};																			\
ScriptRegisterManager::MethodRegister voidMethod6register##_function(scriptRegister,ScriptVoidMethod6_##_function::Call,#_function);


#define SCRIPT_VOID_METHOD12(_function, _t1, _t2, _t3, _t4, _t5, _t6, _t7, _t8, _t9, _t10, _t11, _t12)					\
class ScriptVoidMethod12_##_function										\
{																			\
public:																		\
	static int Call(lua_State* L)											\
	{																		\
		CLASS* instance = GetScriptInstance<CLASS>(L,1);					\
		instance->_function( 												\
			GetScriptValue(L,2,(_t1*)0),									\
			GetScriptValue(L,3,(_t2*)0),									\
			GetScriptValue(L,4,(_t3*)0),									\
			GetScriptValue(L,5,(_t4*)0),									\
			GetScriptValue(L,6,(_t5*)0),									\
			GetScriptValue(L,7,(_t6*)0),									\
			GetScriptValue(L,8,(_t7*)0),									\
			GetScriptValue(L,9,(_t8*)0),									\
			GetScriptValue(L,10,(_t9*)0),									\
			GetScriptValue(L,11,(_t10*)0),									\
			GetScriptValue(L,12,(_t11*)0),									\
			GetScriptValue(L,13,(_t12*)0));									\
		return 1;															\
	}																		\
};																			\
ScriptRegisterManager::MethodRegister voidMethod12register##_function(scriptRegister,ScriptVoidMethod12_##_function::Call,#_function);

#define SCRIPT_RESULT_METHOD(_function,_R)									\
class ScriptResultMethod_##_function										\
{																			\
public:																		\
	static int Call(lua_State* L)											\
	{																		\
		CLASS* instance = GetScriptInstance<CLASS>(L,1);					\
		return PushScriptValue(L,instance->_function());					\
	}																		\
};																			\
ScriptRegisterManager::MethodRegister resultMethodregister##_function(scriptRegister,ScriptResultMethod_##_function::Call,#_function);

#define SCRIPT_RESULT_METHOD1(_function,_R,_t)								\
class ScriptResultMethod1_##_function										\
{																			\
public:																		\
	static int Call(lua_State* L)											\
	{																		\
		CLASS* instance = GetScriptInstance<CLASS>(L,1);				    \
		return PushScriptValue(L,										    \
			instance->_function(										    \
				GetScriptValue(L,2,(_t*)0)));								\
	}																		\
};																			\
ScriptRegisterManager::MethodRegister resultMethod1register##_function(scriptRegister,ScriptResultMethod1_##_function::Call,#_function);

#define SCRIPT_RESULT_METHOD2(_function,_R,_t0,_t1)							\
class ScriptResultMethod2_##_function										\
{																			\
public:																		\
	static int Call(lua_State* L)											\
	{																		\
		CLASS* instance = GetScriptInstance<CLASS>(L,1);				    \
		return PushScriptValue(L,										    \
			instance->_function(										    \
				GetScriptValue(L,2,(_t0*)0),								\
				GetScriptValue(L,3,(_t1*)0)));								\
	}																		\
};																			\
ScriptRegisterManager::MethodRegister resultMethod1register##_function(scriptRegister,ScriptResultMethod2_##_function::Call,#_function);

#define SCRIPT_STATIC_RESULT_METHOD(_function,_R)							\
class ScriptStaticResultMethod_##_function									\
{																			\
public:																		\
	static int Call(lua_State* L)											\
	{																		\
		return PushScriptValue(L,CLASS::_function());						\
	}																		\
};																			\
ScriptRegisterManager::MethodRegister staticResultMethodregister##_function(scriptRegister,ScriptStaticResultMethod_##_function::Call,#_function);

						


#endif
