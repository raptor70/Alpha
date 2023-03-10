#ifndef __LOADER_H__
#define __LOADER_H__

#include "loader_factory.h"
#include "resource.h"

class Loader
{
public:
	Loader() {}
	virtual ~Loader(){}

	virtual Bool LoadToResources(const Str& _path, ResourceRefArray& _outResources) = 0;
};

typedef RefTo<Loader>			LoaderRef;
typedef ArrayOf<LoaderRef>	LoaderRefArray;

#endif