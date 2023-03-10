#include "loader_XML.h"

#include "file/base_file.h"

DEFINE_LOADER_CLASS(XML);

/*void rapidxml::parse_error_handler(const char *what, void *where)
{
	LOGGER_LogError("XML error : %s\n", what);
	DEBUG_Require(FALSE);
}*/

//-----------------------------------------------

void* xmlAlloc(size_t _size)
{
	return NEW U8[_size];
}

//-----------------------------------------------

void xmlFree(void* _ptr)
{
	DELETE[] (U8*)_ptr;
}

//-----------------------------------------------s

Loader_XML::Loader_XML()
{
}

//-----------------------------------------------

Loader_XML::~Loader_XML()
{
}

//-----------------------------------------------

Bool Loader_XML::LoadToResources(const Str& _path, ResourceRefArray& _outResources)
{
	OldFile file(_path,OldFile::USE_TYPE_ReadBuffer);
	file.Open();
	if( !file.IsOpen() )
		return FALSE;

	Str data;
	if( !file.ReadCompleteTextFile(data) )
		return FALSE;

	rapidxml::xml_document<> doc;  
	doc.set_allocator(xmlAlloc, xmlFree);
	doc.parse<0>((Char*)data.GetArray());

	if( !LoadToResourcesFromXml(doc,_outResources))
		return FALSE;

	file.Close();

	return TRUE;
}

Bool Loader_XML::LoadToResourcesFromXml(const rapidxml::xml_document<>& _xmlDoc, ResourceRefArray& _outResources)
{
	// todo 
	return TRUE;
}
