#ifndef __LOADER_DB_H__
#define __LOADER_DB_H__

#include "loader.h"
#include "renderer/shader.h"

BEGIN_LOADER_CLASS(DB)
public:
	Loader_DB();
	virtual ~Loader_DB();

	virtual Bool LoadToResources(const Str& _path, ResourceRefArray& _outResources);
END_LOADER_CLASS

#endif
