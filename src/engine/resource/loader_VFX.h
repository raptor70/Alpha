#ifndef __LOADER_VFX_H__
#define __LOADER_VFX_H__

#include "loader.h"
#include "renderer/shader.h"

BEGIN_LOADER_CLASS(VFX)
public:
	Loader_VFX();
	virtual ~Loader_VFX();

	virtual Bool LoadToResources(const Str& _path, ResourceRefArray& _outResources);
	virtual ShaderType	GetShaderType() const	{ return SHADER_Vertex; }
protected:
END_LOADER_CLASS

BEGIN_LOADER_CLASS_INHERITED(PFX,VFX)
	virtual ShaderType	GetShaderType() const	{ return SHADER_Pixel; }
END_LOADER_CLASS

BEGIN_LOADER_CLASS_INHERITED(HFX, VFX)
virtual ShaderType	GetShaderType() const { return SHADER_Header; }
END_LOADER_CLASS

#endif
