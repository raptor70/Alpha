#include "component_factory.h"

#include "file/base_file.h"

SINGLETON_Define(EntityComponentFactory);

EntityComponentFactory::~EntityComponentFactory()
{
	for(U32 i=0; i<m_daClasses.GetCount(); i++)
	{
		m_daClasses[i]->m_Function = NULL;
		m_daClasses[i]->m_nName = "";
	}
}

void EntityComponentFactory::InitAll()
{
	for (U32 i = 0; i < m_daClasses.GetCount(); i++)
		m_daClasses[i]->InitRegister();
}

EntityComponent* EntityComponentFactory::GetNewComponent(const Name& _name)
{
	for(U32 i=0; i<m_daClasses.GetCount(); i++)
	{
		if( _name == m_daClasses[i]->m_nName )
		{
			return m_daClasses[i]->m_Function();
		}
	}

	return NULL;
}

const EntityComponentFactory::Register* EntityComponentFactory::GetComponentRegister(const Name& _name)
{
	for (U32 i = 0; i<m_daClasses.GetCount(); i++)
	{
		if (_name == m_daClasses[i]->m_nName)
		{
			return m_daClasses[i];
		}
	}

	return NULL;
}

void	EntityComponentFactory::GenerateDoc(const Str& _path)
{
	OldFile newFile(_path, OldFile::USE_TYPE_Write);
	newFile.Open(TRUE);

	// entity params
	Str line;
	{
		newFile.WriteTextLine("ENTITY");
		Entity entity("");
		ParamsTable table;
		entity.InitParams(table);
		for (U32 p = 0; p < table.GetNbValues(); p++)
		{
			const Params* param = table.GetValue(p);
			GenerateDocParamLine(param, line);
			newFile.WriteTextLine(line);
		}
	}

	for (U32 i = 0; i < m_daClasses.GetCount(); i++)
	{
		Register* reg = m_daClasses[i];
		
		// component name
		line.SetFrom("%s", (Char*)reg->m_nName.GetStr());
		newFile.WriteTextLine(line);

		// component params
		ParamsTable table;
		EntityComponentRef newComponent = reg->m_Function();
		newComponent->InitParams(table);
		for (U32 p = 0; p < table.GetNbValues(); p++)
		{
			const Params* param = table.GetValue(p);
			GenerateDocParamLine(param, line);
			newFile.WriteTextLine(line);
		}
	}
	newFile.Close();
}

void	EntityComponentFactory::GenerateDocParamLine(const Params* _param, Str& _line)
{
	Str lineType;
	switch (_param->GetType())
	{
	case Params::TYPE_Bool :
	{
		lineType.SetFrom("Bool(%s)", _param->GetBool()?"true":"false");
		break;
	}
	case Params::TYPE_Float :
	{
		lineType.SetFrom("Float(%.3f)", _param->GetFloat());
		break;
	}
	case Params::TYPE_S32 :
	{
		lineType.SetFrom("S32(%d)",_param->GetS32());
		break;
	}
	case Params::TYPE_Vec2f:
	{
		lineType.SetFrom("Vec3f(%.3f,%.3f,%.3f)",
			_param->GetVec2f().x,
			_param->GetVec2f().y);
		break;
	}
	case Params::TYPE_Vec3f:
	{
		lineType.SetFrom("Vec3f(%.3f,%.3f,%.3f)",
			_param->GetVec3f().x,
			_param->GetVec3f().y,
			_param->GetVec3f().z);
		break;
	}
	case Params::TYPE_Vec4f:
	{
		lineType.SetFrom("Vec4f(%.3f,%.3f,%.3f,%.3f)",
			_param->GetVec4f().x,
			_param->GetVec4f().y,
			_param->GetVec4f().z,
			_param->GetVec4f().w);
		break;
	}
	case Params::TYPE_Str:
	{
		lineType.SetFrom("Str(\"%s\")",(Char*)_param->GetStr());
		break;
	}
	case Params::TYPE_Color:
	{
		lineType.SetFrom("Color(%d,%d,%d,%d)",
			S32(_param->GetColor().r * 255.f),
			S32(_param->GetColor().g * 255.f),
			S32(_param->GetColor().b * 255.f),
			S32(_param->GetColor().a * 255.f));
		break;
	}
	default:
	{	
		lineType.SetFrom("UnknownType");
		break;
	}
	}

	_line.SetFrom("\t-\t%s = %s", (Char*)_param->GetName().GetStr(), (Char*)lineType);
}