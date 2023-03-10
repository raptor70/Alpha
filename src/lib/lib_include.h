#ifndef __LIB_INCLUDE_H__
#define __LIB_INCLUDE_H__

// include libs
#define STRINGIFY_MACRO(x) STR(x)
#define STR(x) #x
#define EXPAND(x) x
#define INCLUDE_LIB(version, path) STRINGIFY_MACRO(EXPAND(version)EXPAND(/)EXPAND(path))
#define LINK_LIB(version, path)	comment( lib, STRINGIFY_MACRO(EXPAND(lib/)EXPAND(version)EXPAND(/)EXPAND(path)))

// lib versions
#define LIB_FREETYPE	freetype-2.5.5
#define LIB_SQLITE		sqlite-3.11.0
#define LIB_FMOD		fmod-1.09.04
#define LIB_VULKAN		vulkan-1.3.204
#define LIB_TINYGLTF	tinygltf-2.5.0
#define LIB_RAPIDJSON	rapidjson-1.1.0
#define LIB_IMGUI		imgui-docking-1.86
#define LIB_RAPIDXML	rapidxml-1.13
#define LIB_RENDERDOC	renderdoc-1.18
#define LIB_PTHREAD		pthreads-2.9.1
#define LIB_STEAM		steamworks-1.41
#define LIB_OPTICK		optick-1.3.0
#define LIB_PIX			winpixeventruntime-1.0.170918004
#define LIB_PNG			libpng-1.6.28
#define LIB_LUA			lua-5.3.0
#define LIB_TIFF		tiff-4.0.3
#define LIB_ZLIB		zlib-1.2.8

#endif // __LIB_INCLUDE_H__