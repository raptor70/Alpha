#ifndef __COMPONENT_FACTORY_H__
#define __COMPONENT_FACTORY_H__

#include "params/params_table.h"

class EntityComponent;
typedef	EntityComponent* (*CreateComponentFunction)();

class EntityComponentFactory
{
	SINGLETON_Declare(EntityComponentFactory);

	EntityComponentFactory() { m_ComponentName = "component"; }
	~EntityComponentFactory();

public:
	struct Register
	{
		Name m_nName;
		Name m_nParentName;
		mutable const Register* m_pParentRegister;
		ParamsTable	m_DefaultParams;
		CreateComponentFunction m_Function;

		const Register* GetParentRegister() const
		{
			if (!m_pParentRegister && m_nParentName != Name::Null)
				m_pParentRegister = EntityComponentFactory::GetInstance().GetComponentRegister(m_nParentName);

			return m_pParentRegister;
		}

		void GetParamNames(NameArray& _names, NameArray& _orgComp) const
		{
			const Register* pParent = GetParentRegister();
			if (pParent)
				pParent->GetParamNames(_names, _orgComp);

			for (U32 i = 0; i < m_DefaultParams.GetNbValues(); i++)
			{
				_names.AddLastItem(m_DefaultParams.GetValue(i)->GetName());
				_orgComp.AddLastItem(m_nName);
			}
		}

#if defined(USE_EDITOR) || defined(USE_EDITOR_V2)
		struct SEditorEnum
		{
			SEditorEnum() {}
			SEditorEnum(const Name& _name) : m_nParam(_name) { }
			StrArray	m_ValueNames;
			U32Array	m_Values;
			Name		m_nParam;

			Bool operator==(const SEditorEnum& _other) { return m_nParam == _other.m_nParam; }
		};

		struct SEditorFile
		{
			enum Type
			{
				TYPE_Texture,
				TYPE_Mesh,
				TYPE_Script
			};

			SEditorFile() {}
			SEditorFile(const Name& _name) : m_nParam(_name) { }
			SEditorFile(const Name& _name, const Type& _type) : m_nParam(_name), m_Type(_type) { }

			Type	m_Type;
			Name		m_nParam;

			Bool operator==(const SEditorFile& _other) { return m_nParam == _other.m_nParam; }
		};

		Bool	FindEnumParam(const Name& _name, const EntityComponentFactory::Register::SEditorEnum** _enum)	const
		{
			const Register* pParent = GetParentRegister();
			if (pParent && pParent->FindEnumParam(_name, _enum))
				return TRUE;

			U32 idx = 0;
			if (m_aEnumParams.Find(SEditorEnum(_name), idx))
			{
				*_enum = &m_aEnumParams[idx];
				return TRUE;
			}
			return FALSE;
		}
		Bool	FindFileParam(const Name& _name, const EntityComponentFactory::Register::SEditorFile** _file)	const
		{
			const Register* pParent = GetParentRegister();
			if (pParent && pParent->FindFileParam(_name, _file))
				return TRUE;

			U32 idx = 0;
			if (m_aFileParams.Find(SEditorFile(_name), idx))
			{
				*_file = &m_aFileParams[idx];
				return TRUE;
			}
			return FALSE;
		}
		Bool		IsSoundParam(const Name& _name) const 
		{ 
			const Register* pParent = GetParentRegister();
			if (pParent && pParent->IsSoundParam(_name))
				return TRUE;

			return m_aSoundParams.Find(_name); 
		}
		void		GetRequirements(NameArray& _result) const
		{ 
			const Register* pParent = GetParentRegister();
			if (pParent)
				pParent->GetRequirements(_result);

			_result += m_aRequirements; 
		}

		Name m_nCategory;
		ArrayOf<SEditorEnum>	m_aEnumParams;
		ArrayOf<SEditorFile>	m_aFileParams;
		NameArray				m_aSoundParams;
		NameArray				m_aRequirements;
#endif

		Register() {}

		Register(const Name&	_name,CreateComponentFunction _function )
		{
			m_nName = _name;
			m_nParentName = Name::Null;
			m_pParentRegister = NULL;
#ifdef USE_EDITOR
			m_nCategory = Name::Null;
#endif
			m_Function = _function;
			EntityComponentFactory::GetInstance().m_daClasses.AddLastItem(this);
		}

		Register(const Name& _name, const Name& _Parentname, CreateComponentFunction _function) :Register(_name, _function) { m_nParentName = _Parentname; }

		virtual void InitRegister() = 0;

		void InitParams(ParamsTable& _table) const
		{
			for (U32 i = 0; i < m_DefaultParams.GetNbValues(); i++)
			{
				const Params* pCur = m_DefaultParams.GetValue(i);
				if(!_table.ExistsParam(pCur->GetName()))
					*(_table.GetOrAddValue(pCur->GetName())) = *pCur;
			}
		}
	};

	void InitAll();

	U32 GetNbComponents() const { return m_daClasses.GetCount(); }
	const Name& GetComponentName(U32 _idx) { return m_daClasses[_idx]->m_nName; }
#ifdef USE_EDITOR
	const Name& GetComponentCategoryName(U32 _idx) { return m_daClasses[_idx]->m_nCategory; }
#endif
	const Register* GetComponentRegister(const Name& _name);

	const Name& GetComponentName()	{ return m_ComponentName; }
	void	GenerateDoc(const Str& _path);
private:
	void	GenerateDocParamLine(const Params* _param, Str& _line);

	ArrayOf<Register*>	m_daClasses;
	Name m_ComponentName;

public:
	EntityComponent* GetNewComponent(const Name& _name);
};


//------------------------------

#define _DECLARE_COMPONENT_REGISTER_CLASS(_name)  \
	BEGIN_SUPER_CLASS(EntityComponent##_name##_Register,EntityComponentFactory::Register)	\
		public:																\
			EntityComponent##_name##_Register(const Name&	_compName,CreateComponentFunction _function ) : SUPER(_compName,_function){} \
			EntityComponent##_name##_Register(const Name&	_compName,const Name&	_parentName, CreateComponentFunction _function ) : SUPER(_compName,_parentName,_function){} \
			virtual void InitRegister() OVERRIDE;							\
	END_SUPER_CLASS

#define _DECLARE_COMPONENT_CLASS_BODY(_name)						  \
	public:																		\
		static const Name&		GetStaticComponentName() { return EntityComponent##_name::g_Register.m_nName; }			\
		virtual const Name&	GetComponentName() const { return EntityComponent##_name::GetStaticComponentName(); } \
		virtual void InitParams(ParamsTable& _table) OVERRIDE { SUPER::InitParams(_table); g_Register.InitParams(_table); }; \
	private :																	\
		static EntityComponent##_name##_Register	g_Register;					\
		static EntityComponent* CreateComponent() { return NEW EntityComponent##_name(); } 

#define BEGIN_COMPONENT_CLASS(_name)											\
	_DECLARE_COMPONENT_REGISTER_CLASS(_name)										\
	BEGIN_SUPER_CLASS(EntityComponent##_name,EntityComponent)					\
	_DECLARE_COMPONENT_CLASS_BODY(_name)

#define BEGIN_COMPONENT_CLASS_INHERITED(_name,_parent)							\
	_DECLARE_COMPONENT_REGISTER_CLASS(_name)										\
	BEGIN_SUPER_CLASS(EntityComponent##_name,EntityComponent##_parent)		\
	_DECLARE_COMPONENT_CLASS_BODY(_name)

#define END_COMPONENT_CLASS														\
	END_SUPER_CLASS

//------------------------------

#define DEFINE_COMPONENT_CLASS(_name)											\
	EntityComponent##_name##_Register EntityComponent##_name::g_Register(#_name,EntityComponent##_name::CreateComponent); \
	void EntityComponent##_name##_Register::InitRegister() \
	{ \
		Str currentComponentName = #_name;

#define DEFINE_COMPONENT_CLASS_INHERITED(_name,_parent)											\
	EntityComponent##_name##_Register EntityComponent##_name::g_Register(#_name,#_parent,EntityComponent##_name::CreateComponent); \
	void EntityComponent##_name##_Register::InitRegister() \
	{ \
		Str currentComponentName = #_name;

#define END_DEFINE_COMPONENT }

//------------------------------

#define INIT_ENTITY_PARAMS(_type,_name,_value)		\
	{ \
		Str	paramStr = currentComponentName;\
		paramStr += #_name;		   \
		Name paramName = paramStr; \
		if( !m_DefaultParams.ExistsParam(paramName) ) \
		{ \
			m_DefaultParams.GetOrAddValue(paramName)->Set##_type(_value); \
		} \
	}

//------------------------------

#ifdef USE_EDITOR
#define EDITOR_CATEGORY(_category)				m_nCategory = #_category;
#define EDITOR_START_ENUM_PARAM(_name)			\
{ \
	Str	paramStr = currentComponentName;\
	paramStr += #_name;		\
	EntityComponentFactory::Register::SEditorEnum& currentEnum = m_aEnumParams.AddLastItem(EntityComponentFactory::Register::SEditorEnum(paramStr)); 
#define EDITOR_ENUM_VALUE(_name,_value)			currentEnum.m_ValueNames.AddLastItem(#_name); currentEnum.m_Values.AddLastItem((U32)_value);
#define EDITOR_END_ENUM_PARAM					}

#define EDITOR_FILE_VALUE(_name,_type)			{ Str	paramStr = currentComponentName; paramStr += #_name; m_aFileParams.AddLastItem(EntityComponentFactory::Register::SEditorFile(paramStr,EntityComponentFactory::Register::SEditorFile::TYPE_##_type));}
#define EDITOR_SOUND_VALUE(_name)				{ Str	paramStr = currentComponentName; paramStr += #_name; m_aSoundParams.AddLastItem(paramStr);}
#define EDITOR_NEED_COMPONENT(_name)			{ m_aRequirements.AddLastItem(#_name);}
#else
#define EDITOR_CATEGORY(_category)				
#define EDITOR_START_ENUM_PARAM(_name)			
#define EDITOR_ENUM_VALUE(_name,_value)			
#define EDITOR_END_ENUM_PARAM					
#define EDITOR_FILE_VALUE(_name,_type)			
#define EDITOR_SOUND_VALUE(_name)				
#define EDITOR_NEED_COMPONENT(_name)			
#endif

#endif