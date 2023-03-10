#include "lib_include.h"

#define _CRT_SECURE_NO_WARNINGS

#pragma warning( disable : 4700 )

#define FT2_BUILD_LIBRARY
#include INCLUDE_LIB(LIB_FREETYPE , src\base\ftbase.c)
#include INCLUDE_LIB(LIB_FREETYPE , src\base\ftbitmap.c)
#include INCLUDE_LIB(LIB_FREETYPE , src\base\ftdebug.c)
#include INCLUDE_LIB(LIB_FREETYPE , src\base\ftglyph.c)
#include INCLUDE_LIB(LIB_FREETYPE , src\base\ftinit.c)
#include INCLUDE_LIB(LIB_FREETYPE , src\base\ftsystem.c)
#include INCLUDE_LIB(LIB_FREETYPE , src\gzip\ftgzip.c)
#include INCLUDE_LIB(LIB_FREETYPE , src\psnames\psnames.c)
#include INCLUDE_LIB(LIB_FREETYPE , src\sfnt\sfnt.c)
#include INCLUDE_LIB(LIB_FREETYPE , src\smooth\smooth.c)
#include INCLUDE_LIB(LIB_FREETYPE , src\truetype\truetype.c)