#ifndef __LOADER_PNG_H__
#define __LOADER_PNG_H__

#include "loader.h"

BEGIN_LOADER_CLASS(PNG)
public:
	Loader_PNG();
	virtual ~Loader_PNG();

	virtual Bool LoadToResources(const Str& _path, ResourceRefArray& _outResources);
protected:
END_LOADER_CLASS

#endif
