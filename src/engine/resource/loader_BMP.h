#ifndef __LOADER_BMP_H__
#define __LOADER_BMP_H__

#include "loader.h"

BEGIN_LOADER_CLASS(BMP)
public:
	Loader_BMP();
	virtual ~Loader_BMP();

	virtual Bool LoadToResources(const Str& _path, ResourceRefArray& _outResources);
protected:
END_LOADER_CLASS

#endif
