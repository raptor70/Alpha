#include "shader_data.h"

ShaderData::~ShaderData()
{
	RendererDatas::GetInstance().UnregisterShaderData(this);
}