#undef __NAME_STATIC_H__
#define __NAME_DEFINE__
#define DECLARE_NAME(name)	Name g_n##name = #name;
#include "name_static.h"
#undef DECLARE_NAME

void NAME_RemoveAll()
{
#undef __NAME_STATIC_H__
#define DECLARE_NAME(name)	g_n##name = Name::Null;
#include "name_static.h"
#undef DECLARE_NAME
}