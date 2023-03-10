#ifndef __SHADER_DESC_H__
#define __SHADER_DESC_H__

struct SShaderDesc
{
	Str	m_Name;
	Str	m_VS;
	Str m_PS;
	StrArray m_Combinations;
};

typedef ArrayOf<SShaderDesc> ShaderDescArray;

#endif