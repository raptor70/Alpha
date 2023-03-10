#ifndef __LOADER_LUAH__
#define __LOADER_LUAH__

#include "loader.h"

BEGIN_LOADER_CLASS(LUA)
public:
	Loader_LUA();
	virtual ~Loader_LUA();

	virtual Bool LoadToResources(const Str& _path, ResourceRefArray& _outResources);
END_LOADER_CLASS

#endif
