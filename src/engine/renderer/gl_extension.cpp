#include "gl_extension.h"

#ifdef WIN32
#define GL_EXT_FUNCTION(_name,_type) _type _name;
#include "gl_extension_list.h"
#undef GL_EXT_FUNCTION

Bool	GL_EXTENSION_Init()
{
	Double version =  atof((const Char*)glGetString(GL_VERSION));
	Str vendor = (const Char*)glGetString(GL_VENDOR);
	Str renderer = (const Char*)glGetString(GL_RENDERER);
	StrArray extensions;
	Str extline = (const Char*)glGetString(GL_EXTENSIONS);
	extline.Split(" ",extensions);

	LOGGER_Log("OPENGL %.1f\n",version);
	LOGGER_Log("Graphic Card %s %s\n",vendor.GetArray(),renderer.GetArray());
	
	#undef GL_EXT_VERSION
	#undef GL_EXT_EXTENSION
	#define GL_EXT_VERSION(_v) DEBUG_RequireMessage(version>=_v,"Require at least OpenGL %f",_v);
	#define GL_EXT_EXTENSION(_ext) DEBUG_RequireMessage(extensions.HasItem(#_ext),"Require OpenGL extension %s",#_ext);
	#define GL_EXT_FUNCTION(_name,_type) _name = (_type)wglGetProcAddress(#_name);
	#include "gl_extension_list.h"
	#undef GL_EXT_FUNCTION
	
	return TRUE;
}
#else
Bool	GL_EXTENSION_Init()
{
    return TRUE;
}
#endif