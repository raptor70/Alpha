#include "loader_factory.h"

SINGLETON_Define(LoaderFactory);

LoaderFactory::~LoaderFactory()
{
	for(U32 i=0; i<m_daClasses.GetCount(); i++)
	{
		m_daClasses[i]->m_Function = NULL;
		m_daClasses[i]->m_nExt = "";
	}
}