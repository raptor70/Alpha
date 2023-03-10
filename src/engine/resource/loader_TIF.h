#ifndef __LOADER_TIF_H__
#define __LOADER_TIF_H__

#include "loader.h"

BEGIN_LOADER_CLASS(TIF)
public:
	Loader_TIF();
	virtual ~Loader_TIF();

	virtual Bool LoadToResources(const Str& _path, ResourceRefArray& _outResources);
protected:
END_LOADER_CLASS

#endif
