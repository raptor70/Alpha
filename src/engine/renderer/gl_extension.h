#ifndef __GL_EXTENSION_H__
#define __GL_EXTENSION_H__

#ifdef WIN32
#include "gl_include.h"
#include "glext.h"
#include "wglext.h"

#define GL_EXT_VERSION(_v)
#define GL_EXT_EXTENSION(_ext)
#define GL_EXT_FUNCTION(_name,_type) extern _type _name;
#include "gl_extension_list.h"
#undef GL_EXT_FUNCTION

#endif

Bool	GL_EXTENSION_Init();

#endif