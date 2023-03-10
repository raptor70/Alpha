#ifndef __SHADER_GROUP_H__
#define __SHADER_GROUP_H__

#include "shader.h"
class RendererDriver;

BEGIN_SUPER_CLASS(ShaderGroup, Resource)
public:
	ShaderGroup();
	virtual ~ShaderGroup();

	static  const Char* GetResourcePrefix() { return "shadergroup"; }

	enum Params
	{
		PARAM_None = -1,

#define SHADER_ATTRIBUTE(_name) ATTRIBUTE_##_name,
#define SHADER_UNIFORM(_name) UNIFORM_##_name,
#define SHADER_UNIFORMDATA(_name) UNIFORMDATA_##_name,
#include "shader_params.h"
#undef SHADER_UNIFORM
#undef SHADER_UNIFORMDATA
#undef SHADER_ATTRIBUTE

		PARAM_Count
	};

	void	InitParams(RendererDriver* _pDriver);
	void	AddShader(ShaderType _type,const Str& _source, const StrArray& _flags = StrArray());
	S32		GetParamId(Params _param) const			{ return m_ParamIds[_param]; }

	Bool IsDirty() const;
	void Reload();

	ResourceRef m_Shaders[SHADER_Count];
	S32			m_ParamIds[PARAM_Count];
END_SUPER_CLASS

typedef RefTo<ShaderGroup> ShaderGroupRef;
typedef ArrayOf<ShaderGroupRef> ShaderGroupRefArray;
typedef PtrTo<ShaderGroup> ShaderGroupPtr;
typedef ArrayOf<ShaderGroupPtr> ShaderGroupPtrArray;

#endif