#ifndef __LOADER_TTF_H__
#define __LOADER_TTF_H__

#include "loader.h"

BEGIN_LOADER_CLASS(TTF)
public:
	Loader_TTF();
	virtual ~Loader_TTF();

	virtual Bool LoadToResources(const Str& _path, ResourceRefArray& _outResources);
END_LOADER_CLASS

#endif
