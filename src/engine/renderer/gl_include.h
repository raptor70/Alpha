#ifndef __GL_INCLUDE_H__
#define __GL_INCLUDE_H__

#ifdef WIN32
	#include "platform/platform_win32.h"
	#include <gl/GL.h>
	#include <gl/GLU.h>
	#pragma comment( lib, "opengl32.lib")
	#pragma comment( lib, "glu32.lib")
#elif ANDROID
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
	#define OPENGL_ES
#elif IOS
    #include <OpenGLES/ES2/gl.h>
    #include <OpenGLES/ES2/glext.h>
    #define OPENGL_ES
#else
	#error "No OpenGL include !! "
#endif

#endif