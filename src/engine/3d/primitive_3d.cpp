#include "primitive_3d.h"

Primitive3DRef Primitive3D::Create(const Char* _pName, Bool _bIsDynamic)
{
	if (_bIsDynamic)
	{
		return ResourceManager::GetInstance().GetVirtualResource<CDynamiPrimitive3D>(_pName);
	}

	return ResourceManager::GetInstance().GetVirtualResource<CStatiPrimitive3D>(_pName);
}

Primitive3D::~Primitive3D()
{
	RendererDatas::GetInstance().UnregisterPrimitve(this);
}