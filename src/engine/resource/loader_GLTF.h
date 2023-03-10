#ifndef __LOADER_GLTF_H__
#define __LOADER_GLTF_H__

#include "loader.h"

BEGIN_LOADER_CLASS(GLTF)
public:
	Loader_GLTF();
	virtual ~Loader_GLTF();

	virtual Bool LoadToResources(const Str& _path, ResourceRefArray& _outResources);

	const Str& GetPath() { return m_Path; }
protected:
	Str		m_Path;
END_LOADER_CLASS

#endif

