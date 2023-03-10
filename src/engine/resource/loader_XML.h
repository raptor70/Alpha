#ifndef __LOADER_XML_H__
#define __LOADER_XML_H__

#include "loader.h"

BEGIN_LOADER_CLASS(XML)
public:
	Loader_XML();
	virtual ~Loader_XML();

	virtual Bool LoadToResources(const Str& _path, ResourceRefArray& _outResources);
	virtual Bool LoadToResourcesFromXml(const rapidxml::xml_document<>& _xmlDoc, ResourceRefArray& _outResources);
protected:
END_LOADER_CLASS

#endif
