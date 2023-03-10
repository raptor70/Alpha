#ifndef __SHADER_H__
#define __SHADER_H__

#include "resource/resource.h"

enum ShaderType
{
	SHADER_None = -1,
	SHADER_Vertex = 0,
	SHADER_Pixel,
	SHADER_Header,

	SHADER_Count
};

BEGIN_SUPER_CLASS(ShaderSource,Resource)
public:
	ShaderSource();
	virtual ~ShaderSource();

	static  const Char* GetResourcePrefix() { return "shadersource"; }

	void	Init(ShaderType _type, const Str& _source)		{ m_Type = _type; m_Source = _source; }
	
	const Str&	GetSource() const						{ return m_Source; }
	ShaderType	GetType() const							{ return m_Type; }
protected:
	ShaderType	m_Type;
	Str			m_Source;
END_SUPER_CLASS

BEGIN_SUPER_CLASS(Shader, Resource)
public:
	static  const Char* GetResourcePrefix() { return "shader"; }

	void	Init(ResourceRef _source) { m_pSource = _source; }
	ResourcePtr GetSource() const { return m_pSource; }

	void	AddFlags(const Str& _flag) { m_Flags.AddLastItem(_flag); }
	const StrArray& GetFlags() const { return m_Flags; }

protected:
	ResourceRef	m_pSource;
	StrArray	m_Flags;
	END_SUPER_CLASS

#endif