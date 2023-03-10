#ifndef __LOADER_FACTORY_H__
#define __LOADER_FACTORY_H__

class Loader;
typedef	Loader* (*CreateLoaderFunction)();

class LoaderFactory
{
	SINGLETON_Declare(LoaderFactory);

	~LoaderFactory();

public:
	struct Register
	{
		Name m_nExt;
		CreateLoaderFunction m_Function;

		Register() {}

		Register(const Name&	_ext,CreateLoaderFunction _function )
		{
			m_nExt = _ext;
			m_Function = _function;
			LoaderFactory::GetInstance().m_daClasses.AddLastItem(this);
		}
	};
private:
	ArrayOf<Register*>	m_daClasses;

public:
	Loader* GetNewLoader(const Name& _ext)
	{
		for(U32 i=0; i<m_daClasses.GetCount(); i++)
		{
			if( _ext == m_daClasses[i]->m_nExt )
			{
				return m_daClasses[i]->m_Function();
			}
		}

		return NULL;
	}
};

#define BEGIN_LOADER_CLASS(_name)										\
	DECLARE_LOADER_CLASS(Loader_##_name,Loader)

#define BEGIN_LOADER_CLASS_INHERITED(_name,_otherName)					\
	DECLARE_LOADER_CLASS(Loader_##_name,Loader_##_otherName)

#define DECLARE_LOADER_CLASS(_className,_parentClassName)				\
	BEGIN_SUPER_CLASS(_className,_parentClassName)						\
	private :															\
		static LoaderFactory::Register	g_Register;						\
		static Loader* Create() { return NEW _className(); } 

#define END_LOADER_CLASS												\
	END_SUPER_CLASS

#define DEFINE_LOADER_CLASS(_name)										\
	LoaderFactory::Register Loader_##_name::g_Register(#_name,Loader_##_name::Create);

#endif