#include "shader_group.h"

ShaderGroup::ShaderGroup()
{
	for(S32 i=0; i<PARAM_Count; i++)
		m_ParamIds[i] = -1;
}

//----------------------------------

ShaderGroup::~ShaderGroup()
{
}

//----------------------------------

void	ShaderGroup::AddShader(ShaderType _type,const Str& _source, const StrArray& _flags)
{
	ResourceRef pShaderSource = ResourceManager::GetInstance().GetResourceFromFile<ShaderSource>(_source.GetArray());
	if (!pShaderSource.IsValid())
	{
		LOGGER_LogError("Could not shader %s\n", _source.GetArray());
		return;
	}

	// compute crc
	Str cat;
	for (const Str& f : _flags)
		cat += f;

	Str nName("%s_%x", pShaderSource->GetBaseName().GetStr().GetArray(), Name(cat).GetID());
	m_Shaders[_type] = ResourceManager::GetInstance().GetVirtualResource<Shader>(nName);
	Shader* pShader = m_Shaders[_type].GetCastPtr<Shader>();
	pShader->Init(pShaderSource);
	for (const Str& f : _flags)
		pShader->AddFlags(f);
}

//----------------------------------

void	ShaderGroup::InitParams(RendererDriver* _pDriver)
{
	#define SHADER_ATTRIBUTE(_name) m_ParamIds[ATTRIBUTE_##_name] = _pDriver->GetShaderAttribute(this,"a_"#_name);
	#define SHADER_UNIFORM(_name) m_ParamIds[UNIFORM_##_name] = _pDriver->GetShaderUniform(this,"u_"#_name);
	#define SHADER_UNIFORMDATA(_name) m_ParamIds[UNIFORMDATA_##_name] = _pDriver->GetShaderUniformData(this,"u_"#_name);
	#include "shader_params.h"
	#undef SHADER_UNIFORM
	#undef SHADER_ATTRIBUTE
	#undef SHADER_UNIFORMDATA
}

//----------------------------------

Bool ShaderGroup::IsDirty() const
{
	for (U32 i = 0; i < SHADER_Count; i++)
	{
		//if(m_ShadersDate[i] != m_Shaders[i]->GetCurrentModificationDate())
		//	return TRUE;
		const Shader* pShader = m_Shaders[i].GetCastPtr<Shader>();
		if (pShader->GetSource()->IsDirty())
			return TRUE;

	}

	return FALSE;
}

//----------------------------------

void ShaderGroup::Reload()
{
	for (U32 i = 0; i < SHADER_Count; i++)
	{
		Shader* pShader = m_Shaders[i].GetCastPtr<Shader>();

		if (m_Shaders[i].IsNull())
			continue;

		Str path = pShader->GetSource()->GetPath();
		ResourceRef newShader = ResourceManager::GetInstance().GetResourceFromFile<ShaderSource>(path);
		if (!newShader.IsValid())
		{
			LOGGER_LogError("Could not shader %s\n", path.GetArray());
		}
		else
		{
			pShader->Init(newShader);
			LOGGER_Log("Reload shader %s\n", path.GetArray());
		}
	}
}