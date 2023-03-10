#ifndef __SCRIPT_REGISTER_H__
#define __SCRIPT_REGISTER_H__

class ScriptRegisterManager
{
	SINGLETON_Declare(ScriptRegisterManager)

	~ScriptRegisterManager();

public:
	struct ClassRegister
	{
		ClassRegister()
		{
			ScriptRegisterManager::GetInstance().g_daRegisteredClass.AddLastItem(this);
		}

		virtual int Register(lua_State *L) = 0;

		virtual int PushMethods(lua_State *L) ;

		struct Method
		{
			Str				m_name;
			lua_CFunction	m_function;

			~Method()
			{
			}
		};
		ArrayOf<Method>		m_daMethods;
	};

	void	Register(lua_State* L);

	struct MethodRegister
	{
		MethodRegister(ClassRegister& r, lua_CFunction _function, const Str& _name)
		{
			ClassRegister::Method m;
			m.m_name = _name;
			m.m_function = _function;
			r.m_daMethods.AddLastItem(m);
		}
	};

	 ArrayOf<ClassRegister*> g_daRegisteredClass;
};

#endif